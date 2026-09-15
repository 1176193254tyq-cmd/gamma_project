/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : DIDO_E1214.c
 * Project :
 * Description : E1214 DI转eth设备
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/14
 * Modified By : jia.liu
 * ------------------------------------------------------------
 * Last Modified : 2023/1/29
 * Modified By : wujinjin 修改了接收处理函数，增加了沙河DIDO部分内容
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 修改宏定义，增加case注释
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改宏定义
 * ------------------------------------------------------------
 * HISTORY : 2022/10/08 修改函数的结构
 */

#include "main.h"
#include "timed_check.h"
#include "dido_e1214.h"


extern INT16U e1214_di_data[MAX_DIDO_NUM];
extern INT16U e1214_do_set[MAX_DIDO_NUM];
extern INT16U e1214_do_read[MAX_DIDO_NUM];


/**
 * @brief 处理DIDO转eth返回数据
 * @note
        ptr-接收到的缓冲区指针
        num-DIDO序号，0代表第一台
        Tx:00 53 00 00 00 04 01 02 (01 02)    DI返回1个字节 02（0b 0000,0010）第2路离散量状态
        Rx:06 5F 00 00 00 04 01 01 (01 02)    DO返回数据长度为1，值为2 (0b 0000, 0010)
 * @retval
 */
static void DIDO_E1214_ReceDataProcess(const INT8U* ptr, const int num)
{
    //接收数据
    unsigned char pbuf[DIDO_E1214_BUFF_LEN] = {0};
    memcpy(pbuf, ptr, DIDO_E1214_BUFF_LEN);
    //初始化
    u16_conv temp;
    INT16U startaddr = 0;
    sysPara* sys_cfg = SysConf_GetInfo();
    //异常，非读取DI、DO返回
    if((pbuf[7] != DIDO_E1214_RECV_DI) && (pbuf[7] != DIDO_E1214_RECV_DO))
    {
        return;
    }
    //返回的数据起始地址
    temp.D8[1] = pbuf[0];
    temp.D8[0] = pbuf[1];
    startaddr = temp.D16;
    if((startaddr != DIDO_E1214_block1_addr) && (startaddr != DIDO_E1214_block2_addr))
    {
        return;
    }
    //DIDO通讯正常
    Set_DIDO_Comm(num, IsNoFault);
    //DI数据、DO数据
    temp.D8[1] = 0;
    temp.D8[0] = pbuf[9];
    switch(pbuf[7])
    {
        case DIDO_E1214_RECV_DI:
            e1214_di_data[num]=temp.D16;
            break;
        case DIDO_E1214_RECV_DO:
            e1214_do_read[num]=temp.D16;
            break;
    }   
}

/**
 * @brief 获取写单个线圈的值
 * @param[in] indicate 点表的指示
 * @param[in] feedback 点表的反馈
 * @return 
 * 线圈值 0xFF为ON  0x00为OFF  0x55AA为空操作
 * 
 */
static INT16U Get_WriteDo_Val(const INT16U indicate, const INT16U feedback)
{
    if(indicate == 1)          //指示开
    {
        if(feedback == 0)      //反馈关
        {
            return DIDO_E1214_RELAY_ON;
        }
    }
    else if(indicate == 0)     //指示关
    {
        if(feedback == 1)      //反馈开
        {
            return DIDO_E1214_RELAY_OFF;
        }
    }

    return DIDO_E1214_RELAY_NOP;//空操作
}




/**
 * @brief E1214根据外部控制，输出DO状态
 * @note
        fd-文件描述符
        num-DIDO序号，0代表第一台
 * @retval
 */
static void DIDO_E1214_Control(int fd, int num)
{
    sysPara* sys_cfg = SysConf_GetInfo();
    for(INT8U i=0;i<6;i++)
    {
        if(((e1214_do_set[num]>>i)&0x01) != ((e1214_do_read[num]>>i)&0x01))
        {
            if((e1214_do_set[num]>>i)&0x01)
            {
                Modbus_TCP_Write05_SingleCoil(fd, MODBUS_DEFAULT_DEV, i, DIDO_E1214_RELAY_ON, CMD_DELAY_200);
            }
            else
            {
                Modbus_TCP_Write05_SingleCoil(fd, MODBUS_DEFAULT_DEV, i, DIDO_E1214_RELAY_OFF, CMD_DELAY_200);
            }
        }
    }
}


void DIDO_E1214_Task(INT8U dido_num)
{
    //初始化
    // const int dido_num = (int)(*_num);
    sysPara* sys_cfg = SysConf_GetInfo();
    int socket_fd = 0;
    INT32U loop = 0;
    INT8U connect_cnt=0;
    ThreadConfirm(__func__);
    LOG_INFO("E1214-%d读写线程启动",dido_num);
    while (1) 
    {
        //配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->dido_port[dido_num]);
        server_addr.sin_addr.s_addr = inet_addr((char*)sys_cfg->dido_ip[dido_num]);
        struct timeval time_out = {0, (400 * 1000)};

        //创建本地客户端socket
        if ((socket_fd = Create_Client_Socket(server_addr, time_out)) == -1) 
        {
            connect_cnt=connect_cnt<=LOG_PRINTF_CNT? connect_cnt+1 : connect_cnt;        //小于12自增
            if(connect_cnt<=LOG_PRINTF_CNT)
            {
                LOG_INFO("DIDO-%d 服务端连接失败! ip:%s[port:%d]", dido_num, sys_cfg->dido_ip[dido_num], sys_cfg->dido_port[dido_num]);    
            }
            sleep(CONNECT_DELAY_TIME);
            continue;
        }
        else
        {
            connect_cnt=0;
            //DIDO通讯正常
            LOG_INFO("DIDO-%d 服务端连接成功! ip:%s[port:%d]", dido_num, sys_cfg->dido_ip[dido_num], sys_cfg->dido_port[dido_num]);
            Set_DIDO_Comm(dido_num, IsNoFault);
            int recv_bytes = 0;
            while (1) 
            {
                switch (loop++)
                {
                case 0:
                    //读取DO状态
                    Modbus_TCP_Read01_SingleCoil(socket_fd,1, DIDO_E1214_block1_addr, DIDO_E1214_block1_size,CMD_DELAY_200);
                    break;
                case 1:
                    //读取DI状态
                    Modbus_TCP_Read02_InputDiscrete(socket_fd,1, DIDO_E1214_block2_addr, DIDO_E1214_block2_size,CMD_DELAY_200);
                    break;
                case 2:
                    //设置DO状态
                    DIDO_E1214_Control(socket_fd, dido_num);
                default:
                    loop = 0;
                    break;
                }
                //接收DIDO的返回数据
                recv_bytes = Recv_Modbus_Timer(socket_fd, dido_num, DIDO_E1214_BUFF_LEN, DIDO_E1214_ReceDataProcess, Set_DIDO_Comm);
                //DIDO通讯异常或超时
                if ((recv_bytes < 0) || ((recv_bytes == 0) && (Get_DIDO_Comm(dido_num) == IsFault))) {
                    LOG_INFO("DIDO-%d: close the DIDO socket, after receive back data fault:%d", dido_num, recv_bytes);
                    close(socket_fd);
                    sleep(1);
                    break;
                }
                //轮询周期
                usleep(1000);
            } //loop R/W
        }
    } //loop connect

    close(socket_fd);
    return;
}

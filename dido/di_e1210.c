/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : di_e1210.c
 * Project :
 * Description : E1210 DI转eth设备
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/14
 * Modified By : jia.liu
 * ------------------------------------------------------------
 * HISTORY : 2023/1/29   
 * Modified By : wujinjin 重写了Recv_Modbus函数
 * ------------------------------------------------------------
 * HISTORY :  
 * ------------------------------------------------------------
 * HISTORY :  
 */

#include "main.h"
#include "timed_check.h"
#include "di_e1210.h"

extern INT16U e1210_di_data[MAX_DI_NUM];

/**
 * @brief 处理DI转eth返回数据
 * @note
        ptr-接收到的缓冲区指针
        num-DI序号，0代表第一台
 * @retval
 */
static void DI_E1210_ReceDataProcess(const INT8U* pbuf, const int num)
{
    u16_conv temp;
    INT16U startaddr = 0;
    sysPara* sys_cfg = SysConf_GetInfo();

    //读取寄存器返回
    if(pbuf[7] == 0x02)
    {
        Set_DI_Comm(num, IsNoFault);//DI通讯正常
        //返回的数据起始地址
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        //寄存器首地址
        startaddr = temp.D16;
        //0x00 ~ 0x0F，DI离散数据
        if(startaddr == DI_E1210block1_addr)
        {
            temp.D8[1] = pbuf[10];
            temp.D8[0] = pbuf[9];
            e1210_di_data[num]=temp.D16;
        }
    } 
}

   
/**
 * @brief E1210设备处理线程
 * @param[in] _item 设备序号
 */
void DI_E1210_Task(INT8U di_num)
{
    //初始化
    // const int di_num = (int)(*_item);
    sysPara* sys_cfg = SysConf_GetInfo();
    int socket_fd = 0;
    int recv_bytes = 0;
    INT8U connect_cnt=0;
    unsigned char recv_buf[DI_E1210_BUFF_LEN] = {0};
    LOG_INFO("E1210-%d读写线程启动",di_num);
    while(1)
    {
        //配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->di_port[di_num]);
        server_addr.sin_addr.s_addr = inet_addr((char*)sys_cfg->di_ip[di_num]);
        struct timeval time_out = {0, (400 * 1000)};

        //创建本地客户端socket
        if((socket_fd = Create_Client_Socket(server_addr, time_out)) == -1)
        {
            connect_cnt=connect_cnt<=LOG_PRINTF_CNT? connect_cnt+1 : connect_cnt;        //小于12自增
            if(connect_cnt<=LOG_PRINTF_CNT)
            {
                LOG_INFO("E1210-%d 服务端连接失败! ip:%s[port:%d]", di_num, sys_cfg->di_ip[di_num], sys_cfg->di_port[di_num]);    
            }
            
            sleep(CONNECT_DELAY_TIME);
            continue;
        }
        else
        {
            connect_cnt=0;
            //DI通讯正常
            LOG_INFO("E1210-%d: 服务端连接成功! ip:%s[port:%d]", di_num, sys_cfg->di_ip[di_num], sys_cfg->di_port[di_num]);
            Set_DI_Comm(di_num, IsNoFault); //清除通讯故障标志
            while(1)
            {
                //读寄存器命令
                Modbus_TCP_Read02_InputDiscrete(socket_fd, MODBUS_DEFAULT_DEV, DI_E1210block1_addr, DI_E1210block1_size, CMD_DELAY_200);
                //接收DI的返回数据
                memset((char*)recv_buf, 0, sizeof(recv_buf));
                recv_bytes = Recv_Modbus_Timer(socket_fd, di_num, DI_E1210_BUFF_LEN, DI_E1210_ReceDataProcess, Set_DIDO_Comm);
                //DIDO通讯异常或超时
                if ((recv_bytes < 0) || ((recv_bytes == 0) && (Get_DI_Comm(di_num)  == IsFault))) {
                    LOG_INFO("DI-%d: 关闭DI socket,接收数据长度为:%d", di_num, recv_bytes);
                    close(socket_fd);
                    sleep(1);
                    break;
                }           
                usleep(1000); //轮询间隔
            } //loop read
        }
    } //loop connect
    return;
}

/*
 * Copyright (C) 2022
 * File Name: eth_measure.c
 * Project:
 * Description: 测控装置，协议使用modbusTcp
 * File Created:
 * Author: lj
 * -----
 * Last Modified:
 * Modified By:XuBo
 * -----
 * HISTORY:
 */

#include "main.h"
#include "log.h"
// #include "timed.h"
#include "eth_measure.h"

/**
 * @brief 处理MEASURE返回数据
 * @note
        ptr-接收到的缓冲区指针
        eg.
        Tx:000-00 A7 00 00 00 06 01 02 04 97 00 20
        Rx:001-00 A7 00 00 00 07 01 02 04 00 00 00 10


 * @retval
 */
static void MEASURE_ReceDataProcess(unsigned char *ptr, int num)
{
    u16_conv temp;
    u32_conv temp_32bit;
    INT16U startaddr; //寄存器首地址
    unsigned char *pbuf = ptr;
    INT16U dataddr;
    sysPara *Syscfg = SysConf_GetInfo();
    //读取寄存器返回
    /*
    if (pbuf[7] == 0x03)
    {
        Set_MEASU_Comm(num, IsNoFault); // MEASURE通讯正常
        //返回的数据起始地址
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16; //寄存器首地址
        // 1108，1个遥测
        if (startaddr == MEASURE_block1_addr) // 1个寄存器
        {
            dataddr = 9;
            //油温pt100
            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            SystemTotalData[54551 + num * 100].DS16 = temp.DS16;
        }
    }
    */
    if (pbuf[7] == 0x01) //读取遥信
    {
        Set_MEASU_Comm(num, IsNoFault); // MEASURE通讯正常
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1]; //返回的数据起始地址
        startaddr = temp.D16; //寄存器首地址
        if (startaddr == MEASURE_block0_addr)
        {
            dataddr = 9;
            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //熔断器动作跳闸
            SystemTotalData[54525 + num * 100].D16 = temp.D16; // 10034

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //接地开关合闸信号
            SystemTotalData[54523 + num * 100].D16 = temp.D16; // 10035

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //隔离开关合闸信号
            SystemTotalData[54521 + num * 100].D16 = temp.D16; // 10036

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //高压负荷开关合闸信号
            SystemTotalData[54519 + num * 100].D16 = temp.D16; // 10037

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            // NULL
            // SystemTotalData[ + num * 100].D16 = temp.D16; // 10038

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //高压室开门信号
            SystemTotalData[54513 + num * 100].D16 = temp.D16; // 10039

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //低压室开门信号
            SystemTotalData[54514 + num * 100].D16 = temp.D16; // 10040
        }
        else if (startaddr == MEASURE_block2_addr)
        {
            dataddr = 9;
            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //轻瓦斯告警
            SystemTotalData[54506 + num * 100].D16 = temp.D16; // 10085

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //油位高告警
            SystemTotalData[54509 + num * 100].D16 = temp.D16; // 10086

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            // NULL
            // SystemTotalData[ + num * 100].D16 = temp.D16; // 10087

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //过温报警
            SystemTotalData[54504 + num * 100].D16 = temp.D16; // 10088

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            // NULL
            // SystemTotalData[ + num * 100].D16 = temp.D16; // 10089

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //油位低告警
            SystemTotalData[54503 + num * 100].D16 = temp.D16; // 10090
        }
        else if (startaddr == MEASURE_block4_addr)
        {
            dataddr = 9;
            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //重瓦斯跳闸
            SystemTotalData[54507 + num * 100].D16 = temp.D16; // 10109

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            // NULL
            // SystemTotalData[ + num * 100].D16 = temp.D16; // 10110

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //超高温跳闸
            SystemTotalData[54505 + num * 100].D16 = temp.D16; // 10111

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            // NULL
            // SystemTotalData[ + num * 100].D16 = temp.D16; // 10112

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //压力异常跳闸
            SystemTotalData[54508 + num * 100].D16 = temp.D16; // 10113

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //油温低跳闸
            SystemTotalData[54503 + num * 100].D16 = temp.D16; // 10114

            temp.D8[1] = pbuf[dataddr++];
            temp.D8[0] = pbuf[dataddr++];
            //熔断器动作跳闸
            SystemTotalData[54525 + num * 100].D16 = temp.D16; // 10115
        }
    }
    /*
    else if (pbuf[7] == 0x05)
    {
        Set_MEASU_Comm(num, IsNoFault); // MEASURE通讯正常
        //返回的寄存器地址
        temp.D8[1] = pbuf[8];
        temp.D8[0] = pbuf[9];
        startaddr = temp.D16;
        //高压断路器遥控合闸
        if (startaddr == MEASURE_block3_addr)
        {
            temp.D8[1] = pbuf[10];
            temp.D8[0] = pbuf[11];
            LOG_INFO("MEASURE-ETH:MEASURE-%d Remote closing of high-voltage circuit breaker cmd callback, current status : 0x%02x", num, temp.D16);
        }
        else if (startaddr == MEASURE_block4_addr)
        {
            temp.D8[1] = pbuf[10];
            temp.D8[0] = pbuf[11];
            LOG_INFO("MEASURE-ETH:MEASURE-%d Remote closing of high-voltage circuit breaker cmd callback, current status : 0x%02x", num, temp.D16);
        }
    }
    */
}

/**
 * @brief 获取写单个线圈的值
 * @param[in] indicate 点表的指示
 * @param[in] feedback 点表的反馈
 * @return 线圈值 0xFF00为ON  0x0000为OFF
 */
static INT16U Get_Write_Val(const INT16U indicate, const INT16U feedback)
{
    if (indicate == 1) //开
    {
        if (feedback == 0) //反馈关
        {
            return RELAY_ON;
        }
    }
    else if (indicate == 0) //关
    {
        if (feedback == 1) //反馈开
        {
            return RELAY_OFF;
        }
    }
    return RELAY_ERR;
}

/**
 * @brief 根据外部控制，输出遥控
 * @note
        fd-文件描述符
 * @retval
 */
static void MEASURE_Control(int fd, int num)
{
    sysPara *Syscfg = SysConf_GetInfo();
    if (SystemTotalData[54583 + num * 100].D16 == 0x0001)
    {
        Modbus_TCP_Write05_SingleCoil(fd,MEASURE_SLAVE_ADDR, MEASURE_block3_addr, RELAY_ON, CMD_DELAY_200);
    }
    else if (SystemTotalData[54583 + num * 100].D16 == 0x0000)
    {
        Modbus_TCP_Write05_SingleCoil(fd,MEASURE_SLAVE_ADDR, MEASURE_block3_addr, RELAY_OFF, CMD_DELAY_200);
    }
    /*
    INT16U write_val = 0;
    //高压断路器遥控合闸点
    if ((write_val = Get_Write_Val(SystemTotalData[54583 + num * 100].D16, SystemTotalData[54593 + num * 100].D16)) != RELAY_ERR)
    {
        Modbus_TCP_Write05_SingleCoil(fd, MEASURE_block3_addr, write_val, CMD_DELAY_200);
        SystemTotalData[54593 + num * 100].D16 = SystemTotalData[54583 + num * 100].D16;
    }
    //高压断路器遥控分闸点
    if ((write_val = Get_Write_Val(SystemTotalData[54584 + num * 100].D16, SystemTotalData[54594 + num * 100].D16)) != RELAY_ERR)
    {
        Modbus_TCP_Write05_SingleCoil(fd, MEASURE_block4_addr, write_val, CMD_DELAY_200);
        SystemTotalData[54594 + num * 100].D16 = SystemTotalData[54584 + num * 100].D16;
    }
    */
}

void ETH_MEASURE_Task(const char *_item)
{
    int numbytes = 0;
    sysPara *Syscfg = SysConf_GetInfo();
    const char *item = _item;
    const int measure_num = (int)(*item); // bms序号 0是第一台
    unsigned char ReceBuf[64] = {0};
    struct sockaddr_in server_addr = {0};
    int Sockfd = 0;
    int Loop = 0;
    INT8U status;
    while (1)
    {
        //客户程序填充服务端的资料
        bzero(&server_addr, sizeof(server_addr)); // 初始化,置0
        server_addr.sin_family = AF_INET;         // IPV4
        server_addr.sin_port = htons(Syscfg->measure_port[measure_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)Syscfg->measure_ip[measure_num]);
        if ((Sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) // AF_INET:Internet;SOCK_STREAM:TCP
        {
            fprintf(stderr, "MEASURE Socket Error:%s\a\n", strerror(errno));
        }
        /////////////////////设置发送/接收超时///////////////
        struct timeval rec_TimeOut;
        rec_TimeOut.tv_sec = 0;
        rec_TimeOut.tv_usec = 400 * 1000;
        if (setsockopt(Sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&rec_TimeOut, sizeof(rec_TimeOut)) == -1)
        {
            printf("MEASURE-ETH:setsockopt SO_SNDTIMEO failed!\n");
            close(Sockfd);
            sleep(1);
            continue;
        }
        if (setsockopt(Sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rec_TimeOut, sizeof(rec_TimeOut)) == -1)
        {
            printf("MEASURE-ETH:setsockopt SO_RCVTIMEO failed!\n");
            close(Sockfd);
            sleep(1);
            continue;
        }
        if (connect(Sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
        {
            // LOG_INFO("MEASURE-ETH: Connect fail!");
            close(Sockfd); //关闭客户端
            sleep(1);
            continue;
        }
        else
        {
            LOG_INFO("MEASURE->ETH-%d: Connect SUCCESS ", measure_num);
            Set_MEASU_Comm(measure_num, IsNoFault);
            Loop = 0;
            while (1)
            {
                switch (Loop++)
                {
                case 0:
                    Modbus_TCP_Read01_SingleCoil(Sockfd,MEASURE_SLAVE_ADDR, MEASURE_block0_addr, MEASURE_block0_size, CMD_DELAY_200);
                    break;
                case 1:
                    Modbus_TCP_Read01_SingleCoil(Sockfd,MEASURE_SLAVE_ADDR, MEASURE_block2_addr, MEASURE_block2_size, CMD_DELAY_200);
                    break;
                case 2:
                    Modbus_TCP_Read01_SingleCoil(Sockfd,MEASURE_SLAVE_ADDR, MEASURE_block4_addr, MEASURE_block4_size, CMD_DELAY_200);
                    break;
                case 3:
                    MEASURE_Control(Sockfd, measure_num);
                    Loop = 0;
                    break;
                default:
                    Loop = 0;
                    break;
                }

                memset((char *)ReceBuf, 0, sizeof(ReceBuf));
                numbytes = recv(Sockfd, ReceBuf, 64, 0);
                if (((numbytes < 0) && (errno != EAGAIN)) || (numbytes == 0))
                {
                    if (numbytes == 0) //连接关闭
                    {
                        LOG_INFO("MEASURE->ETH-%d: LC close the socket ", measure_num);
                    }

                    if ((numbytes < 0) && (errno != EAGAIN)) //其他错误
                    {
                        LOG_INFO("MEASURE->ETH-%d: Other socket fault! ", measure_num);
                    }

                    close(Sockfd);
                    LOG_INFO("MEASURE->ETH-%d: CLose the di socket 1", measure_num);
                    break;
                }
                else if ((numbytes < 0) && (errno == EAGAIN)) //超时
                {
                    // printf("Time out(di): numbytes = %d, errno = %d\n",numbytes,errno);
                }
                else //对接收到数据进行处理, 需要对数据进行格式校验
                {
                    Set_MEASU_Comm(measure_num, IsNoFault);                         // MEASURE通讯正常
                    MEASURE_ReceDataProcess((unsigned char *)ReceBuf, measure_num); //需要增加粘包处理
                }
                if (Get_MEASU_Comm(measure_num)) //多次timeout 或者 invalid data，已经判了超时
                {
                    close(Sockfd);
                    LOG_INFO("MEASURE->ETH-%d：close the di socket 2", measure_num);
                    sleep(1);
                    break;
                }
                usleep(1000);
            } // while(1)
        }
    } // while(1)

    close(Sockfd);
    return;
}
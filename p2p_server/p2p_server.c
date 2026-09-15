/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : modbus_ems_server.c
 * Description : Modbus TCP通信协议，对接EMS的链接
 * ------------------------------------------------------------ */

#include "main.h"
#include "timed_check.h"
#include "p2p_server.h"
#include <stdlib.h>     // malloc/free
#include <string.h>     // memset/strerror
#include <errno.h>
#include <pthread.h>
#include <limits.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

extern char *const pIsCheckTime;

static void Hex_To_String(char *out, size_t out_size, const unsigned char *buf, size_t len)
{
    size_t used = strlen(out);
    if (out_size == 0 || used >= out_size - 1) {
        return;
    }
    for (size_t i = 0; i < len && used < out_size - 1; i++) {
        int n = snprintf(out + used, out_size - used, "%02x ", buf[i]);
        if (n < 0) {
            break;
        }
        if ((size_t)n >= out_size - used) {
            used = out_size - 1;
            break;
        }
        used += (size_t)n;
    }
}



/**
 * @brief 服务端收发信处理（线程入口）
 * @param[in] arg 线程入参（ClientCtx*）
 */
static void* server_handle_data(void *arg)
{

    ClientCtx *ctx = (ClientCtx*)arg;
    const int new_fd_thread = ctx ? ctx->fd : -1;
    free(ctx), ctx = NULL;

  
    static int volatile connect_num = 0;               // 统计链接的客户端数量
    static unsigned int volatile connect_time = 0;     // 统计链接的客户端总次数
    static unsigned int volatile connect_ID_min = UINT_MAX; // 链接的最小序号

    int numbytes = 0;
    u16_conv Temp;
    static unsigned short volatile power_addr[MAX_SYS_NUM][MAX_SUB_NUM] = {0};
    char buffer[TCP_SERVER_RECV_LEN] = {0};
    char Sendbuffer[TCP_SERVER_RECV_LEN] = {0};
    char LogStr[100] = {0};
    char shortBuf[16] = {0};
    char time_str[36] = {0};
    u16_conv set_sub_half;
    bool is_sub = false;
    bool is_print = false;
    INT16U i = 0;
    INT16U AddrStar = 0;
    INT16U year = 0, month = 0, day = 0, hour = 0, min = 0, second = 0;
    INT16U MStar = 0, MLen = 0;
    INT8U dev_add = 0;
    u16_conv RegVal;

    ////////////////设置接收超时////////////////
    struct timeval rec_TimeOut;
    sysPara* sys_cfg = SysConf_GetInfo();

    rec_TimeOut.tv_sec = 0;
    rec_TimeOut.tv_usec = (20 * 1000);
    if (setsockopt(new_fd_thread, SOL_SOCKET, SO_RCVTIMEO, (char *)&rec_TimeOut, sizeof(rec_TimeOut)) == -1)
    {
        LOG_INFO("P2P Set socket property failed, SO_RCVTIMEO: %s *Reason: P2P server (fd:%d) ",
                 strerror(errno), new_fd_thread);
        if (new_fd_thread > 0)
        {
            close(new_fd_thread);
    
        }
        return NULL;
    }
    connect_num++;
    connect_time++;
 
 
    LOG_INFO("P2P Client[%d] new connect success! (fd:%d) ", connect_num, new_fd_thread);
    SET_INPUT(P2P_mode,1);

    unsigned short *pTimeCnt  = (unsigned short*)malloc(sizeof(unsigned short));
    unsigned int *pConnectID  = (unsigned int*)malloc(sizeof(unsigned int));
    if (!pTimeCnt || !pConnectID) {
        if (new_fd_thread > 0) {
            close(new_fd_thread);
        }
        free(pTimeCnt);
        free(pConnectID);
        SET_INPUT(P2P_mode,0);
        return NULL;
    }
    *pTimeCnt = 0;
    *pConnectID = connect_time;

    while (1)
    {
        if ((*pConnectID) < connect_ID_min)
        {
            connect_ID_min = (*pConnectID);
        }

        // 收信
        memset(buffer, 0, sizeof(buffer));
        numbytes = recv(new_fd_thread, buffer, TCP_SERVER_RECV_LEN, 0);

        // 接收错误
        if (((numbytes < 0) && (errno != EAGAIN)) || (numbytes == 0))
        {
            // 连接关闭
            if (numbytes == 0)
            {
                LOG_INFO("P2P close the socket\n");
            }
            // 其他错误
            if ((numbytes < 0) && (errno != EAGAIN))
            {
                LOG_INFO("P2P Other socket fault!\n");
            }
            (*pTimeCnt) = 0;
            if (new_fd_thread > 0)
            {
                close(new_fd_thread);
            }
            connect_num--;
         
            LOG_INFO("P2P Client[%d] Close the TCP server socket! (fd:%d) ", connect_num, new_fd_thread);
            // 对方关闭连接，或者连接出现错误，则关闭退出线程。

                SET_INPUT(P2P_mode,0);

            break;
        }
        // 超时
        else if ((numbytes < 0) && (errno == EAGAIN))
        {
            (*pTimeCnt) += 1;
            if ((*pTimeCnt) > (50 * ((EMS_TIMEOUT_CNT / 100) + 5))) // 600s 清理僵尸进程
            {
                if (new_fd_thread > 0)
                {
                    close(new_fd_thread);
                }
                connect_num--;

                SET_INPUT(P2P_mode,0);

                LOG_INFO("P2P Client[%d] Close the TCP server socket! (fd:%d) ", connect_num, new_fd_thread);
                break;
            }
        }
     
        else
        {
            // EMS通讯正常
  
            (*pTimeCnt) = 0;

            // 接收的数据
            Sendbuffer[0] = buffer[0];
            Sendbuffer[1] = buffer[1];
            Sendbuffer[2] = buffer[2];
            Sendbuffer[3] = buffer[3];
            Sendbuffer[4] = buffer[4];
            Sendbuffer[6] = buffer[6];
            Sendbuffer[7] = buffer[7];
            Sendbuffer[8] = buffer[11] * 2;
            dev_add = buffer[6];
            MStar = 9;
            // 请求的起始地址
            Temp.D8[1] = buffer[8];
            Temp.D8[0] = buffer[9];

            // 按功能码分
            switch (buffer[7])
            {
            case 0x03:

                for (i = 0; (i < buffer[11]) && (MStar < TCP_SERVER_RECV_LEN) && (Temp.D16 < MAX_SYSTEM_TOTAL_DATA_NUM); i++)
                {
                    if(dev_add==1)
                    {
                        RegVal.D16= GET_HOLD(Temp.D16);
                        //LOG_INFO("address is[%d],value is %d",Temp.D16, RegVal.D16);
                    }
                    else
                    {
                        break;
                    }
                    Sendbuffer[MStar++] = RegVal.D8[1];
                    Sendbuffer[MStar++] = RegVal.D8[0];
                    Temp.D16++;
                }
                Sendbuffer[5] = Sendbuffer[8] + 3;
                break;

            case 0x04:
                // 读输入寄存器
                //Read input register
                for (i = 0; (i < buffer[11]) && (MStar < TCP_SERVER_RECV_LEN) && (Temp.D16 < MAX_SYSTEM_TOTAL_DATA_NUM); i++)
                {
                    if(dev_add==1)
                    {

                        RegVal.D16= GET_INPUT(Temp.D16);
                     // LOG_INFO("address is[%d],value is %d",Temp.D16, RegVal.D16);
                    }
                    else if((dev_add>=2)&&(dev_add<(2+BANK_SIZE)))
                    {
                        int bankN = dev_add-2;

                        RegVal.D16= GET_C_INPUT(bankN,Temp.D16);
                    }
                    else
                    {
                        break;
                    }

                    // if((dev_add==1)&&(Temp.D16>=30043)&&(Temp.D16<=30046))
                    // {
                    //     Sendbuffer[MStar++] = RegVal.D8[0];
                    //     Sendbuffer[MStar++] = RegVal.D8[1];
                    // }
                    // else
                    // {
                        Sendbuffer[MStar++] = RegVal.D8[1];
                        Sendbuffer[MStar++] = RegVal.D8[0];
                    // }

                    Temp.D16++;
                }
                Sendbuffer[5] = Sendbuffer[8] + 3;
                break;

            case 0xA3:
                // 写多个寄存器
                // 寄存器个数
                MLen = (buffer[10] << 8) + buffer[11];
                AddrStar = Temp.D16;
                is_sub = false;
                is_print = false;

                for (i = 0; (i < MLen) && ((14 + i * 2) < TCP_SERVER_RECV_LEN); i++)
                {

                    RegVal.D8[1] = buffer[13 + i * 2]; // （高字节在前、低字节在后）
                    RegVal.D8[0] = buffer[14 + i * 2];

                    SET_HOLD(Temp.D16, RegVal.D16);

                    Temp.D16++;
                }
                if ((false == is_sub) || (is_print == true))
                {
                    memset(LogStr, 0, sizeof(LogStr));
                    // 请求:MBAP 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L 字节长度 寄存器值（13+寄存器数量×2）
                    {
                        size_t dump_len = (size_t)(MLen * 2 + 13);
                        if (dump_len > (size_t)TCP_SERVER_RECV_LEN) {
                            dump_len = (size_t)TCP_SERVER_RECV_LEN;
                        }
                        Hex_To_String(LogStr, sizeof(LogStr), (const unsigned char *)buffer, dump_len);
                    }
                    LOG_INFO("P2P set 0xA3, address:%d, register length:%d,buffer => %s", Temp.D16, MLen, LogStr);
                }

                // 返信内容
                Sendbuffer[5] = 6;
                Sendbuffer[8] = buffer[8];
                Sendbuffer[9] = buffer[9];
                Sendbuffer[10] = buffer[10];
                Sendbuffer[11] = buffer[11];
                break;

            case 0xA4:
                // 写多个寄存器
                // 寄存器个数
                MLen = (buffer[10] << 8) + buffer[11];
                AddrStar = Temp.D16;
                is_sub = false;
                is_print = false;

                for (i = 0; (i < MLen) && ((14 + i * 2) < TCP_SERVER_RECV_LEN); i++)
                {

                    RegVal.D8[1] = buffer[13 + i * 2]; // （高字节在前、低字节在后） 
                    RegVal.D8[0] = buffer[14 + i * 2];
                    if(dev_add==1)
                    {
                        SET_INPUT(Temp.D16, RegVal.D16);
                        LOG_INFO("对点地址：%d,%d",Temp.D16,RegVal.D16);
                         LOG_INFO("SHUJU %d,%d",Temp.D16, GET_INPUT(Temp.D16));
                       
                    }
                    
                    else if((dev_add>=2)&&(dev_add<(2+BANK_SIZE)))
                    {
                        int bankN = dev_add-2;
                        SET_C_INPUT(bankN,Temp.D16, RegVal.D16);
                    }
                    Temp.D16++;
                }
                if ((false == is_sub) || (is_print == true))
                {
                    memset(LogStr, 0, sizeof(LogStr));
                    // 请求:MBAP 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L 字节长度 寄存器值（13+寄存器数量×2）
                    {
                        size_t dump_len = (size_t)(MLen * 2 + 13);
                        if (dump_len > (size_t)TCP_SERVER_RECV_LEN) {
                            dump_len = (size_t)TCP_SERVER_RECV_LEN;
                        }
                        Hex_To_String(LogStr, sizeof(LogStr), (const unsigned char *)buffer, dump_len);
                    }
                    LOG_INFO("P2P set 0xA4, address:%d, register length:%d,buffer => %s", Temp.D16, MLen, LogStr);
                }

                // 返信内容
                Sendbuffer[5] = 6;
                Sendbuffer[8] = buffer[8];
                Sendbuffer[9] = buffer[9];
                Sendbuffer[10] = buffer[10];
                Sendbuffer[11] = buffer[11];
                break;

            default:
                break;
            }

            // 返信
            write(new_fd_thread, Sendbuffer, Sendbuffer[5] + 6);

            signal(SIGPIPE, SIG_IGN);
        }


        if ((connect_num >= (MAX_CLIENT_NUM - 1)) && (connect_ID_min == (*pConnectID)))
        {
            if (new_fd_thread > 0)
            {
                close(new_fd_thread);
            }
            connect_num--;

                SET_INPUT(P2P_mode,0);

            LOG_INFO("P2P Client[%d] Close the TCP server socket! (fd:%d,connect_ID_min:%d) ",
                     connect_num, new_fd_thread, connect_ID_min);
            break;
        }
    }

    connect_ID_min = UINT_MAX;
    free(pTimeCnt);
    free(pConnectID);
    return NULL;
}

/**
 * @brief 创建本地服务端的客户通信线程
 * @param[in] *pserver_fd 服务端fd指针
 */
static void Accept_Client_Connect(int *pserver_fd)
{
    struct sockaddr_in client_addr = {0};
    socklen_t sin_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 0, sin_size);

    int client_fd = accept((*pserver_fd), (struct sockaddr *)(&client_addr), &sin_size);
    if (client_fd == -1)
    {
        return;
    }

    LOG_INFO("P2P New Client (fd:%d) connect\n", client_fd);

    pthread_t client_thread_id = 0;
    pthread_attr_t client_thread_attr;
    pthread_attr_init(&client_thread_attr);
    pthread_attr_setdetachstate(&client_thread_attr, PTHREAD_CREATE_DETACHED);

    /* 为每个连接分配上下文，传 fd */
    ClientCtx *ctx = (ClientCtx*)malloc(sizeof(ClientCtx));
    if (!ctx) {
        LOG_INFO("P2P malloc ctx failed, close fd:%d\n", client_fd);
        close(client_fd);
        return;
    }
    ctx->fd = client_fd;

    if (pthread_create(&client_thread_id, &client_thread_attr, server_handle_data, ctx) != 0)
    {
        LOG_INFO("P2P Client (fd:%d) pthread create Error:%s\n", client_fd, strerror(errno));

        close(client_fd);
        free(ctx);
        return;
    }

        LOG_INFO("P2P Client (fd:%d) Connect from %s:%d\n",
        client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        usleep(10 * 1000);
}

void Task_P2P_Server(void)
{
    // 配置socket资料
    static int sockfd_service = 0;
    static INT32U server_create_count = 0;
    sysPara *sys_cfg = SysConf_GetInfo();
    static in_port_t sin_port;
    sin_port = htons(sys_cfg->P2P_PORT);

    // 创建EMS服务端socket
    while (1)
    {
        if ((sockfd_service = Create_Server_Socket(sin_port, MAX_CLIENT_NUM)) == -1)
        {
            // 无通讯故障时打印log，防止log刷屏
            if (Get_EMS_Comm(0) == IsNoFault)
            {
                LOG_INFO("P2P Create server socket failure!, count:%d", server_create_count++);
            }
            sleep(10);
            continue;
        }
        // EMS通讯正常
        LOG_INFO("P2P server[%d] : Create socket success! wait client connect ......", ntohs(sin_port));
   
        break;
    }

    // 等待客户端的链接make

    while (1)
    {
        Accept_Client_Connect(&sockfd_service);
        usleep(100 * 1000);
    }
}

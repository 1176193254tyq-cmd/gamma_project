/* ------------------------------------------------------------
* Copyright (C) 2026
* File Name : modbus_ems_server.c
* Project :
* Description : Modbus TCP通信协议，对接EMS的链接
* File Created :tyq

* ------------------------------------------------------------

* ------------------------------------------------------------
* HISTORY : 
*/

#include "main.h"
#include "timed_check.h"
#include "modbus_tcp_server.h"
#include "pcs_globals.h"
#include <sys/time.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include "taida_lc_ems.h"

extern char *const pIsCheckTime;

extern INT8U BusType;
extern  INT16U powertestflag;
extern  INT16U actpowervalue;
extern  INT16U actpowervaddr;
INT8U PCSnum;//用于记录pcs的个数

INT8U BMSnum;//用于记录bms的个数

BOOLEAN BMS_Write_Flag;//用于记录bms是否写入标志

BOOLEAN Cem9000_Write_Flag;//用于记录测控是否写入标志

INT32U  Cem9000_Control_Value;//用于记录测控的值

INT32U Cem9000_Addr;//用于记录测控的地址

INT32U Write_Lenth;//用于记录写入的长度

INT32U Bank_Addr_Base;//用于记录BMS的基地址

INT32U PCS_Addr_Base;//用于记录PCS的基地址

INT32U Bank_addr;//用于记录BMS的地址

BOOLEAN MV_Power_Flag=false;//用于记录下发MV的功率的标志

volatile BOOLEAN Set_Time_Flag=false;//用于记录是否设置时间的标志

volatile BOOLEAN Group1_power_multi_Flag=false;//用于记录是否下发多组功率的标志

volatile BOOLEAN Group2_power_multi_Flag=false;//用于记录是否下发多组功率的标志

static pthread_mutex_t g_ems_client_mutex = PTHREAD_MUTEX_INITIALIZER;//用于记录互斥锁

static INT32U g_ems_client_num = 0;//用于记录客户端的个数

static INT32U g_ems_connect_time = 0;//用于记录客户端连接的时间
int16_t calcReactivePower(int16_t activePower,
                          int16_t powerFactor);
/**
* @brief 将二进制数据以十六进制格式追加到输出字符串中
*
* 该函数将输入的二进制缓冲区转换为十六进制字符串表示，并追加到输出字符串的末尾。
* 输出字符串会以空字符结尾，且不会超出指定的缓冲区大小。
*
* @param[out] out 输出字符串缓冲区，用于存储转换后的十六进制字符串
* @param[in] out_size 输出缓冲区的总大小（包括空字符）
* @param[in] buf 要转换的二进制数据缓冲区
* @param[in] len 要转换的二进制数据长度
*
* @note 函数会处理以下特殊字符：
*       - \t (制表符)
*       - \r (回车符)
*       - \n (换行符)
*       其他控制字符也会被正确处理
*
* @warning 如果输出缓冲区空间不足，函数会提前终止转换
*          函数不会检查输入指针是否为NULL
*/

static void Hex_To_String(char *out, size_t out_size, const unsigned char *buf, size_t len)
{
    size_t used = strlen(out);
    if (out_size == 0 || used >= out_size - 1)
    {
        return;
    }
    for (size_t i = 0; i < len && used < out_size - 1; i++)
    {

        int n = snprintf(out + used, out_size - used, "%02x ", buf[i]);
        if (n < 0)
        {
            break;
        }
        if ((size_t)n >= out_size - used)
        {
            used = out_size - 1;
            break;
        }
        used += (size_t)n;
    }
}

/**
* @brief 计算PCS编号的函数
* @param address 输入的地址值
* @return 返回计算得到的PCS编号，范围在0-3之间
*/

INT16U PCSnum_calculate(INT16U address)
{
    // 判断地址是否在12000-12033或27000-27220范围内
    if(((address >= 12000)&&(address <=(12000+33)))||((address >= 27000)&&(address <=(27000+220))))
    {

        return 0;  // 返回PCS编号0
    }
    // 判断地址是否在12300-12333范围内
    else if(((address >= 12000+300)&&(address <=(12000+33+300))))
    {

        return  1;  // 返回PCS编号1
    }
    // 判断地址是否在12600-12633或27300-27520范围内
    else if(((address >= 12000+600)&&(address <=(12000+33+600)))||((address >= 27000+300)&&(address <=(27000+220+300))))
    {

        return  2;  // 返回PCS编号2
    }
    // 判断地址是否在12900-12933范围内
    else if(((address >= 12000+900)&&(address <=(12000+33+900))))
    {

        return  3;   // 返回PCS编号3
    }
}
/**
* @brief 关闭EMS客户端连接
* @param fd 客户端文件描述符
* @note 该函数负责关闭客户端连接，并更新客户端计数器
*/

static void EMS_Close_Client(int fd)
{
    // 如果文件描述符有效（大于0），则关闭连接
    if (fd > 0)
    {
        // 关闭TCP连接，关闭读写双向通道
        shutdown(fd, SHUT_RDWR);
        // 关闭文件描述符
        close(fd);
    }

    // 加锁，保护共享资源g_ems_client_num

    pthread_mutex_lock(&g_ems_client_mutex);

    // 如果客户端数量大于0，则减少计数
    if (g_ems_client_num > 0)
    {
        g_ems_client_num--;
    }

    // 记录客户端关闭的日志信息
    LOG_INFO("EMS Client[%d] Close the TCP server socket! (fd:%d)",
    g_ems_client_num, fd);

    // 解锁

    pthread_mutex_unlock(&g_ems_client_mutex);
}
/**
* @brief TCP服务器数据处理线程函数，用于处理客户端连接和数据交互
*
* 该函数作为线程入口，处理来自EMS客户端的TCP连接请求，实现Modbus协议通信，
* 包括读取保持寄存器(0x03)、读取输入寄存器(0x04)、写多个寄存器(0x10)和
* 写单个寄存器(0x06)等功能。同时处理系统时间同步、PCS/BMS控制等逻辑。
* @note 该函数会处理以下特殊字符序列：\t(制表符)、\r(回车)、\n(换行)
* @note 线程内部维护了连接数统计(connect_num)和连接ID最小值(connect_ID_min)
* @note 超时时间设置为20ms，长时间无数据(约600s)会自动关闭连接
* @note 最大支持MAX_CLIENT_NUM个客户端同时连接
*/

static void* Server_Handle_Data(void *arg)
{

    ClientCtx *ctx = (ClientCtx*)arg;
    const INT16U new_fd_thread = ctx ? ctx->fd : -1;
    free(ctx),
    ctx = NULL;

    static INT16U volatile connect_num = 0; // 统计链接的客户端数量

    static INT16U volatile connect_time = 0;          // 统计链接的客户端总次数

    static INT16U volatile connect_ID_min = UINT_MAX; // 链接的最小序号

    static INT16U volatile connect_ID_max = 0; // 链接的最大序号

    ssize_t numbytes;//接收数据长度

    u16_conv Temp;//临时变量，用于拼接寄存器值

    INT8U buffer[TCP_SERVER_RECV_LEN] = {0};//接收数据缓冲区

    INT8U sendbuffer[TCP_SERVER_RECV_LEN] = {0};//发送数据缓冲区

    INT8U logstr[100] = {0};//日志字符串

    INT8U time_str[36] = {0};//时间字符串
    BOOLEAN is_sub = false;
    BOOLEAN is_print = false;

    INT16U i = 0;

    INT16U AddrStar = 0;

    INT16U LC_EMS_Address = 0;

    INT16U year = 0, month = 0, day = 0, hour = 0, min = 0, second = 0;

    INT16U MStar = 0, MLen = 0;

    INT8U dev_add = 0;
    INT16S MV_reactive_power=0;
    u16_conv RegVal;
    INT16S EMS_Power_Input[4] = {0};
    struct timeval rec_timeout;
    sysPara* sys_cfg = SysConf_GetInfo();

    INT16U pcsnum = 0;
    Set_EMS_Comm(0, IsNoFault, TRUE);    // EMS通讯正常,全部通信链接均判断正常
    rec_timeout.tv_sec = 0;
    rec_timeout.tv_usec = (5 * 1000);
    SYS_TIME_BUF time;
    INT16U PCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 60);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 62);//group1,从机PCS的运行状态
    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 2) & 0x1u) == 1u);//group1,从机PCS的运行

    INT16U PCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 60);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行

    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 62);//group2,主机PCS的运行状态
    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 2) & 0x1u) == 1u);//group2,从机PCS的运行
    if (setsockopt(new_fd_thread, SOL_SOCKET, SO_RCVTIMEO, (char *)&rec_timeout, sizeof(rec_timeout)) == -1)
    {
        LOG_INFO("EMS Set socket property failed, SO_RCVTIMEO: %s *Reason: EMS server (fd:%d) ", strerror(errno), new_fd_thread);
        if (new_fd_thread > 0)
        {
            close(new_fd_thread);
        }
        return;
    }

    // 1. 规范强转类型 + 分配内存

    INT16U *pTimeCnt = (INT16U *)malloc(sizeof(INT16U));

    INT32U   *pConnectID = (INT32U *)malloc(sizeof(INT32U));
    // 2. 单独判断 + 安全释放（核心修复）
    if (pTimeCnt == NULL || pConnectID == NULL)
    {
        if (new_fd_thread > 0)
        {
            close(new_fd_thread);
        }
        // 只释放 成功分配 的指针，避免崩溃
        if (pTimeCnt != NULL) {
            free(pTimeCnt);
            pTimeCnt = NULL;  // 释放后置空
        }
        if (pConnectID != NULL) {
            free(pConnectID);
            pConnectID = NULL;
        }
        // 生产环境建议加错误日志
        LOG_INFO("malloc failed");  // 打印系统级错误原因
        return;
    }
    *pTimeCnt = 0;
    connect_num++;
    connect_time++;
    *pConnectID = connect_time;
    connect_ID_max  = *pConnectID;  // 记录最新序号
    LOG_INFO("EMS Client[%d] new connect success! (fd:%d) ", connect_num, new_fd_thread);
    while (1)
    {
        if ((*pConnectID) < connect_ID_min)
        {
            connect_ID_min = (*pConnectID);
        }

        // 收信
        memset(buffer, 0, sizeof(buffer));
        memset(sendbuffer, 0, sizeof(sendbuffer));
        numbytes = recv(new_fd_thread, buffer, TCP_SERVER_RECV_LEN, 0);
        // 接收错误
        if (((numbytes < 0) && (errno != EAGAIN)) || (numbytes == 0))
        {
            // 连接关闭
            if (numbytes == 0)
            {
                LOG_INFO("EMS close the socket\n");
            }
            // 其他错误
            if ((numbytes < 0) && (errno != EAGAIN))
            {
                LOG_INFO("EMS socket recv error fd:%d errno:%d %s",new_fd_thread,errno,strerror(errno));
            }
            (*pTimeCnt) = 0;
            if (new_fd_thread > 0)
            {
                close(new_fd_thread);
            }
            connect_num--;
            LOG_INFO("EMS Client[%d] Close the TCP server socket! (fd:%d) ", connect_num, new_fd_thread);            // 对方关闭连接，或者连接出现错误，则关闭退出线程。

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
                LOG_INFO("EMS Client[%d] Close the TCP server socket! (fd:%d) ", connect_num, new_fd_thread);
                break;
            }

        }
        else     // 对接收到数据进行处理
        {
            (*pTimeCnt) = 0;
            // printf("connect_num = %d\n",connect_num);
            // 接收的数据
            sendbuffer[0] = buffer[0];
            sendbuffer[1] = buffer[1];
            sendbuffer[2] = buffer[2];
            sendbuffer[3] = buffer[3];
            sendbuffer[4] = buffer[4];
            sendbuffer[6] = buffer[6];
            sendbuffer[7] = buffer[7];
            sendbuffer[8] = buffer[11] * 2;
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
                    sendbuffer[MStar++] = RegVal.D8[1];
                    sendbuffer[MStar++] = RegVal.D8[0];
                    Temp.D16++;
                }
                sendbuffer[5] = sendbuffer[8] + 3;
                break;
                case 0x04:
                // 读输入寄存器
                //Read input register
                for (i = 0; (i < buffer[11]) && (MStar < TCP_SERVER_RECV_LEN) && (Temp.D16 < MAX_SYSTEM_TOTAL_DATA_NUM); i++)
                {
                    if(dev_add==1)
                    {

                        RegVal.D16= GET_INPUT(Temp.D16);
                        //    LOG_INFO("address is[%d],value is %d",Temp.D16, RegVal.D16);
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
                    sendbuffer[MStar++] = RegVal.D8[1];
                    sendbuffer[MStar++] = RegVal.D8[0];
                    Temp.D16++;
                }
                sendbuffer[5] = sendbuffer[8] + 3;
                break;
                case 0x10:
                // 写多个寄存器
                // 寄存器个数
                MLen = (buffer[10] << 8) + buffer[11];
                AddrStar = Temp.D16;
                is_sub = true;
                is_print = false;

                INT8U sys_num=0;
                for (i = 0; (i < MLen) && ((14 + i * 2) < TCP_SERVER_RECV_LEN); i++)
                {
                    RegVal.D8[1] = buffer[13 + i * 2]; // （高字节在前、低字节在后）
                    RegVal.D8[0] = buffer[14 + i * 2];
 //is_sub代表是否为功率控制点位，is_print代表是否打印
                if ((Temp.D16 >= 1010) && (Temp.D16 <= 1011))
                   {
                         /* 功率值发生变化才打印 */
                        if (RegVal.D16 != GET_HOLD(Temp.D16))
                        {
                            is_print = true;
                        }
                    }
                    else
                    {
                    /* 只要存在一个普通点位，就必须打印整条0x10 */
                    is_sub = false;
                    }


                    SET_HOLD(Temp.D16, RegVal.D16);
                    // if(Temp.D16==1012)
                    // {
                    //     MV_reactive_power=calcReactivePower(GET_HOLD(1010),RegVal.D16);
                    //     LOG_INFO("计算的无功功率为：%d",MV_reactive_power);
                    //     LOG_INFO("%d %d",GET_HOLD(1010),RegVal.D16);
                    //     if(master1_Run&&slave1_Run&&master2_Run&&slave2_Run)
                    //     {
                    //         LOG_INFO("在线");
                    //     }
                    //     else
                    //     {
                    //         LOG_INFO("有从站不在线");
                    //     }
                    // }
                    pcsnum =PCSnum_calculate(Temp.D16);
                    sys_num = (pcsnum>= 2) ? 1 : 0;
                    //  LOG_INFO("pcsnum=%d,sys_num=%d",pcsnum,sys_num);
                    if ((pcsnum >= 0)&&(((Temp.D16<27046+sys_num*300))||(Temp.D16>27052+sys_num*300)))
                    {
                        if(((Temp.D16<=27203+sys_num*300)&&(Temp.D16>=27200+sys_num*300))||(Temp.D16>=27208+sys_num*300&&Temp.D16<=27211+sys_num*300)||(Temp.D16>=27216+sys_num*300&&Temp.D16<=27219+sys_num*300))
                        {
                            LC_EMS_Address=LC_EMS_Convert_Address(sys_num,Temp.D16);
                            LOG_INFO("LC_EMS_Address=%d",LC_EMS_Address);
                            if(LC_EMS_Address==27103)
                            {
                                if(RegVal.D16==1){
                                    Set_In_Sys(0,SYS_EVENT_CMD_START);
                                }
                                else if(RegVal.D16==0){
                                    Set_In_Sys(0,SYS_EVENT_CMD_STOP);
                                }

                            }
                            else if(LC_EMS_Address==27403)
                            {
                                if(RegVal.D16==1){
                                    Set_In_Sys(1,SYS_EVENT_CMD_START);
                                }
                                else if(RegVal.D16==0){
                                    Set_In_Sys(1,SYS_EVENT_CMD_STOP);
                                }
                            }
                            else{
                                PcsWriteReq pcsreq;
                                pcsreq.addr     = LC_EMS_Address;
                                pcsreq.value    = RegVal.D16;
                                pcsreq.len      = 1;
                                pcsreq.is_multi = false;
                                // 去重，保留最新值
                                Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &pcsreq);//放入缓冲区中
                                LOG_INFO("pcsnum:%d,addr:%d,value:%d", pcsnum, Temp.D16, RegVal.D16);

                            }

                        }

                    }
                    else
                    {
                        PcsWriteReq pcsreq;
                        pcsreq.addr     = Temp.D16;
                        pcsreq.value    = RegVal.D16;
                        pcsreq.len      = 1;
                        pcsreq.is_multi = false;
                        // 去重，保留最新值
                        Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &pcsreq);//放入缓冲区中
                        LOG_INFO("pcsnum:%d,addr:%d,value:%d", pcsnum, Temp.D16, RegVal.D16);
                    }
                    Temp.D16++;
                }
                if ((false == is_sub) || (is_print == true))
                {
                    memset(logstr, 0, sizeof(logstr));
                    // 请求:MBAP 功能码 起始地址H 起始地址L 寄存器数量H 寄存器数量L 字节长度 寄存器值（13+寄存器数量×2）
                    {
                        size_t dump_len = (size_t)(MLen * 2 + 13);
                        if (dump_len > (size_t)TCP_SERVER_RECV_LEN) {
                            dump_len = (size_t)TCP_SERVER_RECV_LEN;
                        }
                        Hex_To_String(logstr, sizeof(logstr), (const unsigned char *)buffer, dump_len);
                    }
                    LOG_INFO("EMS set 0x10, address:%d, register length:%d,buffer => %s", Temp.D16, MLen, logstr);
                }
                // 系统对时
                if (AddrStar == SYS_SYNC_TIEM_ADDR)
                {
                    sendbuffer[5] = 6;
                    sendbuffer[8] = buffer[8];
                    sendbuffer[9] = buffer[9];
                    sendbuffer[10] = buffer[10];
                    sendbuffer[11] = buffer[11];
                    write(new_fd_thread, sendbuffer, sendbuffer[5] + 6);
                    year = (buffer[13] << 8) + buffer[14];
                    month = (buffer[15] << 8) + buffer[16];
                    day = (buffer[17] << 8) + buffer[18];
                    hour = (buffer[19] << 8) + buffer[20];
                    min = (buffer[21] << 8) + buffer[22];
                    second = (buffer[23] << 8) + buffer[24];
                    memset(time_str, 0, sizeof(time_str));
                    sprintf(time_str, "date -s '%d-%d-%d %d:%d:%d'", year, month, day, hour, min, second);
                    Do_System(time_str, 2);                          // 设置系统时间
                    Do_System("hwclock --systohc", 1);               // 同步到RTC
                    LOG_INFO("EMS synchronize time : %s", time_str); // date -s '2022-7-27 10:59:43'
                    (*pIsCheckTime) = 1;
                    Set_Time_Flag=true;

                }
                if((AddrStar<=27215+sys_num*300)&&(AddrStar>=27212+sys_num*300))
                {
                    INT16S percent[4];

                    percent[sys_num] = (int16_t)(((int32_t)GET_HOLD(27212 + sys_num * 300) * 100) / 690);
                    PcsWriteReq pcsreq;
                    pcsreq.addr     = 27000+300*sys_num + 111;
                    pcsreq.value    = percent[sys_num];
                    pcsreq.len      = 1;
                    pcsreq.is_multi = false;
                    // 去重，保留最新值
                    Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &pcsreq);//放入缓冲区中
                    LOG_INFO("pcsnum:%d,addr:%d,value:%d", pcsnum, Temp.D16, RegVal.D16);

                }

                if(BusType==D_BUS)
                {
                    if(((AddrStar>=27046)&&(AddrStar<=27052)))
                    {
                        Group1_power_multi_Flag=true;

                    }
                    else if((AddrStar>=27346)&&(AddrStar<=27352))
                    {
                        Group2_power_multi_Flag=true;
                    }
                    else if((AddrStar<=27207+sys_num*300)&&(AddrStar>=27204+sys_num*300))
                    {

                        LC_EMS_Calc_Power_Percent(sys_num);

                    }
                }
                else
                {
                    if((AddrStar<=27207+sys_num*300)&&(AddrStar>=27204+sys_num*300))
                    {

                        LC_EMS_Calc_Power_Percent(sys_num);
                        for(INT8U i=0;i<3;i++)
                        {
                            PcsWriteReq pcsreq;
                            pcsreq.addr     = 27000+300*sys_num + 50+i;
                            pcsreq.value    = GET_HOLD(27000+300*sys_num + 50+i);
                            pcsreq.len      = 1;
                            pcsreq.is_multi = false;
                            // 去重，保留最新值
                            Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &pcsreq);//放入缓冲区中
                            LOG_INFO("pcsnum:%d,addr:%d,value:%d", pcsnum, Temp.D16, RegVal.D16);
                        }

                    }
                }
                // 返信内容
                sendbuffer[5] = 6;
                sendbuffer[8] = buffer[8];
                sendbuffer[9] = buffer[9];
                sendbuffer[10] = buffer[10];
                sendbuffer[11] = buffer[11];
                break;
                case 0x06:
                {
                    if(dev_add!=1)
                    {
                        break;
                    }
                    is_sub = false;

                    // 写单个寄存器
                    RegVal.D8[1] = buffer[10];
                    RegVal.D8[0] = buffer[11];
                    SET_HOLD(Temp.D16, RegVal.D16);
                    // 回包
                    sendbuffer[5]  = buffer[5];
                    sendbuffer[8]  = buffer[8];
                    sendbuffer[9]  = buffer[9];
                    sendbuffer[10] = buffer[10];
                    sendbuffer[11] = buffer[11];

                    if (false == is_sub)
                    {
                        LOG_INFO("EMS set 0x06, address:%d -> data:%d", Temp.D16, RegVal.D16);
                    }
                    // 系统对时
                    if ((Temp.D16 >= SYS_SYNC_TIEM_ADDR)&&(Temp.D16 <= SYS_SYNC_TIEM_ADDR + 5))
                    {
                        Get_System_Time(&time);
                        switch (Temp.D16)
                        {
                            case 1500:
                            year = RegVal.D16 ;
                            month = time.month;
                            day = time.day;
                            hour = time.hour;
                            min = time.minute;
                            second = time.second;
                            break;

                            case 1501:
                            year = time.year;
                            month = RegVal.D16;
                            day = time.day;
                            hour = time.hour;
                            min = time.minute;
                            second = time.second;
                            break;

                            case 1502:
                            year = time.year;
                            month = time.month;
                            day = RegVal.D16;
                            hour = time.hour;
                            min = time.minute;
                            second = time.second;
                            break;

                            case 1503:
                            year = time.year;
                            month = time.month;
                            day = time.day;
                            hour = RegVal.D16;
                            min = time.minute;
                            second = time.second;
                            break;

                            case 1504:
                            year = time.year;
                            month = time.month;
                            day = time.day;
                            hour = time.hour;
                            min = RegVal.D16;
                            second = time.second;
                            break;

                            case 1505:
                            year = time.year;
                            month = time.month;
                            day = time.day;
                            hour = time.hour;
                            min = time.minute;
                            second = RegVal.D16;
                            break;
                            default:
                            break;

                        }
                        write(new_fd_thread, sendbuffer, sendbuffer[5] + 6);
                        memset(time_str, 0, sizeof(time_str));
                        sprintf(time_str, "date -s '%d-%d-%d %d:%d:%d'", year, month, day, hour, min, second);
                        Do_System(time_str, 2);                          // 设置系统时间
                        Do_System("hwclock --systohc", 1);               // 同步到RTC
                        //settimeofday(time_str, NULL);
                        LOG_INFO("EMS synchronize time : %s", time_str); // date -s '2022-7-27 10:59:43'
                        (*pIsCheckTime) = 1;
                        Set_Time_Flag=true;
                    }
                    // CEM9000 远方控制逻辑
                    if(Temp.D16==33200&&RegVal.D16==0xAA)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2000;
                        Cem9000_Control_Value=0xFF00;
                    }
                    else if(Temp.D16==33200&&RegVal.D16==0XEE)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2001;
                        Cem9000_Control_Value=0xFF00;
                    }
                    else if(Temp.D16==33201)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2002;
                        Cem9000_Control_Value=0xFF00;
                    }
                    else if(Temp.D16==33202&&RegVal.D16==0XEE)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2004;
                        Cem9000_Control_Value=0xFF00;
                    }
                    else if(Temp.D16==33202&&RegVal.D16==0XAA)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2003;
                        Cem9000_Control_Value=0xFF00;
                    }
                    else if(Temp.D16==33203&&RegVal.D16==0XEE)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2006;
                        Cem9000_Control_Value=0xFF00;
                    }
                    else if(Temp.D16==33203&&RegVal.D16==0XAA)
                    {
                        Cem9000_Write_Flag =true;
                        Cem9000_Addr =2005;
                        Cem9000_Control_Value=0xFF00;
                    }

                    // 主机 PCS 开/停机
                    for(INT8U sys_num=0; (sys_num < sys_cfg->sysNum); sys_num++)
                    {
                        if(sys_cfg->pcs_brand[0] == PCS_Taida)
                        {
                            if(Temp.D16 == (27000 + 300 * sys_num + 103))//主机PCS开机或者关机
                            {
                                if(RegVal.D16==1) Set_In_Sys(sys_num,SYS_EVENT_CMD_START);
                                else              Set_In_Sys(sys_num,SYS_EVENT_CMD_STOP);
                                break;
                            }
                        }

                    }
                    if(Temp.D16 == 6)
                    {
                        if(RegVal.D16==3) //PCS开机
                        {
                            Set_In_Sys(0,SYS_EVENT_CMD_START);
                            sleep(5);//MV中两组PCS同时开机，需要等待5秒
                            Set_In_Sys(1,SYS_EVENT_CMD_START);
                        }
                        if(RegVal.D16==4) //PCS关机
                        {
                            Set_In_Sys(0,SYS_EVENT_CMD_STOP);
                            Set_In_Sys(1,SYS_EVENT_CMD_STOP);
                        }
                        else if(RegVal.D16==5) //PCS待机，实际下发为0功率
                        {
                            Set_In_Sys(0,SYS_EVENT_CMD_STANDBY);
                            Set_In_Sys(1,SYS_EVENT_CMD_STANDBY);
                        }
                        else if(RegVal.D16==6) //PCS复位，必须在关机状态下复位
                        {
                            // Set_In_Sys(0,SYS_EVENT_CMD_RESET);
                            // Set_In_Sys(1,SYS_EVENT_CMD_RESET);
                        PcsWriteReq pcsreq;
                        pcsreq.addr     = 27000;
                        pcsreq.value    = 1;
                        pcsreq.len      = 1;
                        pcsreq.is_multi = false;
                        Pcs_Write_Enqueue_Dedup_By_Addr(0, &pcsreq);//放入缓冲区中
                        usleep(50);
                        //   PcsWriteReq pcsreq;
                        pcsreq.addr     = 27300;
                        pcsreq.value    = 1;
                        pcsreq.len      = 1;
                        pcsreq.is_multi = false;
                        Pcs_Write_Enqueue_Dedup_By_Addr(2, &pcsreq);//放入缓冲区中



                        }
                        break;
                    }
                    else if(Temp.D16 == 7)//黑启动模式
                    {

                        PcsWriteReq pcsreq;
                        pcsreq.addr     = 27021;
                        pcsreq.value    = RegVal.D16;
                        pcsreq.len      = 1;
                        pcsreq.is_multi = false;
                        Pcs_Write_Enqueue_Dedup_By_Addr(0, &pcsreq);//放入缓冲区中
                        usleep(50);
                        //   PcsWriteReq pcsreq;
                        pcsreq.addr     = 27321;
                        pcsreq.value    = RegVal.D16;
                        pcsreq.len      = 1;
                        pcsreq.is_multi = false;
                        Pcs_Write_Enqueue_Dedup_By_Addr(2, &pcsreq);//放入缓冲区中

                        break;
                    }
                    // if(Temp.D16==1012)
                    // {
                    //     MV_reactive_power=calcReactivePower(GET_HOLD(1010),RegVal.D16);
                    //     LOG_INFO("计算的无功功率为：%d",MV_reactive_power);
                    // }
                    // pcsnum 含义：与写线程中的 socket_Taida_Pcs[num] 对应（0、1、2、3 ...）

                    int pcsnum = -1; // 0:PCS1, 1:PCS2, 2:PCS3, 3:PCS4, 4:PCS5, 5:PCS6, 6:PCS7, 7:PCS8
                    if(sys_cfg->pcs_brand[0] == PCS_PE)
                    {
                        if (Temp.D16==27000) pcsnum = 0;
                        else if (Temp.D16==27300) pcsnum = 1;
                    }
                    else if(sys_cfg->pcs_brand[0] == PCS_TRINA)
                    {
                        if(Temp.D16>=12000&&Temp.D16<=12605)
                        {
                               pcsnum = 0;
                              powertestflag=true;
                               actpowervalue=RegVal.D16;
                               actpowervaddr=Temp.D16;
                        }

                        else if(Temp.D16>=12700&&Temp.D16<=13305)
                        {
                            pcsnum = 1;

                        }
                        else if(Temp.D16>=13400&&Temp.D16<=14005)
                        {
                            pcsnum = 2;

                        }
                        else if(Temp.D16>=14100&&Temp.D16<=14705)
                        {
                            pcsnum = 3;

                        }
                        else if((Temp.D16==1010)||(Temp.D16==1011)) // 下发MV 总功率触发标志
                        {
                            MV_Power_Flag=true;
                        }

                    }
                    else if(sys_cfg->pcs_brand[0] == PCS_Taida)
                    {
                        /*PCS组1参数设置：
                        27103-27106, 系统开/关命令~主电池1 充电功率系数
                        27000-27049, 组重置~从电池1 放电功率系数
                        27108-27109, 无功功率参考~表观功率参考
                        27111-27118, 孤岛电压参考~ 放电功率限制
                        */
                        if((Temp.D16>=27103&&Temp.D16<=27106)||(Temp.D16>=27000&&Temp.D16<=27051)||(Temp.D16==27053)||(Temp.D16>=27108&&Temp.D16<=27109)||(Temp.D16>=27111&&Temp.D16<=27118)) // 主机1
                        {
                            pcsnum = 0;
                        }
                        /*PCS组2参数设置：
                        27403-27406, 系统开/关命令~主电池1 充电功率系数
                        27300-27349, 组重置~从电池1 放电功率系数
                        27408-27409, 无功功率参考~表观功率参考
                        27411-27418, 孤岛电压参考~ 放电功率限制
                        */
                        else if((Temp.D16>=27403&&Temp.D16<=27406)||(Temp.D16>=27300&&Temp.D16<=27351)||(Temp.D16==27353)||(Temp.D16>=27408&&Temp.D16<=27409||(Temp.D16>=27411&&Temp.D16<=27418))) // 主机2
                        {
                            pcsnum = 2;
                        }
                        //PCS组1参数设置：有功功率，特殊处理，当运行模式是PF模式的时需要将值传递给PCSactivepower，然后进行转换（台达PCS原因）
                        else if(Temp.D16==27052)
                        {
                            if(((( GET_INPUT(17000 + 300 * 0 + 64)) >> 1) & 1u) == 1)
                            {
                                PCSactivepower[0]=RegVal.D16;

                            }
                            else
                            {
                                pcsnum = 0;
                            }
                        }
                        //PCS组2参数设置：有功功率，特殊处理，当运行模式是PF模式的时需要将值传递给PCSactivepower，然后进行转换（台达PCS原因）
                        else if(Temp.D16==27352)
                        {

                            if(((( GET_INPUT(17000 + 300 * 1 + 64)) >> 1) & 1u) == 1)
                            {
                                PCSactivepower[1]=RegVal.D16;
                            }
                            else
                            {
                                pcsnum = 2;
                            }
                        }
                        //PCS组1参数设置：功率因数，特殊处理，当运行模式是PF模式的时需要将值传递给PCSPF，然后进行转换（台达PCS原因）
                        else if(Temp.D16==27110)
                        {
                            if(((( GET_INPUT(17000 + 300 * 0 + 64)) >> 1) & 1u) == 1)
                            {
                                PCSPF[0]=RegVal.D16;
                            }
                            else
                            {
                                pcsnum = 0;
                            }
                        }
                        //PCS组2参数设置：功率因数，特殊处理，当运行模式是PF模式的时需要将值传递给PCSPF，然后进行转换（台达PCS原因）
                        else if(Temp.D16==27410)
                        {
                            if(((( GET_INPUT(17000 + 300 * 1 + 64)) >> 1) & 1u) == 1)
                            {
                                PCSPF[1]=RegVal.D16;
                            }
                            else
                            {
                                pcsnum = 2;
                            }

                        }

                        else if((Temp.D16==1010)||(Temp.D16==1011)) // 下发MV 总功率触发标志
                        {
                            MV_Power_Flag=true;
                        }
                        else if ((Temp.D16 >= 1506)&&(Temp.D16 <= 1521))  // PCS/BMS启用设置
                        {
                            PCS_BMS_Enable_Flag = true;

                        }
                        else if ((Temp.D16 >= 1538)&&(Temp.D16 <= 1547))  // PCS/BMS启用设置
                        {
                            BMS_PCS_Enable_Flag = true;

                        }
                        //PCS参数设置   电网放电功率标定 k~从机1 控制启用
                        if((Temp.D16>=12000&&Temp.D16<=12032)) pcsnum = 0; // 主机1
                        else if(Temp.D16>=12300&&Temp.D16<=12332) pcsnum = 1; // 从机1-1
                        else if((Temp.D16>=12600&&Temp.D16<=12632)) pcsnum = 2; // 主机2
                        else if(Temp.D16>=12900&&Temp.D16<=12932) pcsnum = 3; // 从机2-1

                    }

                    if (pcsnum >= 0)
                    {
                        PcsWriteReq pcsreq;
                        pcsreq.addr     = Temp.D16;
                        pcsreq.value    = RegVal.D16;
                        pcsreq.len      = 1;
                        pcsreq.is_multi = false;
                        // led_off();
                        // 去重，保留最新值
                        Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &pcsreq);//放入缓冲区中
                        LOG_INFO("pcsnum:%d,addr:%d,value:%d", pcsnum, Temp.D16, RegVal.D16);
                        //led_on();
                    }

                    break;
                }

                default:
                break;
            }

            // EMS通讯正常
            Set_EMS_Comm(0, IsNoFault, TRUE);

            // 返信
            write(new_fd_thread, sendbuffer, sendbuffer[5] + 6);
           
        }
        if ((connect_num >= (MAX_CLIENT_NUM - 1)) && (connect_ID_min == (*pConnectID)))
        {
            if(connect_ID_max<=(connect_ID_min+(MAX_CLIENT_NUM/5)))// 最新序号和最老序号差得很近时，留住socket
            {
                connect_num--;
                LOG_INFO("EMS Client[%d] Leave the TCP server socket! (fd:%d,connect_ID_min:%d,connect_ID_max:%d) ", connect_num, new_fd_thread, connect_ID_min,connect_ID_max);
            }
            else
            {
                if (new_fd_thread > 0)
                {
                    close(new_fd_thread);
                }
                connect_num--;
                LOG_INFO("EMS Client[%d] Close the TCP server socket! (fd:%d,connect_ID_min:%d) ", connect_num, new_fd_thread, connect_ID_min);
            }
            break;
        }
    }
    connect_ID_min = UINT_MAX;
    connect_ID_max = 0;
    free(pTimeCnt);
    pTimeCnt = NULL;
    free(pConnectID);
    pConnectID = NULL;
    return NULL;
}

/**
* @brief 接受客户端连接请求并创建处理线程
*
* 该函数用于接受来自客户端的连接请求，在满足特定条件时设置EMS通信状态。
* 成功接受连接后，为每个客户端创建独立的处理线程，并分配客户端上下文。
*
* @param pserver_fd 指向服务器套接字文件描述符的指针
*
* @note 函数会检查REMOTE_LOCAL_CONTROL_ENABLE和REMOTE_LOCAL_CONTROL_MODE标志
* @note 处理线程设置为分离状态(PTHREAD_CREATE_DETACHED)
* @note 函数内部处理了内存分配失败和线程创建失败的情况
* @note 包含对特殊字符\t, \r, \n的处理
*
* @return 无返回值
*/

static void Accept_Client_Connect(int *pserver_fd)
{
    struct sockaddr_in client_addr = {0};
    socklen_t sin_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 0, sin_size);
    //判断是否满足本地控制的条件
    if((GET_HOLD(REMOTE_LOCAL_CONTROL_ENABLE)==1)&&(GET_HOLD(REMOTE_LOCAL_CONTROL_MODE)==0))
    {
        Set_EMS_Comm(0, IsNoFault, TRUE);
    }

    int client_fd = accept((*pserver_fd), (struct sockaddr *)(&client_addr), &sin_size);
    if (client_fd == -1)
    {
        return;
    }
    LOG_INFO("EMS New Client (fd:%d) connect\n", client_fd);

    pthread_t client_thread_id = 0;

    pthread_attr_t client_thread_attr;

    pthread_attr_init(&client_thread_attr);

    pthread_attr_setdetachstate(&client_thread_attr, PTHREAD_CREATE_DETACHED);

    /* 为每个连接分配上下文，传 fd */

    ClientCtx *ctx = (ClientCtx*)malloc(sizeof(ClientCtx));
    if (!ctx) {
        LOG_INFO("EMS malloc ctx failed, close fd:%d\n", client_fd);
        close(client_fd);
        return;
    }
    ctx->fd = client_fd;

    if (pthread_create(&client_thread_id, &client_thread_attr, Server_Handle_Data, ctx) != 0)
    {
        LOG_INFO("EMS Client (fd:%d) pthread create Error:%s\n", client_fd, strerror(errno));

        pthread_attr_destroy(&client_thread_attr);
        close(client_fd);
        free(ctx);
        return;
    }

    pthread_attr_destroy(&client_thread_attr);
    LOG_INFO("EMS Client (fd:%d) Connect from %s:%d\n",
    client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    usleep(10 * 1000);
}
/**
* @brief EMS服务端任务，负责创建socket服务并监听客户端连接
*
* 该函数执行以下操作：
* 1. 配置socket参数（端口等）
* 2. 创建EMS服务端socket（失败时重试）
* 3. 监听并接受客户端连接
*
* @note 函数包含两个无限循环：
*       - 第一个循环负责创建socket（失败时每10秒重试）
*       - 第二个循环负责接受客户端连接（每100ms检查一次）
*
* @attention 使用了静态变量保持socket状态
* @warning 包含\r\n\t等特殊字符处理
*/

void Task_EMS_Server(void)
{
    // 配置socket资料

    static int sockfd_service = 0;

    static INT32U server_create_count = 0;
    sysPara *sys_cfg = SysConf_GetInfo();

    static in_port_t sin_port;
    sin_port = htons(sys_cfg->localEmsPort);

    // 创建EMS服务端socket
    while (1)
    {
        if ((sockfd_service = Create_Server_Socket(sin_port, MAX_CLIENT_NUM)) == -1)
        {
            // 无通讯故障时打印log，防止log刷屏
            if (Get_EMS_Comm(0) == IsNoFault)
            {
                LOG_INFO("EMS Create server socket failure!, count:%d", server_create_count++);
            }
            sleep(10);
            continue;
        }
        // EMS通讯正常
        LOG_INFO("EMS server[%d] : Create socket success! wait client connect ......", ntohs(sin_port));
        Set_EMS_Comm(0, IsNoFault, TRUE);
        break;
    }

    // 等待客户端的链接
    while (1)
    {
        Accept_Client_Connect(&sockfd_service);
        usleep( 100* 1000);
    }
}
/*有功功率因数计算无功功率*/
/**
 * @brief 根据有功功率和功率因数计算无功功率
 *
 * @param activePower
 *        有功功率原始值，类型int16_t，倍率1kW
 *        例如：
 *        1000  表示 +1000kW
 *        -1000 表示 -1000kW
 *
 * @param powerFactor
 *        功率因数原始值，类型int16_t，范围[-1000, 1000]
 *        倍率0.001
 *        例如：
 *        800  表示 +0.800
 *        -800 表示 -0.800
 *        1000 表示 +1.000
 *
 * @return 无功功率，类型int16_t，倍率1kvar
 *
 * 规则：
 * 1. powerFactor > 0，返回正无功；
 * 2. powerFactor < 0，返回负无功；
 * 3. powerFactor == 0，直接返回0；
 * 4. 有功功率正负不影响无功方向；
 * 5. 自动保证P² + Q²不超过2500²。
 */
int16_t calcReactivePower(int16_t activePower,
                          int16_t powerFactor)
{
    int32_t activePowerValue;
    int32_t absActivePower;
    int32_t absPowerFactor;

    float pf;
    float reactivePower;
    float maxReactivePower;
    float temp;

    int32_t reactivePowerResult;

    /* 功率因数为0，不进行计算 */
    if (powerFactor == 0)
    {
        return 0;
    }

    /* 功率因数范围检查 */
    if ((powerFactor < -1000) ||
        (powerFactor > 1000))
    {
        return 0;
    }

    activePowerValue = (int32_t)activePower;

    if (activePowerValue < 0)
    {
        absActivePower = -activePowerValue;
    }
    else
    {
        absActivePower = activePowerValue;
    }

    if (powerFactor < 0)
    {
        absPowerFactor = -(int32_t)powerFactor;
    }
    else
    {
        absPowerFactor = (int32_t)powerFactor;
    }

    /* 有功为0 */
    if (absActivePower == 0)
    {
        return 0;
    }

    /* 有功超过10000kW */
    if (absActivePower > PCS_RATED_POWER_KVA)
    {
        return 0;
    }

    /* PF原始值转换为实际功率因数 */
    pf = (float)absPowerFactor / 1000.0f;

    /*
     * 目标无功：
     * Q = P × sqrt(1 / PF² - 1)
     */
    temp = 1.0f / (pf * pf) - 1.0f;

    if (temp < 0.0f)
    {
        temp = 0.0f;
    }

    reactivePower =
        (float)absActivePower * sqrtf(temp);

    /*
     * 最大允许无功：
     * Qmax = sqrt(10000² - P²)
     */
    temp =
        (float)PCS_RATED_POWER_KVA *
        (float)PCS_RATED_POWER_KVA -
        (float)absActivePower *
        (float)absActivePower;

    if (temp < 0.0f)
    {
        temp = 0.0f;
    }

    maxReactivePower = sqrtf(temp);

    /* 容量限幅 */
    if (reactivePower > maxReactivePower)
    {
        reactivePower = maxReactivePower;
    }

    /* PF负数对应负无功 */
    if (powerFactor < 0)
    {
        reactivePower = -reactivePower;
    }

    reactivePowerResult =
        (int32_t)lroundf(reactivePower);

    return (int16_t)reactivePowerResult;
}
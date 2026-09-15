/* ------------------------------------------------------------
* Copyright (C) 2026
* File Name : web_server.c
* Project :
* Description : Modbus TCP通信协议，对接WEB的链接
* File Created :
* Author : tyq
* ------------------------------------------------------------
* Last Modified : 2025/10/17
* Modified By : （队列化写入改造：0x06 入队去重，写线程消费）
* ------------------------------------------------------------
* HISTORY : 
*/

#include "main.h"
#include "timed_check.h"
#include "web_server.h"
#include "pcs_write_queue.h"   // ★ 新增：写入请求队列

extern char *const pIsCheckTime;

static INT8U  PCSnum;

static INT8U  BMSnum;

extern BOOLEAN BMS_Write_Flag;

extern BOOLEAN Cem9000_Write_Flag;

extern INT32U  Cem9000_Control_Value;

extern INT32U  Cem9000_Addr;

extern INT32U  Write_Lenth;

extern INT32U  Bank_Addr_Base;

extern INT32U  PCS_Addr_Base;

extern INT32U  Bank_addr;

extern BOOLEAN MV_Power_Flag;

BOOLEAN PCS_BMS_Enable_Flag;

BOOLEAN BMS_PCS_Enable_Flag;

extern volatile BOOLEAN Set_Time_Flag;

extern INT32S PCSactivepower[10];

extern INT32S PCSPF[10];
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
    const int new_fd_thread = ctx ? ctx->fd : -1;
    free(ctx),
    ctx = NULL;

    static int volatile connect_num = 0; // 统计链接的客户端数量

    static unsigned int volatile connect_time = 0;          // 统计链接的客户端总次数

    static unsigned int volatile connect_ID_min = UINT_MAX; // 链接的最小序号

    int numbytes = 0;//接收数据长度

    u16_conv Temp;//临时变量，用于拼接寄存器值
    char buffer[TCP_SERVER_RECV_LEN] = {0};//接收数据缓冲区
    char sendbuffer[TCP_SERVER_RECV_LEN] = {0};//发送数据缓冲区
    char logstr[100] = {0};//日志字符串
    char time_str[36] = {0};//时间字符串

    bool is_sub = false;

    bool is_print = false;

    INT16U i = 0;

    INT16U AddrStar = 0;

    INT16U year = 0, month = 0, day = 0, hour = 0, min = 0, second = 0;

    INT16U MStar = 0, MLen = 0;

    INT8U dev_add = 0;

    u16_conv RegVal;
    SYS_TIME_BUF time;

    int pcsnum=0;

    int sys_num=0;
    ////////////////设置接收超时////////////////
    struct timeval rec_timeout;
    sysPara* sys_cfg = SysConf_GetInfo();

    // EMS通讯正常,全部通信链接均判断正常

    rec_timeout.tv_sec  = 0;
    rec_timeout.tv_usec = (20 * 1000);
    if (setsockopt(new_fd_thread, SOL_SOCKET, SO_RCVTIMEO, (char *)&rec_timeout, sizeof(rec_timeout)) == -1)
    {
        LOG_INFO("web Set socket property failed, SO_RCVTIMEO: %s *Reason: web server (fd:%d) ", strerror(errno), new_fd_thread);
        if (new_fd_thread > 0)
        {
            close(new_fd_thread);
        }
        return;
    }

    connect_num++;
    connect_time++;
    LOG_INFO("web Client[%d] new connect success! (fd:%d) ", connect_num, new_fd_thread);
    unsigned short *ptimecnt   = malloc(sizeof(unsigned short));
    unsigned int  *pconnectID  = malloc(sizeof(unsigned int));
    if (!ptimecnt || !pconnectID)
    {
        if (new_fd_thread > 0)
        {
            close(new_fd_thread);
        }
        free(ptimecnt);
        free(pconnectID);
        return;
    }
    *ptimecnt = 0;
    *pconnectID = connect_time;

    while (1)
    {
        if ((*pconnectID) < connect_ID_min)
        {
            connect_ID_min = (*pconnectID);
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
                LOG_INFO("web close the socket\n");
            }
            // 其他错误
            if ((numbytes < 0) && (errno != EAGAIN))
            {
                LOG_INFO("web Other socket fault!\n");
            }
            (*ptimecnt) = 0;
            if (new_fd_thread > 0)
            {
                close(new_fd_thread);
            }
            connect_num--;
            LOG_INFO("web Client[%d] Close the TCP server socket! (fd:%d) ", connect_num, new_fd_thread);
            // 对方关闭连接，或者连接出现错误，则关闭退出线程。
            break;
        }
        // 超时
        else if ((numbytes < 0) && (errno == EAGAIN))
        {
            (*ptimecnt) += 1;
            if ((*ptimecnt) > (50 * ((EMS_TIMEOUT_CNT / 100) + 5))) // 600s 清理僵尸进程
            {
                if (new_fd_thread > 0)
                {
                    close(new_fd_thread);
                }
                connect_num--;
                LOG_INFO("web Client[%d] Close the TCP server socket! (fd:%d) ", connect_num, new_fd_thread);
                break;
            }
        }
        // 对接收到数据进行处理
        else
        {
            // EMS通讯正常

            (*ptimecnt) = 0;

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
                for (i = 0; (i < buffer[11]) && (MStar < TCP_SERVER_RECV_LEN) && (Temp.D16 < MAX_SYSTEM_TOTAL_DATA_NUM); i++)
                {
                    if(dev_add==1)
                    {
                        RegVal.D16= GET_INPUT(Temp.D16);
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
                        //     sendbuffer[MStar++] = RegVal.D8[0];
                        //     sendbuffer[MStar++] = RegVal.D8[1];
                        // }

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
                    is_sub = false;
                    is_print = false;
                    for (i = 0; (i < MLen) && ((14 + i * 2) < TCP_SERVER_RECV_LEN); i++)
                    {
                        RegVal.D8[1] = buffer[13 + i * 2]; // （高字节在前、低字节在后）
                        RegVal.D8[0] = buffer[14 + i * 2];

                        SET_HOLD(Temp.D16, RegVal.D16);
                        pcsnum =PCSnum_calculate(Temp.D16);
                        sys_num = (pcsnum>= 2) ? 1 : 0;
                        //  LOG_INFO("pcsnum=%d,sys_num=%d",pcsnum,sys_num);
                        if ((pcsnum >= 0)&&(((Temp.D16<27046+sys_num*300))||(Temp.D16>27052+sys_num*300)))
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
                        LOG_INFO("Web set 0x10, address:%d, register length:%d,buffer => %s", Temp.D16, MLen, logstr);
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
                        LOG_INFO("Web synchronize time : %s", time_str); // date -s '2022-7-27 10:59:43'
                        (*pIsCheckTime) = 1;
                        Set_Time_Flag=true;

                    }
                    else if(((AddrStar>=27046)&&(AddrStar<=27052)))
                    {
                        Group1_power_multi_Flag=true;
                        LOG_INFO("111");
                    }
                    else if((AddrStar>=27346)&&(AddrStar<=27352))
                    {
                        Group2_power_multi_Flag=true;
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

                        // 写本地镜像
                        SET_HOLD(Temp.D16, RegVal.D16);

                        // 回包
                        sendbuffer[5]  = buffer[5];
                        sendbuffer[8]  = buffer[8];
                        sendbuffer[9]  = buffer[9];
                        sendbuffer[10] = buffer[10];
                        sendbuffer[11] = buffer[11];

                        if (false == is_sub)
                        {
                            LOG_INFO("WEB set 0x06, address:%d -> data:%d", Temp.D16, RegVal.D16);
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
                            LOG_INFO("WEB synchronize time : %s", time_str); // date -s '2022-7-27 10:59:43'
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
                            Cem9000_Addr =2003;
                            Cem9000_Control_Value=0xFF00;
                        }
                        else if(Temp.D16==33202&&RegVal.D16==0XAA)
                        {
                            Cem9000_Write_Flag =true;
                            Cem9000_Addr =2004;
                            Cem9000_Control_Value=0xFF00;
                        }
                        else if(Temp.D16==33203&&RegVal.D16==0XEE)
                        {
                            Cem9000_Write_Flag =true;
                            Cem9000_Addr =2005;
                            Cem9000_Control_Value=0xFF00;
                        }
                        else if(Temp.D16==33203&&RegVal.D16==0XAA)
                        {
                            Cem9000_Write_Flag =true;
                            Cem9000_Addr =2006;
                            Cem9000_Control_Value=0xFF00;
                        }

                        // 主机 PCS 开/停机
                        for(INT8U sys_num=0; (sys_num < sys_cfg->sysNum); sys_num++)
                        {
                            if(Temp.D16 == (27000 + 300 * sys_num + 103))//主机PCS开机或者关机
                            {
                                if(RegVal.D16==1) Set_In_Sys(sys_num,SYS_EVENT_CMD_START);
                                else              Set_In_Sys(sys_num,SYS_EVENT_CMD_STOP);
                                break;
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
                                Set_In_Sys(0,SYS_EVENT_CMD_RESET);
                                Set_In_Sys(1,SYS_EVENT_CMD_RESET);
                            }
                            break;
                        }

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
                            if((Temp.D16>=27103&&Temp.D16<=27106)||(Temp.D16>=27000&&Temp.D16<=27049)||(Temp.D16==27053)||(Temp.D16>=27108&&Temp.D16<=27109)||(Temp.D16>=27111&&Temp.D16<=27118)) // 主机1
                            {
                                pcsnum = 0;
                            }
                            /*PCS组2参数设置：
                            27403-27406, 系统开/关命令~主电池1 充电功率系数
                            27300-27349, 组重置~从电池1 放电功率系数
                            27408-27409, 无功功率参考~表观功率参考
                            27411-27418, 孤岛电压参考~ 放电功率限制
                            */
                            else if((Temp.D16>=27403&&Temp.D16<=27406)||(Temp.D16>=27300&&Temp.D16<=27349)||(Temp.D16==27353)||(Temp.D16>=27408&&Temp.D16<=27409||(Temp.D16>=27411&&Temp.D16<=27418))) // 主机2
                            {
                                pcsnum = 2;
                            }
                            //PCS组1参数设置：有功功率，特殊处理，当运行模式是PF模式的时需要将值传递给PCSactivepower，然后进行转换（台达PCS原因）
                            else if(Temp.D16==27052)
                            {

                                if(((( GET_INPUT(17000 + 300 * 0 + 64)) >> 1) & 1u) == 1)
                                {
                                    PCSactivepower[0]=RegVal.D16;
                                    LOG_INFO("pcs1 pf mode,pcs1 active power is %d",PCSactivepower[0]);
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

                            else if((Temp.D16==1010)||(Temp.D16==1011))// 下发MV 总功率触发标志
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

                            // 去重，保留最新值
                            Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &pcsreq);//放入缓冲区中
                        }
                        /************************************************BMS参数设置，罗马尼亚项目LC不控制BMS************************************************************************ */
                        // int bmsnum = -1;
                        // int bmsaddr;
                        // if(sys_cfg->bms_brand[0] == BMS_XIENENG_G2pro)
                        // {

                            //     if ((Temp.D16>=35001)&&(Temp.D16<=38000))
                            //     {
                                //         bmsnum =Find_Bank_N(Temp.D16,&bmsaddr);
                                //         LOG_INFO("bmsnum is %d",bmsnum);
                                //     }
                                //     else
                                //     {
                                    //         LOG_INFO("BMS set value out limit address!");
                                    //     }

                                    // }
                                    // if (bmsnum >= 0)
                                    // {
                                        //     BmsWriteReq bmsreq;
                                        //     bmsreq.addr     = Temp.D16;
                                        //     bmsreq.value    = RegVal.D16;
                                        //     bmsreq.len      = 1;
                                        //     bmsreq.is_multi = false;

                                        //     // 去重，保留最新值
                                        //     bms_write_enqueue_dedup_by_addr(bmsnum-1, &bmsreq);
                                        // }

                                        /************************************************************************************************************************************ */

                                        break;
                                    }

                                    default:
                                    break;
                                }

                                // EMS通讯正常
                                // Set_EMS_Comm(0, IsNoFault, TRUE);

                                // 返信
                                write(new_fd_thread, sendbuffer, sendbuffer[5] + 6);

                                signal(SIGPIPE, SIG_IGN);
                            }
                            if ((connect_num >= (MAX_CLIENT_NUM - 1)) && (connect_ID_min == (*pconnectID)))
                            {
                                if (new_fd_thread > 0)
                                {
                                    close(new_fd_thread);
                                }
                                connect_num--;
                                LOG_INFO("web Client[%d] Close the TCP server socket! (fd:%d,connect_ID_min:%d) ", connect_num, new_fd_thread, connect_ID_min);
                                break;
                            }
                        }
                        connect_ID_min = UINT_MAX;
                        free(ptimecnt);
                        free(pconnectID);
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

                        int client_fd = accept((*pserver_fd), (struct sockaddr *)(&client_addr), &sin_size);
                        if (client_fd == -1)
                        {
                            return;
                        }

                        LOG_INFO("web New Client (fd:%d) connect\n", client_fd);

                        pthread_t client_thread_id = 0;

                        pthread_attr_t client_thread_attr;

                        pthread_attr_init(&client_thread_attr);

                        pthread_attr_setdetachstate(&client_thread_attr, PTHREAD_CREATE_DETACHED);

                        /* 为每个连接分配上下文，传 fd */

                        ClientCtx *ctx = (ClientCtx*)malloc(sizeof(ClientCtx));
                        if (!ctx) {
                            LOG_INFO("web malloc ctx failed, close fd:%d\n", client_fd);
                            close(client_fd);
                            return;
                        }
                        ctx->fd = client_fd;

                        if (pthread_create(&client_thread_id, &client_thread_attr, Server_Handle_Data, ctx) != 0)
                        {
                            LOG_INFO("web Client (fd:%d) pthread create Error:%s\n", client_fd, strerror(errno));

                            close(client_fd);
                            free(ctx);
                            return;
                        }

                        LOG_INFO("web Client (fd:%d) Connect from %s:%d\n",
                        client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                        usleep(10 * 1000);
                    }
                    /**
                    * @brief web服务端任务，负责创建socket服务并监听客户端连接
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

                    void Task_WEB_Server(void)
                    {
                        // 配置socket资料

                        static int sockfd_service = 0;

                        static INT32U server_create_count = 0;
                        sysPara *sys_cfg = SysConf_GetInfo();

                        static in_port_t sin_port;
                        sin_port = htons(sys_cfg->webEmsPort);
                        pcs_write_queue_init();
                        // 创建web服务端socket
                        while (1)
                        {
                            if ((sockfd_service = Create_Server_Socket(sin_port, MAX_CLIENT_NUM)) == -1)
                            {
                                // 无通讯故障时打印log，防止log刷屏
                                if (Get_EMS_Comm(0) == IsNoFault)
                                {
                                    LOG_INFO("web Create server socket failure!, count:%d", server_create_count++);
                                }
                                sleep(10);
                                continue;
                            }
                            // EMS通讯正常
                            LOG_INFO("web server[%d] : Create socket success! wait client connect ......", ntohs(sin_port));

                            break;
                        }

                        // 等待客户端的链接
                        while (1)
                        {
                            Accept_Client_Connect(&sockfd_service);
                            usleep(100 * 1000);
                        }
                    }
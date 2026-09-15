/* ------------------------------------------------------------
 * File Name : whitelist.c
 * Description : Modbus TCP 协议服务（白名单 + 非白名单握手）
 * Last Modified : 2025/10/28
 * ChangeLog :
 *  - 移除全局 new_client_fd，改为“fd 作为线程参数传递”（支持并发）
 *  - 非白名单分支新增 XXTEA 握手（
 *  - 原 server_handle_data 改为 server_handle_data_body(fd)
 * ------------------------------------------------------------ */

#include "main.h"
#include "timed_check.h"
#include "whitelist.h"
#include "pcs_write_queue.h"
#include "acl_db.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "xxtea.h"         // xxtea_encrypt_16 / xxtea_decrypt_16
#include "crypto_util.h"   // get_random_bytes / make_xxtea_key_from_ascii

extern char *const pIsCheckTime;
static int  PCSnum;
static INT8U  BMSnum;
extern BOOLEAN BMS_Write_Flag;
extern BOOLEAN Cem9000_Write_Flag;
extern INT32U  Cem9000_Control_Value;
extern INT32U  Cem9000_Addr;
static INT32U  Write_Lenth;
static INT32U  Bank_Addr_Base;
static INT32U  PCS_Addr_Base;
static INT32U  Bank_addr;

extern BOOLEAN MV_Power_Flag;

INT32U xxkey[4];

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
static void server_handle_data_body(int fd)
{

    static int volatile connect_num = 0;            // 统计链接的客户端数量（注意：并发场景为近似值）
    static unsigned int volatile connect_time = 0;  // 统计链接的客户端总次数
    static unsigned int volatile connect_ID_min = UINT_MAX;

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

    struct timeval rec_TimeOut;
    sysPara* sys_cfg = SysConf_GetInfo();

    /* 通讯正常 */
   
    rec_TimeOut.tv_sec  = 0;
    rec_TimeOut.tv_usec = (20 * 1000);
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rec_TimeOut, sizeof(rec_TimeOut)) == -1)//设置接收超时
    {
        LOG_INFO("whitedevice SO_RCVTIMEO failed: %s (fd:%d)", strerror(errno), fd);
        if (fd > 0) close(fd);
        return;
    }

    signal(SIGPIPE, SIG_IGN);

    /* 连接计数 */
    connect_num++;
    connect_time++;
    LOG_INFO("whitedevice Client[%d] new connect success! (fd:%d)", connect_num, fd);

    unsigned short *pTimeCnt  = (unsigned short*)malloc(sizeof(unsigned short));
    unsigned int  *pConnectID = (unsigned int*)malloc(sizeof(unsigned int));
    if (!pTimeCnt || !pConnectID)
    {
        if (fd > 0) close(fd);
        free(pTimeCnt);
        free(pConnectID);
        return;
    }
    *pTimeCnt   = 0;
    *pConnectID = connect_time;

    while (1)
    {
        if ((*pConnectID) < connect_ID_min) connect_ID_min = (*pConnectID);


        memset(buffer, 0, sizeof(buffer));
        numbytes = recv(fd, buffer, TCP_SERVER_RECV_LEN, 0);

        /* 连接关闭/错误 */
        if (((numbytes < 0) && (errno != EAGAIN)) || (numbytes == 0))
        {
            if (numbytes == 0) LOG_INFO("whitedevice close the socket");
            if ((numbytes < 0) && (errno != EAGAIN)) LOG_INFO("whitedevice socket fault!");
            (*pTimeCnt) = 0;
            if (fd > 0) close(fd);
            connect_num--;
            LOG_INFO("whitedevice Client[%d] Close the TCP server socket! (fd:%d)", connect_num, fd);
            break;
        }
        /* 超时 */
        else if ((numbytes < 0) && (errno == EAGAIN))
        {
            (*pTimeCnt) += 1;
            if ((*pTimeCnt) > (50 * ((EMS_TIMEOUT_CNT / 100) + 5))) // 600s 清理僵尸
            {
                if (fd > 0) close(fd);
                connect_num--;
                LOG_INFO("whitedevice Client[%d] Close the TCP server socket! (fd:%d)", connect_num, fd);
                break;
            }
        }
        /* 正常收包 */
        else
        {
       
            (*pTimeCnt) = 0;

            /* 回包头部组装 */
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

            /* 起始地址 */
            Temp.D8[1] = buffer[8];
            Temp.D8[0] = buffer[9];

            switch (buffer[7])
            {
            case 0x03:
                for (i = 0; (i < buffer[11]) && (MStar < TCP_SERVER_RECV_LEN) && (Temp.D16 < MAX_SYSTEM_TOTAL_DATA_NUM); i++)
                {
                    if (dev_add == 1) RegVal.D16 = GET_HOLD(Temp.D16);
                    else break;

                    Sendbuffer[MStar++] = RegVal.D8[1];
                    Sendbuffer[MStar++] = RegVal.D8[0];
                    Temp.D16++;
                }
                Sendbuffer[5] = Sendbuffer[8] + 3;
                break;

            case 0x04:
                for (i = 0; (i < buffer[11]) && (MStar < TCP_SERVER_RECV_LEN) && (Temp.D16 < MAX_SYSTEM_TOTAL_DATA_NUM); i++)
                {
                    if (dev_add == 1) {
                        RegVal.D16 = GET_INPUT(Temp.D16);
                    } else if ((dev_add >= 2) && (dev_add < (2 + BANK_SIZE))) {
                        int bankN = dev_add - 2;
                        RegVal.D16 = GET_C_INPUT(bankN, Temp.D16);
                    } else {
                        break;
                    }

                    // if ((dev_add == 1) && (Temp.D16 >= 30043) && (Temp.D16 <= 30046)) {
                    //     Sendbuffer[MStar++] = RegVal.D8[0];
                    //     Sendbuffer[MStar++] = RegVal.D8[1];
                    // } 
                    // else {
                        Sendbuffer[MStar++] = RegVal.D8[1];
                        Sendbuffer[MStar++] = RegVal.D8[0];
                    // }
                    Temp.D16++;
                }
                Sendbuffer[5] = Sendbuffer[8] + 3;
                break;

            case 0x10:
            {
                MLen = (buffer[10] << 8) + buffer[11];
                AddrStar = Temp.D16;
                is_sub = false;
                is_print = false;

                if (Temp.D16 >= 35001 && Temp.D16 <= 37974) {
                    BMS_Write_Flag = true;
                    BMSnum = Find_Bank_N(Temp.D16, &Bank_Addr_Base) - 1;
                    Write_Lenth = MLen;
                    Bank_addr = Temp.D16;
                    LOG_INFO("BMSnum is %d", BMSnum);
                }
                else if ((Temp.D16 >= 0 && Temp.D16 <= 1510) || Temp.D16 == 27000) {
                    PCS_Write_Flag[0] = true;
                    PCSWrite_Lenth[0] = MLen;
                    PCSaddr[0] = Temp.D16;
                }
                else if (Temp.D16 == 27300) {
                    PCS_Write_Flag[1] = true;
                    PCSWrite_Lenth[1] = MLen;
                    PCSaddr[1] = Temp.D16;
                }
                else {
                    LOG_INFO("address invalid!");
                }

                for (i = 0; (i < MLen) && ((14 + i * 2) < TCP_SERVER_RECV_LEN); i++) {
                    RegVal.D8[1] = buffer[13 + i * 2];
                    RegVal.D8[0] = buffer[14 + i * 2];
                    SET_HOLD(Temp.D16, RegVal.D16);
                    Temp.D16++;
                }

                if ((false == is_sub) || (is_print == true)) {
                    memset(LogStr, 0, sizeof(LogStr));
                    {
                        size_t dump_len = (size_t)(MLen * 2 + 13);
                        if (dump_len > (size_t)TCP_SERVER_RECV_LEN) {
                            dump_len = (size_t)TCP_SERVER_RECV_LEN;
                        }
                        Hex_To_String(LogStr, sizeof(LogStr), (const unsigned char *)buffer, dump_len);
                    }
                    LOG_INFO("whitedevice set 0x10, address:%d, length:%d, buf=>%s", Temp.D16, MLen, LogStr);
                }

                if (AddrStar == SYS_SYNC_TIEM_ADDR) {
                    year = (buffer[13] << 8) + buffer[14];
                    month = (buffer[15] << 8) + buffer[16];
                    day = (buffer[17] << 8) + buffer[18];
                    hour = (buffer[19] << 8) + buffer[20];
                    min = (buffer[21] << 8) + buffer[22];
                    second = (buffer[23] << 8) + buffer[24];
                    memset(time_str, 0, sizeof(time_str));
                    sprintf(time_str, "date -s '%d-%d-%d %d:%d:%d'", year, month, day, hour, min, second);
                    Do_System(time_str, 2);
                    Do_System("hwclock --systohc", 1);
                    LOG_INFO("whitedevice synchronize time : %s", time_str);
                    (*pIsCheckTime) = 1;
                }

                Sendbuffer[5]  = 6;
                Sendbuffer[8]  = buffer[8];
                Sendbuffer[9]  = buffer[9];
                Sendbuffer[10] = buffer[10];
                Sendbuffer[11] = buffer[11];
                break;
            }

            case 0x06:
            {
                if (dev_add != 1) break;

                is_sub = false;
                RegVal.D8[1] = buffer[10];
                RegVal.D8[0] = buffer[11];
                SET_HOLD(Temp.D16, RegVal.D16);

                Sendbuffer[5]  = buffer[5];
                Sendbuffer[8]  = buffer[8];
                Sendbuffer[9]  = buffer[9];
                Sendbuffer[10] = buffer[10];
                Sendbuffer[11] = buffer[11];

                if (false == is_sub) {
                    LOG_INFO("whitedevice set 0x06, address:%d -> data:%d", Temp.D16, RegVal.D16);
                }

                if (Temp.D16==33200 && RegVal.D16==0xAA) { Cem9000_Write_Flag = true; Cem9000_Addr=2000; Cem9000_Control_Value=0xFF00; }
                else if (Temp.D16==33200 && RegVal.D16==0xEE) { Cem9000_Write_Flag = true; Cem9000_Addr=2001; Cem9000_Control_Value=0xFF00; }
                else if (Temp.D16==33201) { Cem9000_Write_Flag = true; Cem9000_Addr=2002; Cem9000_Control_Value=0xFF00; }

                for (INT8U sys_num=0; (sys_num < sys_cfg->sysNum); sys_num++)
                {
                    if (Temp.D16 == (27000 + 300 * sys_num + 103)) {
                        if (RegVal.D16==1) Set_In_Sys(sys_num, SYS_EVENT_CMD_START);
                        else               Set_In_Sys(sys_num, SYS_EVENT_CMD_STOP);
                        break;
                    }
                }

                int pcsnum = -1;
                SUB_State_ENUM state = Get_State_Sys(0);

                if (sys_cfg->pcs_brand[0] == PCS_PE) {
                    if (state == SYSRun) {
                        if      (Temp.D16 == 27000) pcsnum = 0;
                        else if (Temp.D16 == 27300) pcsnum = 1;
                    }
                }
                else if (sys_cfg->pcs_brand[0] == PCS_TRINA) {
                    if (state == SYSRun) {
                        if (Temp.D16 >= 27000 && Temp.D16 <= 28235) pcsnum = 0;
                    }
                }
                else if (sys_cfg->pcs_brand[0] == PCS_Taida) {
                    if ((Temp.D16>=27105&&Temp.D16<=27106)||(Temp.D16>=27000&&Temp.D16<=27046)||(Temp.D16>=27111&&Temp.D16<=27118)) pcsnum = 0;
                    else if ((Temp.D16>=27405&&Temp.D16<=27406)||(Temp.D16>=27300&&Temp.D16<=27346)||(Temp.D16>=27111&&Temp.D16<=27118)) pcsnum = 2;

                    else if (Temp.D16==27107) { PCSactivepower[0]=RegVal.D16; }
                    else if (Temp.D16==27407) { PCSactivepower[1]=RegVal.D16; }
                    else if (Temp.D16==27110) { PCSPF[0]=RegVal.D16; }
                    else if (Temp.D16==27410) { PCSPF[1]=RegVal.D16; }
                    else if (Temp.D16==1010) { /* MV 总功率触发 */ }

                    if      ((Temp.D16>=12000&&Temp.D16<=12025)) pcsnum = 0;
                    else if (Temp.D16>=12300&&Temp.D16<=12325)   pcsnum = 1;
                    else if ((Temp.D16>=12600&&Temp.D16<=12625)) pcsnum = 2;
                    else if (Temp.D16>=12900&&Temp.D16<=12925)   pcsnum = 3;
                }

                if (pcsnum >= 0) {
                    PcsWriteReq req;
                    req.addr     = Temp.D16;
                    req.value    = RegVal.D16;
                    req.len      = 1;
                    req.is_multi = false;
                    Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &req);
                }
                break;
            }

            default:
                break;
            }

            /* 返信 */
            write(fd, Sendbuffer, Sendbuffer[5] + 6);
        }

        if ((connect_num >= (MAX_CLIENT_NUM - 1)) && (connect_ID_min == (*pConnectID)))
        {
            if (fd > 0) close(fd);
            connect_num--;
            LOG_INFO("whitedevice Client[%d] Close the TCP server socket! (fd:%d, connect_ID_min:%d)", connect_num, fd, connect_ID_min);
            break;
        }
    }

    connect_ID_min = UINT_MAX;
    free(pTimeCnt);
    free(pConnectID);
}

/*加密报文处理 */


static void server_handle_XXTEAdata_body(int fd)
{
    static int volatile connect_num = 0;           // 统计链接的客户端数量（注意：并发场景为近似值）
    static unsigned int volatile connect_time = 0; // 统计链接的客户端总次数
    static unsigned int volatile connect_ID_min = UINT_MAX;

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

    INT8U  tx_encrypt_buf[600];   // 加密后的发送缓冲区
    INT16U out_len;               // 加密后发送的总长度

    struct timeval rec_TimeOut;
    sysPara *sys_cfg = SysConf_GetInfo();

    /* 通讯正常 */

    rec_TimeOut.tv_sec  = 0;
    rec_TimeOut.tv_usec = (20 * 1000);
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&rec_TimeOut, sizeof(rec_TimeOut)) == -1)
    {
        LOG_INFO("whitedevice SO_RCVTIMEO failed: %s (fd:%d)", strerror(errno), fd);
        if (fd > 0) close(fd);
        return;
    }

    signal(SIGPIPE, SIG_IGN);

    /* 连接计数 */
    connect_num++;
    connect_time++;
    LOG_INFO("whitedevice Client[%d] new connect success! (fd:%d)", connect_num, fd);

    unsigned short *pTimeCnt  = (unsigned short *)malloc(sizeof(unsigned short));
    unsigned int   *pConnectID = (unsigned int *)malloc(sizeof(unsigned int));
    if (!pTimeCnt || !pConnectID)
    {
        if (fd > 0) close(fd);
        free(pTimeCnt);
        free(pConnectID);
        return;
    }
    *pTimeCnt   = 0;
    *pConnectID = connect_time;

    while (1)
    {
        if ((*pConnectID) < connect_ID_min)
            connect_ID_min = (*pConnectID);

        memset(buffer, 0, sizeof(buffer));
        numbytes = recv(fd, buffer, TCP_SERVER_RECV_LEN, 0);

        /* 连接关闭/错误 */
        if (((numbytes < 0) && (errno != EAGAIN)) || (numbytes == 0))
        {
            if (numbytes == 0) LOG_INFO("whitedevice close the socket");
            if ((numbytes < 0) && (errno != EAGAIN)) LOG_INFO("whitedevice socket fault!");

            (*pTimeCnt) = 0;
            if (fd > 0) close(fd);
            connect_num--;
            LOG_INFO("whitedevice Client[%d] Close the TCP server socket! (fd:%d)", connect_num, fd);
            break;
        }
        /* 超时 */
        else if ((numbytes < 0) && (errno == EAGAIN))
        {
            (*pTimeCnt) += 1;
            if ((*pTimeCnt) > (50 * ((EMS_TIMEOUT_CNT / 100) + 5))) // 600s 清理僵尸
            {
                if (fd > 0) close(fd);
                connect_num--;
                LOG_INFO("whitedevice Client[%d] Close the TCP server socket! (fd:%d)", connect_num, fd);
                break;
            }
        }
        /* 正常收到加密包 */
        else
        {
         
            (*pTimeCnt) = 0;

            INT16U len = (INT16U)((buffer[4] << 8) | buffer[5]);
            if (6 + len != (INT16U)numbytes)
            {
                LOG_INFO("enc frame len mismatch: MBAP len=%u, total=%d", len, numbytes);
                return;
            }
            INT8U uid = (INT8U)buffer[6];

            /* 2) 取密文并解密 */
            const INT8U *cipher   = (const INT8U *)buffer + 7;
            size_t         cipher_len = (size_t)len - 1; // UID 后就是密文
            if (cipher_len < 8 || (cipher_len & 3))
            {
                // 4B 对齐且 >= 8
                LOG_INFO("enc payload invalid len=%zu", cipher_len);
                return;
            }



    INT8U pdu[512];
    if (!xxtea_decrypt_bytes(pdu, cipher, cipher_len, xxkey)) {
        LOG_INFO("xxtea decrypt failed");
        return;
    }

    size_t pdu_len = pkcs7_unpad4(pdu, cipher_len);


    INT8U  func  = pdu[0];
    INT16U addr  = ((INT16U)pdu[1] << 8) | pdu[2];
    INT16U count = ((INT16U)pdu[3] << 8) | pdu[4];
//LOG_INFO("func:0x%02X, addr:%u, count:%u", func, addr, count);

            switch (func)
            {
                case 0x03: /* Read Holding Registers */
                {
                    // 解密后的请求 PDU: 03 addrH addrL cntH cntL
                    if (pdu_len < 5) { LOG_INFO("0x03 pdu short"); break; }

                    INT16U start = (INT16U)(((INT16U)pdu[1] << 8) | pdu[2]);
                    INT16U cnt   = (INT16U)(((INT16U)pdu[3] << 8) | pdu[4]);
                    if (cnt == 0 || cnt > 125) { LOG_INFO("0x03 bad cnt=%u", cnt); break; }

                    INT8U SendbufferPDU[512]; // 仅存放“明文PDU”
                    size_t  MStarPDU = 0;       // ★★ 每帧必须从 0 开始 ★★

                    // 构造明文响应 PDU: 03 byteCount data...
                    SendbufferPDU[MStarPDU++] = 0x03;
                    SendbufferPDU[MStarPDU++] = (INT8U)(cnt * 2);

                    if (uid != 1) break;
                    for (INT16U i = 0; i < cnt; ++i)
                    {
                        INT16U v = GET_HOLD((INT16U)(start + i));
                        SendbufferPDU[MStarPDU++] = (INT8U)(v >> 8);
                        SendbufferPDU[MStarPDU++] = (INT8U)(v);
                    }

                    // 加密：pad → encrypt
                    INT8U plain[512], cipherOut[512], tx_encrypt_buf_local[600];
                    size_t  plen     = MStarPDU;                         // ★★ 用本帧实际长度 ★★
                    size_t  plen_pad = pkcs7_pad4(plain, SendbufferPDU, plen);//填充字节，4的倍数

                    if (!xxtea_encrypt_bytes(cipherOut, plain, plen_pad, xxkey))//PDU部分加密
                    {
                        LOG_INFO("0x03 encrypt failed");
                        break;
                    }

                    // 封装 MBAP：LEN = 1(UID)+密文长度
                    INT16U out_len_local = (INT16U)(1 + plen_pad);
                    tx_encrypt_buf_local[0] = buffer[0]; tx_encrypt_buf_local[1] = buffer[1]; // TID
                    tx_encrypt_buf_local[2] = buffer[2]; tx_encrypt_buf_local[3] = buffer[3]; // PID
                    tx_encrypt_buf_local[4] = (INT8U)(out_len_local >> 8);
                    tx_encrypt_buf_local[5] = (INT8U)(out_len_local);
                    tx_encrypt_buf_local[6] = buffer[6];                                      // UID
                    memcpy(&tx_encrypt_buf_local[7], cipherOut, plen_pad);

                    write(fd, tx_encrypt_buf_local, 6 + out_len_local);
                    DebugModbusBuf(" Sendbuffer!\n", tx_encrypt_buf_local, 6 + out_len_local); // 调试只打印“实际长度”
                    break;
                }

                case 0x04: /* Read Input Registers */
                {
                    // 解密后的请求 PDU: 04 addrH addrL cntH cntL
                    if (pdu_len < 5) { LOG_INFO("0x04 pdu short"); break; }

                    INT16U start = (INT16U)(((INT16U)pdu[1] << 8) | pdu[2]);
                    INT16U cnt   = (INT16U)(((INT16U)pdu[3] << 8) | pdu[4]);
                    if (cnt == 0 || cnt > 125) { LOG_INFO("0x04 bad cnt=%u", cnt); break; }

                    INT8U SendbufferPDU[512]; // 仅存放“明文PDU”
                    size_t  MStarPDU = 0;       // ★★ 每帧必须从 0 开始 ★★

                    // 构造明文响应 PDU: 04 byteCount data...
                    SendbufferPDU[MStarPDU++] = 0x04;
                    SendbufferPDU[MStarPDU++] = (INT8U)(cnt * 2);

                    for (INT16U i = 0; i < cnt; ++i)
                    {
                        if (uid == 1)
                        {
                            INT16U v = GET_INPUT((INT16U)(start + i));
                            SendbufferPDU[MStarPDU++] = (INT8U)(v >> 8);
                            SendbufferPDU[MStarPDU++] = (INT8U)(v);
                        }
                        else if ((uid >= 2) && (uid < (2 + BANK_SIZE)))
                        {
                            int bankN   = uid - 2;
                            INT16U v  = GET_C_INPUT(bankN, (INT16U)(start + i));
                            SendbufferPDU[MStarPDU++] = (INT8U)(v >> 8);
                            SendbufferPDU[MStarPDU++] = (INT8U)(v);
                        }
                        else
                        {
                            break;
                        }
                    }

                    // 加密：pad → encrypt
                    INT8U plain[512], cipherOut[512], tx_encrypt_buf_local[600];
                    size_t  plen     = MStarPDU;                         // ★★ 用本帧实际长度 ★★
                    size_t  plen_pad = pkcs7_pad4(plain, SendbufferPDU, plen);

                    if (!xxtea_encrypt_bytes(cipherOut, plain, plen_pad, xxkey))
                    {
                        LOG_INFO("0x04 encrypt failed");
                        break;
                    }

                    // 封装 MBAP：LEN = 1(UID)+密文长度
                    INT16U out_len_local = (INT16U)(1 + plen_pad);
                    tx_encrypt_buf_local[0] = buffer[0]; tx_encrypt_buf_local[1] = buffer[1]; // TID
                    tx_encrypt_buf_local[2] = buffer[2]; tx_encrypt_buf_local[3] = buffer[3]; // PID
                    tx_encrypt_buf_local[4] = (INT8U)(out_len_local >> 8);
                    tx_encrypt_buf_local[5] = (INT8U)(out_len_local);
                    tx_encrypt_buf_local[6] = buffer[6];                                      // UID
                    memcpy(&tx_encrypt_buf_local[7], cipherOut, plen_pad);

                    write(fd, tx_encrypt_buf_local, 6 + out_len_local);
                    DebugModbusBuf(" Sendbuffer!\n", tx_encrypt_buf_local, 6 + out_len_local); // 调试只打印“实际长度”
                    break;
                }

                // case 0x10: /* Preset Multiple Registers（原代码保留） */
                // {
                //     MLen     = (buffer[10] << 8) + buffer[11];
                //     AddrStar = Temp.D16;
                //     is_sub   = false;
                //     is_print = false;

                //     if (Temp.D16 >= 35001 && Temp.D16 <= 37974)
                //     {
                //         BMS_Write_Flag = true;
                //         BMSnum         = Find_Bank_N(Temp.D16, &Bank_Addr_Base) - 1;
                //         Write_Lenth     = MLen;
                //         Bank_addr       = Temp.D16;
                //         LOG_INFO("BMSnum is %d", BMSnum);
                //     }
                //     else if ((Temp.D16 >= 0 && Temp.D16 <= 1510) || Temp.D16 == 27000)
                //     {
                //         PCS_Write_Flag[0] = true;
                //         PCSWrite_Lenth[0]  = MLen;
                //         PCSaddr[0]        = Temp.D16;
                //     }
                //     else if (Temp.D16 == 27300)
                //     {
                //         PCS_Write_Flag[1] = true;
                //         PCSWrite_Lenth[1]  = MLen;
                //         PCSaddr[1]        = Temp.D16;
                //     }
                //     else
                //     {
                //         LOG_INFO("address invalid!");
                //     }

                //     for (i = 0; (i < MLen) && ((14 + i * 2) < TCP_SERVER_RECV_LEN); i++)
                //     {
                //         RegVal.D8[1] = buffer[13 + i * 2];
                //         RegVal.D8[0] = buffer[14 + i * 2];
                //         SET_HOLD(Temp.D16, RegVal.D16);
                //         Temp.D16++;
                //     }

                //     if ((false == is_sub) || (is_print == true))
                //     {
                //         memset(LogStr, 0, sizeof(LogStr));
                //         for (j = 0; (j < sizeof(LogStr)) && (j < (MLen * 2 + 13)); j++)
                //         {
                //             sprintf(str_char, "%x", buffer[j]);
                //             strcat(LogStr, str_char);
                //             strcat(LogStr, " ");
                //         }
                //         LOG_INFO("whitedevice set 0x10, address:%d, length:%d, buf=>%s", Temp.D16, MLen, LogStr);
                //     }

                //     if (AddrStar == SYS_SYNC_TIEM_ADDR)
                //     {
                //         year   = (buffer[13] << 8) + buffer[14];
                //         month  = (buffer[15] << 8) + buffer[16];
                //         day    = (buffer[17] << 8) + buffer[18];
                //         hour   = (buffer[19] << 8) + buffer[20];
                //         min    = (buffer[21] << 8) + buffer[22];
                //         second = (buffer[23] << 8) + buffer[24];
                //         memset(time_str, 0, sizeof(time_str));
                //         sprintf(time_str, "date -s '%d-%d-%d %d:%d:%d'", year, month, day, hour, min, second);
                //         Do_System(time_str, 2);
                //         Do_System("hwclock --systohc", 1);
                //         LOG_INFO("whitedevice synchronize time : %s", time_str);
                //         (*pIsCheckTime) = 1;
                //     }

                //     Sendbuffer[5]  = 6;
                //     Sendbuffer[8]  = buffer[8];
                //     Sendbuffer[9]  = buffer[9];
                //     Sendbuffer[10] = buffer[10];
                //     Sendbuffer[11] = buffer[11];
                //     break;
                // }

                // case 0x06: /* Write Single Register（加密回包部分保持原样） */
                // {
                //     if (uid != 1) break;

                //     INT8U SendbufferPDU[512]; // 仅存放“明文PDU”
                //     size_t  MStarPDU = 0;       // ★★ 每帧必须从 0 开始 ★★

                //     is_sub = false;

                //     INT8U plain[512], cipherOut[512], tx_encrypt_buf_local[600];

                //     RegVal.D8[1] = pdu[3];
                //     RegVal.D8[0] = pdu[4];
                //     SET_HOLD(Temp.D16, RegVal.D16);

                //     SendbufferPDU[MStarPDU++] = buffer[7];
                //     SendbufferPDU[MStarPDU++] = buffer[8];
                //     SendbufferPDU[MStarPDU++] = buffer[9];
                //     SendbufferPDU[MStarPDU++] = buffer[10];
                //     SendbufferPDU[MStarPDU++] = buffer[11];

                //     tx_encrypt_buf_local[0] = buffer[0]; tx_encrypt_buf_local[1] = buffer[1]; // TID
                //     tx_encrypt_buf_local[2] = buffer[2]; tx_encrypt_buf_local[3] = buffer[3]; // PID
                //     tx_encrypt_buf_local[4] = (INT8U)(buffer[4] >> 8);
                //     tx_encrypt_buf_local[5] = (INT8U)(buffer[5]);
                //     tx_encrypt_buf_local[6] = buffer[6];

                //     size_t plen     = MStarPDU;                         // ★★ 用本帧实际长度 ★★
                //     size_t plen_pad = pkcs7_pad4(plain, SendbufferPDU, plen);
                //     if (!xxtea_encrypt_bytes(cipherOut, plain, plen_pad, xxkey))
                //     {
                //         LOG_INFO("0x06 encrypt failed");
                //         break;
                //     }
                //     memcpy(&tx_encrypt_buf_local[7], cipherOut, plen_pad);

                //     if (false == is_sub)
                //         LOG_INFO("whitedevice set 0x06, address:%d -> data:%d", Temp.D16, RegVal.D16);

                //     if (Temp.D16 == 33200 && RegVal.D16 == 0xAA) { Cem9000_Write_Flag = true; Cem9000_Addr = 2000; Cem9000_Control_Value = 0xFF00; }
                //     else if (Temp.D16 == 33200 && RegVal.D16 == 0xEE) { Cem9000_Write_Flag = true; Cem9000_Addr = 2001; Cem9000_Control_Value = 0xFF00; }
                //     else if (Temp.D16 == 33201) { Cem9000_Write_Flag = true; Cem9000_Addr = 2002; Cem9000_Control_Value = 0xFF00; }

                //     for (INT8U sys_num = 0; (sys_num < sys_cfg->sysNum); sys_num++)
                //     {
                //         if (Temp.D16 == (27000 + 300 * sys_num + 103))
                //         {
                //             if (RegVal.D16 == 1) Set_In_Sys(sys_num, SYS_EVENT_CMD_START);
                //             else                  Set_In_Sys(sys_num, SYS_EVENT_CMD_STOP);
                //             break;
                //         }
                //     }

                //     int pcsnum = -1;
                //     SUB_State_ENUM state = Get_State_Sys(0);

                //     if (sys_cfg->pcs_brand[0] == PCS_PE)
                //     {
                //         if (state == SYSRun)
                //         {
                //             if      (Temp.D16 == 27000) pcsnum = 0;
                //             else if (Temp.D16 == 27300) pcsnum = 1;
                //         }
                //     }
                //     else if (sys_cfg->pcs_brand[0] == PCS_TRINA)
                //     {
                //         if (state == SYSRun)
                //         {
                //             if (Temp.D16 >= 27000 && Temp.D16 <= 28235) pcsnum = 0;
                //         }
                //     }
                //     else if (sys_cfg->pcs_brand[0] == PCS_Taida)
                //     {
                //         if ((Temp.D16 >= 27105 && Temp.D16 <= 27106) ||
                //             (Temp.D16 >= 27000 && Temp.D16 <= 27046) ||
                //             (Temp.D16 >= 27111 && Temp.D16 <= 27118))
                //         {
                //             pcsnum = 0;
                //         }
                //         else if ((Temp.D16 >= 27405 && Temp.D16 <= 27406) ||
                //                  (Temp.D16 >= 27300 && Temp.D16 <= 27346) ||
                //                  (Temp.D16 >= 27111 && Temp.D16 <= 27118))
                //         {
                //             pcsnum = 2;
                //         }
                //         else if (Temp.D16 == 27107) { PCSactivepower[0] = RegVal.D16; }
                //         else if (Temp.D16 == 27407) { PCSactivepower[1] = RegVal.D16; }
                //         else if (Temp.D16 == 27110) { PCSPF[0]        = RegVal.D16; }
                //         else if (Temp.D16 == 27410) { PCSPF[1]        = RegVal.D16; }
                //         else if (Temp.D16 == 1010)  { /* MV 总功率触发 */ }

                //         if      ((Temp.D16 >= 12000 && Temp.D16 <= 12025)) pcsnum = 0;
                //         else if (Temp.D16 >= 12300 && Temp.D16 <= 12325)   pcsnum = 1;
                //         else if ((Temp.D16 >= 12600 && Temp.D16 <= 12625)) pcsnum = 2;
                //         else if (Temp.D16 >= 12900 && Temp.D16 <= 12925)   pcsnum = 3;
                //     }

                //     if (pcsnum >= 0)
                //     {
                //         PcsWriteReq req;
                //         req.addr     = Temp.D16;
                //         req.value    = RegVal.D16;
                //         req.len      = 1;
                //         req.is_multi = false;
                //         Pcs_Write_Enqueue_Dedup_By_Addr(pcsnum, &req);
                //     }

                //     DebugModbusBuf(" Sendbuffer!\n", tx_encrypt_buf_local, 12);
                //     write(fd, tx_encrypt_buf_local, 12);
                //     break;
                // }

                default:
                    break;
            }
        }

        if ((connect_num >= (MAX_CLIENT_NUM - 1)) && (connect_ID_min == (*pConnectID)))
        {
            if (fd > 0) close(fd);
            connect_num--;
            LOG_INFO("whitedevice Client[%d] Close the TCP server socket! (fd:%d, connect_ID_min:%d)",
                     connect_num, fd, connect_ID_min);
            break;
        }

        usleep(100);
    }

    connect_ID_min = UINT_MAX;
    free(pTimeCnt);
    free(pConnectID);
    return NULL;
}


//密钥握手
static void server_handle_data_untrusted_body(int fd)
{
    hs_state_t st = HS_IDLE;
    INT8U token[16] = {0};
    make_xxtea_key_from_ascii("ABCDEFG", xxkey);// 按小端装入 4×u32

    struct timeval rec_To = {.tv_sec = 0, .tv_usec = 20 * 1000};
    (void)setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &rec_To, sizeof(rec_To));
    signal(SIGPIPE, SIG_IGN);

    const int MAX_RETRY = 3;
    const int MAX_WAIT_TICKS = 500; /* 约 5 秒（配合 usleep） */
    int retry = 0;
    int wait_ticks = 0;

    INT8U rx[TCP_SERVER_RECV_LEN] = {0};
    INT8U tx[TCP_SERVER_RECV_LEN] = {0};

    LOG_INFO("whitedevice guest path: fd=%d (untrusted client)", fd);

    for (;;)
    {
        int n = recv(fd, rx, sizeof(rx), 0);

       
        if (n == 0) {
           
            LOG_INFO("guest peer closed fd=%d", fd);
            close(fd);
            return;
        }
        if (n < 0) {
            if (errno == EINTR) {
                // 被信号打断，继续收
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 超时：等一会儿再试，避免忙等
                if (++wait_ticks > MAX_WAIT_TICKS) {
                    LOG_INFO("guest recv timeout too long fd=%d", fd);
                    close(fd);
                    return;
                }
                usleep(10 * 1000); // 10ms 退避
                continue;
            }
            // 其他错误：退出
            LOG_INFO("guest recv error fd=%d err=%d:%s", fd, errno, strerror(errno));
            close(fd);
            return;
        }
        // 有数据
        wait_ticks = 0; // 成功收到了，清零等待计数

        if (n < 8) {
           
            continue;
        }

        INT8U func = rx[7];

        switch (st)
        {
        case HS_IDLE:
            switch (func)
            {
            case FUNC_AUTH_REQ: {
                if (!get_random_bytes(token, sizeof(token))) {//获取一个随机数作为token
                    LOG_INFO("guest no random, reject fd=%d", fd);
                    close(fd);
                    return;
                }
                memcpy(tx, rx, 7);  // MBAP
                tx[7] = FUNC_AUTH_REQ;
                tx[8] = 16;         // token 长度
                memcpy(&tx[9], token, 16);
                tx[5] = 16 + 3;     // LEN = UID(1)+PDU(1+1+16)
                (void)write(fd, tx, 6 + tx[5]);//将token发给客户端

                st = HS_WAIT_RESP;
                retry = 0;
                wait_ticks = 0;
                LOG_INFO("guest issued token fd=%d", fd);
                break;
            }
            default:
                LOG_INFO("guest drop func=0x%02X (not authed)", func);
                break;
            }
            break;

        case HS_WAIT_RESP:
            switch (func)
            {

                case FUNC_AUTH_REQ: {
                if (!get_random_bytes(token, sizeof(token))) {//获取一个随机数作为token
                    LOG_INFO("guest no random, reject fd=%d", fd);
                    close(fd);
                    return;
                }
                memcpy(tx, rx, 7);  // MBAP
                tx[7] = FUNC_AUTH_REQ;
                tx[8] = 16;         // token 长度
                memcpy(&tx[9], token, 16);
                tx[5] = 16 + 3;     // LEN = UID(1)+PDU(1+1+16)
                (void)write(fd, tx, 6 + tx[5]);//将token发给客户端

                st = HS_WAIT_RESP;
                retry = 0;
                wait_ticks = 0;
                LOG_INFO("guest issued token fd=%d", fd);
                break;  
              }
            case FUNC_AUTH_RESP: {
                if (n < 9 + 16) {
                    LOG_INFO("guest auth_resp too short fd=%d", fd);
                    retry++;
                } else {
                    INT8U ciph[16];
                    memcpy(ciph, &rx[9], 16);//取客户端发送过来加密后的token报文
                    xxtea_decrypt_16(ciph, xxkey);//解密，解密最终会将结果给ciph
                    if (memcmp(ciph, token, 16) == 0) {
                        memcpy(tx, rx, 7);
                        tx[7] = FUNC_AUTH_RESP; tx[8] = 1; tx[9] = 0x01; tx[5] = 1 + 3;
                        (void)write(fd, tx, 6 + tx[5]);
                        LOG_INFO("guest auth OK fd=%d", fd);

                        // 进入加密数据通道
                        server_handle_XXTEAdata_body(fd);
                        return;
                    } else {
                        LOG_INFO("guest auth token mismatch fd=%d", fd);
                        retry++;
                    }
                }
                memcpy(tx, rx, 7);
                tx[7] = FUNC_AUTH_RESP; tx[8] = 1; tx[9] = 0x00; tx[5] = 1 + 3;
                (void)write(fd, tx, 6 + tx[5]);

                if (retry >= MAX_RETRY) {
                    LOG_INFO("guest auth retry exceeded fd=%d", fd);
                    close(fd);
                    return;
                }
                wait_ticks = 0;
                break;
            }
            default:
                LOG_INFO("guest drop func=0x%02X during auth fd=%d", func, fd);
                break;
            }
            break;

        case HS_AUTHED:
     
            break;
        }
    }
}


static void* server_handle_trusted(void* arg)
{
    int fd = *(int*)arg; free(arg);
    server_handle_data_body(fd);
    return NULL;
}

static void* server_handle_untrusted(void* arg)
{
    int fd = *(int*)arg; free(arg);
    server_handle_data_untrusted_body(fd);
    return NULL;
}


static void Accept_Client_Connect(int *pserver_fd)
{
    struct sockaddr_storage client_addr;
    socklen_t sin_size = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));

    int cfd = accept(*pserver_fd, (struct sockaddr *)&client_addr, &sin_size);
    if (cfd == -1) return;

    char iptxt[INET6_ADDRSTRLEN] = {0};
    INT16U port = 0;
    if (client_addr.ss_family == AF_INET) {
        struct sockaddr_in *a4 = (struct sockaddr_in *)&client_addr;
        inet_ntop(AF_INET, &a4->sin_addr, iptxt, sizeof(iptxt));
        port = ntohs(a4->sin_port);
    } else if (client_addr.ss_family == AF_INET6) {
        struct sockaddr_in6 *a6 = (struct sockaddr_in6 *)&client_addr;
        inet_ntop(AF_INET6, &a6->sin6_addr, iptxt, sizeof(iptxt));
        port = ntohs(a6->sin6_port);
    } else {
        strncpy(iptxt, "unknown", sizeof(iptxt)-1);
    }

    AclResult ar;
    bool allowed = acl_check_sockaddr((struct sockaddr*)&client_addr, sin_size, &ar);//查询数据库，匹配白名单IP

    LOG_INFO("whitedevice New Client (fd:%d) %s:%u  %s", cfd, iptxt, port,
             allowed ? "[whitelist]" : "[guest]");

    pthread_t th;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int *arg = (int*)malloc(sizeof(int));
    if (!arg) { close(cfd); pthread_attr_destroy(&attr); return; }
    *arg = cfd;

    void *(*entry)(void*) = allowed ? server_handle_trusted//白名单处理
                                    : server_handle_untrusted;//白名单外处理

    if (pthread_create(&th, &attr, entry, arg) != 0) {
        close(cfd);
        free(arg);
    }
    pthread_attr_destroy(&attr);
}


void Task_Whitelist_Server(void)
{
    static int sockfd_service = 0;
    static INT32U server_create_count = 0;
    sysPara *sys_cfg = SysConf_GetInfo();
    static in_port_t sin_port;
    sin_port = htons(sys_cfg->whitedeviceport);

    // pcs_write_queue_init();

    while (1)
    {
        if ((sockfd_service = Create_Server_Socket(sin_port, MAX_CLIENT_NUM)) == -1)
        {
            if (Get_EMS_Comm(0) == IsNoFault) {
                LOG_INFO("whitedevice Create server socket failure!, count:%d", server_create_count++);
            }
            sleep(10);
            continue;
        }
        LOG_INFO("whitedevice server[%d] : Create socket success! wait client connect ......", ntohs(sin_port));
      
        break;
    }

    while (1)
    {
        Accept_Client_Connect(&sockfd_service);
        usleep(100 * 1000);
    }
}

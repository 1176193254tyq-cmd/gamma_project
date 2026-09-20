
#define _POSIX_C_SOURCE 200809L
#include "main.h"
#include "time.h"
#include "modbus_protocol.h"
#include "taida_lc_ems.h"
#include "pf_convert_final.h"
unsigned short taida_pcs_state[MAX_PCS_NUM] = {0};
extern volatile INT16U Control_Word3_Buf[4];//台达PCS控制字3
volatile INT16U Slave1_sys_state2=0;
volatile INT16U Slave2_sys_state2=0;
volatile INT16U Slave1_sys_state1=0;
volatile INT16U Slave2_sys_state1=0;
extern INT32S PCSactivepower[10];
// static void taida_pcs_versionbit6_process(int num, INT16U addr, INT16U value)
// {
//     static int one[5] = {0};
//     static int ten[5] = {0};
//     static int hundred[5] = {0};
//     static int thousand[5] = {0};
//     static int ten_thousand[5] = {0};

//     static uint8_t flag[5] = {0};   

//     switch (addr)
//     {
//         case 18:
//             one[num] = value;
//             flag[num] |= (1 << 0);
//             break;

//         case 26:
//             ten[num] = value;
//             flag[num] |= (1 << 1);
//             break;

//         case 39:
//             hundred[num] = value;
//             flag[num] |= (1 << 2);
//             break;

//         case 31:
//             thousand[num] = value;
//             flag[num] |= (1 << 3);
//             break;

//         case 13:
//             ten_thousand[num] = value;
//             flag[num] |= (1 << 4);
//             break;

//         default:
//             break;
//     }

   
//     if (flag[num] == 0x1F)  
//     {
//         int version;

//         version = one[num]
//                 + ten[num] * 10
//                 + hundred[num] * 100
//                 + thousand[num] * 1000
//                 + ten_thousand[num] * 10000;

//         flag[num] = 0;   

//       SET_INPUT(2600+num*300+151, version);
//     }

 
// }
static INT16U Merge_Bit8_To_Bit13(INT16U main_value, INT16U slave_value)
{
    INT16U mask = (INT16U)(0x3FU << 8);

    main_value &= (INT16U)(~mask);          // 清除主值 bit8~bit13
    main_value |= (slave_value & mask);       // 写入从机 bit8~bit13

    return main_value;
}
/**
 * @brief 处理台达PCS设备接收到的Modbus数据
 * 
 * 该函数根据接收到的Modbus功能码(0x03/0x04/0x06/0x10)进行不同的数据处理：
 * - 0x03: 处理读保持寄存器响应
 * - 0x04: 处理读输入寄存器响应
 * - 0x06: 处理写单个寄存器响应
 * - 0x10: 处理写多个寄存器响应
 * 
 * @param ptr 指向接收到的数据缓冲区的指针
 * @param num PCS设备编号(0/1/2等)
 * @note 数据格式遵循Modbus协议，高字节在前，低字节在后
 * @note 处理过程中会检查数据有效性(sum==0则无效)
 * @note 处理完成后会设置PCS通讯状态为正常
 */

static void Taida_DataProcess(unsigned char *ptr, int num)
{
    // 接收的数据
    unsigned char pbuf[PCS_BUFF_LEN] = {0};
    // 初始化
    u16_conv temp,RegVal;
    int i = 0;
    INT16U val = 0;
    INT32U sum = 0;
    INT16U startaddr = 0;
    INT16U dataddr = 0;
    char str[15] = {0};
    INT8U sys_num=0;
    int index = 0;
    bool is_ok = 0;
    sysPara *sys_cfg = SysConf_GetInfo();
    memcpy(pbuf, ptr, PCS_BUFF_LEN);
    float S_OUT;
    float PF_OUT;
    if (pbuf[7] == 0x03 )
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // 
        // 返回的数据起始地址
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16; // 寄存器首地址
        dataddr = 9;
        is_ok = true;
        sum = 0;
        for (i = 0; i < (pbuf[8]/2); i++)
        { 
            sum += pbuf[i+dataddr];
        }
        if(sum==0)
        {
            is_ok = false;
        }
        //只有主机可以设置的点，特殊处理System Control Word1，System Control Word2 System Control Word3 MV Relay Control Word1
        if (((temp.D16 >= 99) && (temp.D16 <102 ))||(temp.D16==103))
        {
            int pcsnum = (num>= 2) ? 1 : 0;
            if(temp.D16==101)
            {

            }
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
         //将它们映射到遥信点位上
                Write_Bits_By_Addr(temp.D16+i,RegVal.D16,pcsnum);
                if((temp.D16+i)==101)
                {
                Control_Word3_Buf[pcsnum]=RegVal.D16;
                }

            } 


        }
        //处理主从机均可设置的寄存器
        else if(((temp.D16 >= 40201-40001) && (temp.D16 <40225-40001 ))||((temp.D16 >= 40241-40001) && (temp.D16 <40243-40001))||((temp.D16 >= 40011-40001) && (temp.D16 <40018-40001 )))
        {
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                index= Taida_HOLD_INDEX(num+1,temp.D16+i);//按照对外协议点表，

                SET_HOLD(index, RegVal.D16);
              //LOG_INFO("台达：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);

            } 
        }
        else if(temp.D16==40522-40001)//特殊点位 认证标准
        {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                index= Taida_INPUT_INDEX(num+1,temp.D16);//按照对外协议点表
                SET_INPUT(index, RegVal.D16);
        }
        //处理主机可设置的寄存器
        else 
        {
            int pcsnum = (num>= 2) ? 1 : 0;
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                index= Master_Taida_HOLD_INDEX(pcsnum+1,temp.D16+i);
            if(((( GET_INPUT(17000 + 300 * pcsnum + 64)) >> 1) & 1u) == 1)//如果是在功率因数模式下，则做特殊处理
            {
               if(index==27000+300*pcsnum+52)//台达功率因数符号转换后，再回读有功功率，有功会被清零，因此做特殊处理
                {
                   // P_PF_read_to_S_pf_out(GET_HOLD(17000+300*pcsnum+107),GET_HOLD(17000+300*pcsnum+108),RegVal.D16,&S_OUT,&PF_OUT);
               
                    SET_HOLD(index, PCSactivepower[pcsnum]);
                }
                else if(index==27000+300*pcsnum+110)//台达功率因数符号转换
                {
                    SET_HOLD(index, PCSPF[pcsnum]);
                }
            
                else
                {
                  SET_HOLD(index, RegVal.D16);  
                }
            }
            else
                {
                  SET_HOLD(index, RegVal.D16);  
                }
             // LOG_INFO("台达：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);

            } 
        }
    }
    else if (pbuf[7] == 0x04)
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // PCS通讯正常
        // 返回的数据起始地址
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16; // 寄存器首地址    
        dataddr = 9;
        is_ok = true;
        sum = 0;
        for (i = 0; i < (pbuf[8]/2); i++)
        { 
            sum += pbuf[i+dataddr];
        }
        if(sum==0)
        {
            is_ok = false;
        }
        //PCS Time Year ~Relay Ground Trip OCP Value①
        if ((temp.D16 >= 30045-30001) && (temp.D16 <=30160-30001 ))
        {
               int pcsnum = (num>= 2) ? 1 : 0;

            for (i = 0; (i < (pbuf[8] / 2)) && ((i * 2) < PCS_BUFF_LEN); i++)
            {
                RegVal.D8[1] = pbuf[dataddr++];
                RegVal.D8[0] = pbuf[dataddr++];

                index = Master_Taida_INPUT_INDEX(pcsnum + 1, temp.D16 + i);

                /* 先保存从机1的 sys_state2 */
                if ((num == 1) )
                {
                    if((temp.D16 + i) == (30130 - 30001))
                    {
                     Slave1_sys_state2 = RegVal.D16;   
                    }
                    else if((temp.D16 + i) == (30129 - 30001))
                    {
                      Slave1_sys_state1 = RegVal.D16;   
                    }
                   
                }
                /* 先保存从机2的 sys_state2 */
                else if ((num == 3))
                {
                    if((temp.D16 + i) == (30130 - 30001))
                    {
                     Slave2_sys_state2 = RegVal.D16;   
                    }
                    else if((temp.D16 + i) == (30129 - 30001))
                    {
                      Slave2_sys_state1 = RegVal.D16;   
                    }
                    
                }
                /* 只有处理主机1数据时，才允许合并 Slave1 */
                else if ((num == 0) && (index == Master1_Taida_System_State3))
                {
                    INT16U merge_value;
                    uint8_t pcs3_stat = Get_PCS_Comm(1);
                    if (pcs3_stat == IsFault)
                    {
                        Slave2_sys_state2 = 0;
                    }
                    merge_value = Merge_Bit8_To_Bit13(RegVal.D16, Slave1_sys_state2);
                    merge_value &= ~(1U << 14);   // 先清除 bit14

                    merge_value |= (((Slave1_sys_state1 >> 9) & 0x01U) << 14);
                  //  LOG_INFO("merge_value = %d", merge_value);
                    SET_INPUT(index, merge_value);
                }
                /* 只有处理主机2数据时，才允许合并 Slave2 */
                else if ((num == 2) && (index == Master2_Taida_System_State3))
                {
                    INT16U merge_value;
                    uint8_t pcs3_stat = Get_PCS_Comm(3);
                    if (pcs3_stat == IsFault)
                    {
                        Slave2_sys_state2 = 0;
                    }
                    merge_value = Merge_Bit8_To_Bit13(RegVal.D16, Slave2_sys_state2);
                    merge_value &= ~(1U << 14);   // 先清除 bit14

                    merge_value |= (((Slave2_sys_state1 >> 9) & 0x01U) << 14);
                    SET_INPUT(index, merge_value);
                }
                else if(index==17000+300*pcsnum+42)//台达功率因数符号转换
                {
                        float value;
                        INT16S PF_Value;

                        INT16S p_read_s16  = (INT16S)GET_INPUT(17000 + 300 * pcsnum + 40);
                        INT16S q_read_s16  = (INT16S)GET_INPUT(17000 + 300 * pcsnum + 41);
                        INT16S pf_read_s16 = (INT16S)RegVal.D16;

                        value = (float)pf_read_s16 / 10000.0f;

                        P_PF_read_to_S_pf_out(
                            (float)p_read_s16,
                            (float)q_read_s16,
                            value,
                            &S_OUT,
                            &PF_OUT);

                        PF_Value = (INT16S)roundf(PF_OUT * 10000.0f);

                        SET_INPUT(index, (INT16U)PF_Value);

                        // LOG_INFO("40_raw=%u,40_s16=%d,41_raw=%u,41_s16=%d,RegVal_raw=%u,pf_s16=%d,value=%f,PF_OUT=%f,PF_Value=%d",
                        //         GET_INPUT(17000 + 300 * pcsnum + 40),
                        //         p_read_s16,
                        //         GET_INPUT(17000 + 300 * pcsnum + 41),
                        //         q_read_s16,
                        //         RegVal.D16,
                        //         pf_read_s16,
                        //         value,
                        //         PF_OUT,
                        //         PF_Value);
                }
                else if(index==17000+300*pcsnum+52)//台达功率因数符号转换
                {
                        float value;
                        INT16S PF_Value;

                        INT16S p_read_s16  = (INT16S)GET_INPUT(17000 + 300 * pcsnum + 50);
                        INT16S q_read_s16  = (INT16S)GET_INPUT(17000 + 300 * pcsnum + 51);
                        INT16S pf_read_s16 = (INT16S)RegVal.D16;

                        value = (float)pf_read_s16 / 10000.0f;

                        P_PF_read_to_S_pf_out(
                            (float)p_read_s16,
                            (float)q_read_s16,
                            value,
                            &S_OUT,
                            &PF_OUT);

                        PF_Value = (INT16S)roundf(PF_OUT * 10000.0f);

                        SET_INPUT(index, (INT16U)PF_Value);

                }

                else
                {
                    SET_INPUT(index, RegVal.D16);
                }
            }
        }
        //Slave System Fault Word1~Slave Controller Fault Word1 
        if ((temp.D16 >= 30237-30001) && (temp.D16 <=30247-30001 ))
        {
              int pcsnum = (num!= 0) ? 3 : 1;
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
              
                index= Master_Taida_INPUT_INDEX(pcsnum+1,temp.D16+i);//按照对外协议点表

                    SET_INPUT(index, RegVal.D16);                   

               
                //LOG_INFO("台达：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);

            } 
        }
        else
        {
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
              
                index= Taida_INPUT_INDEX(num+1,temp.D16+i);//按照对外协议点表
                SET_INPUT(index, RegVal.D16);

             //   taida_pcs_versionbit6_process(num,temp.D16+i,RegVal.D16);
            
               //LOG_INFO("台达：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);
            } 
        }

    }
    else if (pbuf[7] == 0x06)
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // PCS通讯正常
        temp.D8[1] = pbuf[8]; // 寄存器地址H
        temp.D8[0] = pbuf[9]; // 寄存器地址L
        RegVal.D8[1] = pbuf[10];     //（高字节在前、低字节在后）
        RegVal.D8[0] = pbuf[11]; 
        startaddr = temp.D16; // 寄存器地址
        //当num==2时代表的是PCS组2的主机，也就是系统1
        if(num==2)
        {
          sys_num=1;
        }
        else
        {
          sys_num=0;  
        }

        if (((SYS_EVENT_STOP_PCS + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_start_addr == startaddr))
        {
            Success_Out_Sys(sys_num);
            SET_HOLD(27000 + 300 * sys_num+ 103,0);
        }
        else if (((SYS_EVENT_START_PCS + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_start_addr == startaddr))
        {
            Success_Out_Sys(sys_num);
            SET_HOLD(27000 + 300 * sys_num+ 103,1);
        }
        else if (((SYS_EVENT_START_FAULT + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_OPRATE_addr == startaddr))
        {
            Success_Out_Sys(sys_num);

        }
         else if (((SYS_EVENT_START_PQ + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_OPRATE_addr2 == startaddr))
        {
            Success_Out_Sys(sys_num);

        }
         else if (((SYS_EVENT_STOP_PQ + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_OPRATE_addr2 == startaddr))
        {
            Success_Out_Sys(sys_num);
           
        }
		//lj 20260107 此处需要核对
         else if (((SYS_EVENT_CMD_RESET + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_OPRATE_addr == startaddr))
        {
            Success_Out_Sys(sys_num);
           
        }
        else if (((SYS_EVENT_ZERO_POWER + 100) == Get_Out_Sys(sys_num)) && (PCS_Taida_ACPower_addr == startaddr))
        {
            Success_Out_Sys(sys_num);
            SET_HOLD(27107,RegVal.D16);
        }
        // else if (((SUB_EVENT_CMD_Power + 100) == Get_Out_Sys(sys_num)) && ((PCS_Taida_ACPower_addr) == startaddr))
        // {
        //     Success_Out_Sub(sys_num,1);
        //     SET_HOLD(27300,RegVal.D16);
        // }
        else if (((SUB_EVENT_CMD_LIM + 100) == Get_Out_Sub(sys_num,0)) && (PCS_PE_ACPower_addr == startaddr))
        {
            Success_Out_Sub(sys_num,0);
            SET_HOLD(27000,RegVal.D16);
        }
        else if (((SUB_EVENT_CMD_LIM + 100) == Get_Out_Sub(sys_num,1)) && ((PCS_PE_ACPower_addr+1) == startaddr))
        {
            Success_Out_Sub(sys_num,1);
            SET_HOLD(27300,RegVal.D16);
        }
    }
    else if (pbuf[7] == 0x10)
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // PCS通讯正常
    }
    else
    {
       DebugModbusBuf("PCS Invalid data!\n", pbuf, 12);
    }
}
/**
 * @brief PCS功率下发0功率，PCS实际功率下发改为多线程下发
 * @param[in] socket_fd 通信文件描述符
 * @param[in] pcs_num PCS序号
 * @retval 0 没发送指令 1 发送了指令
 */
static int PCS_Power(int socket_fd, const int pcs_num,const int sub_num)
{
    INT16S power_percent=0;
    u16_conv pcs_set_power[MAX_PCS_NUM][MAX_SUB_NUM]={0};    
    pcs_set_power[pcs_num][sub_num].D16=GET_HOLD(27107+sub_num*300); //实际下发的功率
    LOG_INFO("实际功率 %d,限制放电功率 %d，限制充电功率%d",GET_HOLD(27107+sub_num*300),LimPdischarge[pcs_num][sub_num],LimPcharge[pcs_num][sub_num]);
    LOG_INFO("PCS%d下发功率:%d",pcs_num,0);

    SET_HOLD(27107+sub_num*300,0);
    Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_ACPower_addr, 0 , CMD_DELAY_20);  
    LOG_INFO("有功清零");
    Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_REPower_addr, 0 , CMD_DELAY_20); 
    LOG_INFO("无功清零");
    return 0;
}
/**
 * @brief 台达PCS设备通讯任务线程
 * 
 * 该函数用于处理与台达PCS设备的Modbus TCP通讯，主要功能包括：
 * 1. 建立与PCS设备的TCP连接
 * 2. 根据不同状态循环发送控制指令和读取设备数据
 * 3. 处理通讯超时和异常情况
 * 4. 支持(P2P模式)切换
 * 
 * @param arg 线程参数，指向PCS设备编号的指针(int类型)
 * @note 设备编号0和2为主机，其他为从机
 * @note 主机负责发送控制指令和读取更多寄存器数据
 * @note 从机仅读取基本状态信息
 * @note 通讯超时时间约为400ms，连续75次超时判定为通讯异常
 * 
 * @return void 无返回值
 */
void PCS_Taida_Task(const char *arg)
{
    //初始化
    int pcs_num = *((int *)arg);  // 获取值
    sysPara *sys_cfg = SysConf_GetInfo();
    int socket_fd = 0;
    int loop = 0;
    int is_send = 0;
    int numbytes = 0;
    INT8U connect_cnt={0};
    INT8U sys_num=0;
    INT8U timeout_cnt=0;
    LOG_INFO("Taida PCS-%d线程启动",pcs_num);
    while (1)
    {
        //配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->pcs_port[pcs_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->pcs_ip[pcs_num]);
        struct timeval time_out = {0, (400 * 1000)};
        //创建本地客户端socket
        if ((socket_fd = Create_Client_Socket(server_addr, time_out)) == -1)
        {
            connect_cnt=connect_cnt<=LOG_PRINTF_CNT? connect_cnt+1 : connect_cnt;        //小于12自增
            if(connect_cnt<=LOG_PRINTF_CNT)
            {               
                LOG_INFO("PCS-%d 服务端连接失败! ip:%s[port:%d]", pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
            }
            sleep(CONNECT_DELAY_TIME);
            if(GET_INPUT(P2P_mode)==1)
            { 
             Set_PCS_Comm(pcs_num, IsNoFault, TRUE);   
            }
            continue;
        }
        else
        {
            connect_cnt=0;
            timeout_cnt = 0;
            //PCS通讯正常
            LOG_INFO("PCS-%d: 服务端连接成功! ip:%s[port:%d]", pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
            int recv_bytes = 0;
        
            while (1)
            {
       
            if(GET_INPUT(P2P_mode)==0)
            {
                switch(loop)
                {
                case 0:
                  if(pcs_num==0||pcs_num==2) 
                  {
                     sys_num = (pcs_num!= 0) ? 1 : 0;
                 //   LOG_INFO("此时数值：%d",Get_Out_Sys(sys_num));
                    if(SYS_EVENT_STOP_PCS==Get_Out_Sys(sys_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_start_addr,0 , CMD_DELAY_20);
                       
                        LOG_INFO("接收停机：%d",sys_num);
                        Wait_Out_Sys(sys_num);
                    }
                    else if(SYS_EVENT_START_PCS==Get_Out_Sys(sys_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_start_addr, 1 , CMD_DELAY_20);
                        
                        Wait_Out_Sys(sys_num);
                    }
                    else if(SYS_EVENT_START_FAULT==Get_Out_Sys(sys_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_OPRATE_addr, 2 , CMD_DELAY_20);
                        Wait_Out_Sys(sys_num);
                    }
                    else if(SYS_EVENT_START_PQ==Get_Out_Sys(sys_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_OPRATE_addr2, 1 , CMD_DELAY_20);
                        Wait_Out_Sys(sys_num);
                    }
                    else if(SYS_EVENT_STOP_PQ==Get_Out_Sys(sys_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_OPRATE_addr2, 0 , CMD_DELAY_20);
                        Wait_Out_Sys(sys_num);
                     
                    }
                    else if(SYS_EVENT_ZERO_POWER==Get_Out_Sys(sys_num))
                    {
                      
                        PCS_Power(socket_fd,sys_num,0);
                        Wait_Out_Sys(sys_num);
                    }
                    else if(SYS_EVENT_STANDBY==Get_Out_Sys(sys_num))
                    {
                      
                        PCS_Power(socket_fd,sys_num,0);
                        usleep(200*1000);

                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_HotStandbyEn, 1 , CMD_DELAY_20);
                        Wait_Out_Sys(sys_num);

                    }
                    else if(SUB_EVENT_CMD_LIM==Get_Out_Sub(sys_num,0))
                    {
                        PCS_Power(socket_fd,sys_num,0);
                        Wait_Out_Sub(sys_num,0);
                    }
                    else if(SUB_EVENT_CMD_LIM==Get_Out_Sub(sys_num,1))
                    {
                        PCS_Power(socket_fd,sys_num,1);
                        Wait_Out_Sub(sys_num,1);
                    }
					else if(SYS_EVENT_CMD_RESET==Get_Out_Sys(sys_num)) //lj 20260107 此处需要核对
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_Taida_SLAVE_ADDR, PCS_Taida_OPRATE_addr, 1 , CMD_DELAY_20);
                        Wait_Out_Sys(sys_num);                    
                    }
                }
                    loop++;
                    break;
                case 1:

                        Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr1, PCS_Master_Taida_size1, CMD_DELAY_20);                  

                    loop++;
                    break;
                case 2:
                   if(pcs_num==0||pcs_num==2) 
                    {
                        Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr2, PCS_Master_Taida_size2, CMD_DELAY_20);                   
                    }
                    loop++;
                    break;
                case 3:
                   if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr3, PCS_Master_Taida_size3, CMD_DELAY_20);
                    }
                    loop++;
                    break;
                case 4:
                    if(pcs_num==0||pcs_num==2) 
                    { 
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr4, PCS_Master_Taida_size4, CMD_DELAY_20);
                    }
                    loop++;
                    break;
                case 5:
                     if(pcs_num==0||pcs_num==2) 
                    {
                        Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr5, PCS_Master_Taida_size5, CMD_DELAY_20);
                    
                    }
                    loop++;
                    break;
                case 6:    
                 if(pcs_num==0||pcs_num==2) 
                    {                        
                        Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr6, PCS_Master_Taida_size6, CMD_DELAY_20);
                    
                    }    
                    loop++;
                    break;
                case 7: 
                 if(pcs_num==0||pcs_num==2) 
                    {              
                        Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr7, PCS_Master_Taida_size7, CMD_DELAY_20);            
                    }
                    loop++;
                    break;
                case 8:
                     if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr8, PCS_Master_Taida_size8, CMD_DELAY_20);
                    
                    }
                    loop++;
                    break;
                case 9:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr1, Each_PCS_Taida_size1, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 10:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr2, Each_PCS_Taida_size2, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 11:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr3, Each_PCS_Taida_size3, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;
                case 12:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr4, Each_PCS_Taida_size4, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 13:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr5, Each_PCS_Taida_size5, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 14:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr6, Each_PCS_Taida_size6, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;
                case 15:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr7, Each_PCS_Taida_size7, CMD_DELAY_20);
                    loop++;
                    break;
                case 16:
                  if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Master_Taida_addr16, PCS_Master_Taida_size16, CMD_DELAY_20);
                     }
                    loop++;
                  
                    break;
                case 17:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Taida_addr10, PCS_Taida_size10, CMD_DELAY_20);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;
                case 18:
                   if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Master_Taida_addr11, PCS_Master_Taida_size11, CMD_DELAY_20);
                    }
                    loop++;
                   
                    break;
                case 19:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Taida_addr12, PCS_Taida_size12, CMD_DELAY_20);
                    loop++;
                    break;
                case 20:
                    if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Master_Taida_addr13, PCS_Master_Taida_size13, CMD_DELAY_20);
                    
                    }
                    loop++;
                    break;
                case 21:
                    if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Master_Taida_addr14, PCS_Master_Taida_size14, CMD_DELAY_20);
                   
                    } loop++;
                    break;
                case 22:
                    if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Master_Taida_addr15, PCS_Master_Taida_size15, CMD_DELAY_20);
                   
                    } loop++;
                    break;
                 case 23:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr8, Each_PCS_Taida_size8, CMD_DELAY_20);
                    loop++;
                    break;
                  case 24:
                     if(pcs_num==0||pcs_num==2) 
                    {
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Master_Taida_addr9, PCS_Master_Taida_size9, CMD_DELAY_20);
                    
                    }
                    loop++;
                    break;
                  case 25:

                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Taida_addr13, PCS_Taida_size13, CMD_DELAY_20);
                
                    loop++;
                    break;
                   case 26:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr9, Each_PCS_Taida_size9, CMD_DELAY_20);
                    loop++;
                    break;
                    case 27:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr10, Each_PCS_Taida_size10, CMD_DELAY_20);
                    loop++;
                    break;
                    case 28:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_04, Each_PCS_Taida_addr11, Each_PCS_Taida_size11, CMD_DELAY_20);
                    loop++;
                    break;
                    case 29:
                    Modbus_TCP_Read(socket_fd, PCS_Taida_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Taida_addr14, PCS_Taida_size14, CMD_DELAY_20);
                    loop++;
                    break;
                default:
                    loop = 0;
                    break;
                }

                numbytes=Recv_Modbus_Back(socket_fd, pcs_num, PCS_BUFF_LEN, Taida_DataProcess);         
                // LOG_INFO("numbytes is %d,pcs_num is %d",numbytes,pcs_num);
                 Update_Pcs_Diff_Status(sys_cfg->pcsNum);
                  if (numbytes < 0)  
                {
                     LOG_INFO("PCS-%d:其他套接字故障 ", pcs_num);
                    close(socket_fd);
                    LOG_INFO("PCS-%d: 连接异常，断开服务端连接 ", pcs_num);
                    break;
                }
                else if (numbytes == 0) {
                     timeout_cnt++;
                   if (timeout_cnt >= 75) {   //超时时间约为400ms
                     LOG_INFO("PCS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                     pcs_num, timeout_cnt);
                     close(socket_fd);               
                     break;
                }

                 }
                else 
                {
                    timeout_cnt=0;
                    Set_PCS_Comm(pcs_num, IsNoFault, TRUE);                                  // PCS通讯正常
                }
                    usleep(1 * 1000);
            }
            else 
            {
               Set_PCS_Comm(pcs_num, IsNoFault, TRUE);  //进入对点模式
                  usleep(50 * 1000);
            }
            //轮询周期
        } //loop read
 
    } //loop connect
}
    close(socket_fd);
    return;
}

/*台达软件版本、安规标准、PCS模式校验*/

static uint8_t Pcs_Is_Online(uint8_t pcs)
{
    INT16U online_flag;

    if ((pcs < 1) || (pcs > PCS_MAX_NUM))
    {
        return 0;
    }

    online_flag = GET_INPUT(218);

    if ((online_flag & (1U << (pcs - 1))) != 0)
    {
        return 1;
    }

    return 0;
}

static uint8_t Get_Online_Pcs_Num(uint8_t pcs_total_num)
{
    uint8_t pcs;
    uint8_t online_num = 0;

    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }

    for (pcs = 1; pcs <= pcs_total_num; pcs++)
    {
        if (Pcs_Is_Online(pcs))
        {
            online_num++;
        }
    }

    return online_num;
}

static uint8_t Check_Pcs_software_version_Diff(uint8_t pcs_total_num,
                                               INT16U start_offset,
                                               INT16U end_offset)
{
    uint8_t pcs;
    uint8_t ref_valid;
    INT16U offset;
    INT16U ref_val;
    INT16U cur_val;

    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }

    if (Get_Online_Pcs_Num(pcs_total_num) <= 1)
    {
        return 0;
    }

    for (offset = start_offset; offset <= end_offset; offset++)
    {
        ref_valid = 0;
        ref_val = 0;

        for (pcs = 1; pcs <= pcs_total_num; pcs++)
        {
            if (!Pcs_Is_Online(pcs))
            {
                continue;
            }

            cur_val = GET_INPUT(PCS_INPUT_BASE1 + PCS_ADDR_STEP * (pcs - 1) + offset);

            if (ref_valid == 0)
            {
                ref_val = cur_val;
                ref_valid = 1;
            }
            else
            {
                if (cur_val != ref_val)
                {
                    return 1;
                }
            }
        }
    }

    return 0;
}

static uint8_t Check_Pcs_Mode_Bit_Diff(uint8_t pcs_total_num,
                                       INT16U reg_offset,
                                       INT16U bit_mask)
{
    uint8_t pcs;
    uint8_t ref_valid = 0;
    INT16U ref_val = 0;
    INT16U cur_val = 0;

   if (pcs_total_num <= 1)
    {
        return 0;
    }
    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }



    for (pcs = 1; pcs <= pcs_total_num; pcs++)
    {


        cur_val = GET_INPUT(PCS_INPUT_BASE2 + PCS_ADDR_STEP * (pcs - 1) + reg_offset) & bit_mask;

        if (ref_valid == 0)
        {
            ref_val = cur_val;
            ref_valid = 1;
        }
        else
        {
            if (cur_val != ref_val)
            {
                return 1;
            }
        }
    }

    return 0;
}
static uint8_t Check_Pcs_Bus_Mode_Bit_Diff(uint8_t pcs_total_num)
{
    uint8_t pcs;
    uint8_t ref_valid = 0;
    INT16U ref_val = 0;
    INT16U cur_val = 0;

   if (pcs_total_num <= 1)
    {
        return 0;
    }
    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }



    for (pcs = 1; pcs <= pcs_total_num; pcs++)
    {


      cur_val = (GET_INPUT(PCS_INPUT_BASE2 +
                     PCS_ADDR_STEP * (pcs - 1) + 60) >> 9) & 0x01;//主机BUS模式
      ref_val = (GET_INPUT(PCS_INPUT_BASE2 +
                     PCS_ADDR_STEP * (pcs - 1) + 62) >> 14) & 0x01;//从机BUS模式
      // LOG_INFO("pcs %d, cur_val = %d, ref_val = %d ,pcs_total_num =%d \n", pcs, cur_val, ref_val,pcs_total_num);
            if (cur_val != ref_val)
            {
                return 1;
            }
       
    }

    return 0;
}
static uint8_t Check_Pcs_Master_Slave_diff(uint8_t pcs_total_num)
{
    INT16U pcs1_val;
    INT16U pcs2_val;
    INT16U pcs3_val;
    INT16U pcs4_val;

    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }

    if (pcs_total_num <= 1)
    {
        return 0;
    }

    /*
     * 2台或3台：
     * 只有 PCS1 和 PCS2 都在线，才比较 1、2。
     * 两者数值相同，bit3 置 1。
     */
    if ((pcs_total_num == 2) || (pcs_total_num == 3))
    {
        if (Pcs_Is_Online(1) && Pcs_Is_Online(2))
        {
            pcs1_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 0 + 24);
            pcs2_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 1 + 24);

            if (pcs1_val == pcs2_val)
            {
                return 1;
            }
        }

        return 0;
    }

    /*
     * 4台：
     * PCS1/PCS2 都在线才比较第一组；
     * PCS3/PCS4 都在线才比较第二组；
     * 只要任意一组数值相同，bit3 置 1。
     */
    if (pcs_total_num == 4)
    {
        if (Pcs_Is_Online(1) && Pcs_Is_Online(2))
        {
            pcs1_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 0 + 24);
            pcs2_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 1 + 24);

            if (pcs1_val == pcs2_val)
            {
                return 1;
            }
        }

        if (Pcs_Is_Online(3) && Pcs_Is_Online(4))
        {
            pcs3_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 2 + 24);
            pcs4_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 3 + 24);

            if (pcs3_val == pcs4_val)
            {
                return 1;
            }
        }

        return 0;
    }

    return 0;
}

void Update_Pcs_Diff_Status(uint8_t pcs_total_num)
{
    INT16U input223 = 0;
    INT16U input224 = 0;
    INT16U input225 = 0;
    INT16U sys_num = 0;
    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }

    if (Check_Pcs_software_version_Diff(pcs_total_num, 152, 156))
        input223 |= (1U << 0);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 157, 161))
        input223 |= (1U << 1);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 162, 164))
        input223 |= (1U << 2);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 165, 169))
        input223 |= (1U << 3);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 170, 174))
        input223 |= (1U << 4);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 175, 177))
        input223 |= (1U << 5);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 178, 182))
        input223 |= (1U << 6);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 183, 185))
        input223 |= (1U << 7);

    SET_INPUT(223, input223);

    /*
     * 认证标准
     */
    if (Check_Pcs_software_version_Diff(pcs_total_num, 186, 186))
        input224 |= (1U << 0);

    SET_INPUT(224, input224);

    /*
     * SET_INPUT(225)
     */
    sys_num = (pcs_total_num >= 3) ? 2 : 1;
    if (Check_Pcs_Bus_Mode_Bit_Diff(sys_num))
        input225 |= (1U << 0);

    if (Check_Pcs_Mode_Bit_Diff(sys_num, 64, (1U << 5)))
        input225 |= (1U << 1);

    if (Check_Pcs_Mode_Bit_Diff(sys_num, 64, 0x001F))
        input225 |= (1U << 2);

    if (Check_Pcs_Master_Slave_diff(pcs_total_num))
        input225 |= (1U << 3);

    SET_INPUT(225, input225);
}
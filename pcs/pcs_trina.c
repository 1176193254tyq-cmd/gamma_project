
#define _POSIX_C_SOURCE 200809L
#include "main.h"
#include "time.h"
#include "modbus_protocol.h"

int Trina_Pcs_Read_Socket[10]={0};//pcs socket
static void Trina_DataProcess(unsigned char *ptr, int num)
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
    INT16U slave_addr = 0;
    INT16U offset_addr = 0;
    char str[15] = {0};
    INT8U sys_num=0;
    int index = 0;
    bool is_ok = 0;
    sysPara *sys_cfg = SysConf_GetInfo();
    memcpy(pbuf, ptr, PCS_BUFF_LEN);
    slave_addr = pbuf[6];

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
         if(((temp.D16 >= PCS_Trina_03addr1) && (temp.D16 <PCS_Trina_03addr8)))
        {
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                index= Trina_HOLD_INDEX(num+1,temp.D16+i);//按照对外协议点表，
                SET_HOLD(index, RegVal.D16);
            //   LOG_INFO("天合：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);
            } 
        }
        else if ((temp.D16 >= PCS_Trina_03addr8) && (temp.D16 < PCS_Trina_03addr9))
        {
            switch (slave_addr)
            {
            case PCS_Trina_SLAVE_J1_ADDR:
                offset_addr = 0;
                break;
            case PCS_Trina_SLAVE_J2_ADDR:
                offset_addr = 40;
                break;
            /*其他slave不处理*/
            default:
                LOG_INFO("data area: [%d : %d], invalid slave addr: %d", temp.D16, temp.D16 + pbuf[8]/2, slave_addr);
                return;
            }
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                index= Trina_HOLD_INDEX(num+1,temp.D16+i);//按照对外协议点表，
                SET_HOLD(index+ offset_addr, RegVal.D16);
            //   LOG_INFO("天合：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);
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
        //Local Controller Version 1~Relay Ground Trip OCP Value①
        if ((temp.D16 >= 4000) && (temp.D16 <=5000 ))
        {
 

            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
              
                index= Trina_INPUT_INDEX(num+1,temp.D16+i);//按照对外协议点表
                SET_INPUT(index, RegVal.D16);
             //   LOG_INFO("trina:%d,temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);

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
 * @param[in] Trina_Pcs_Read_Socket[pcs_num] 通信文件描述符
 * @param[in] pcs_num PCS序号
 * @retval 0 没发送指令 1 发送了指令
 */
static int PCS_Power(const int pcs_num,const int sub_num)
{
    INT16S power_percent=0;
    u16_conv pcs_set_power[MAX_PCS_NUM][MAX_SUB_NUM]={0};    
    pcs_set_power[pcs_num][sub_num].D16=GET_HOLD(27107+sub_num*300); //实际下发的功率
    LOG_INFO("实际功率 %d,限制放电功率 %d，限制充电功率%d",GET_HOLD(27107+sub_num*300),LimPdischarge[pcs_num][sub_num],LimPcharge[pcs_num][sub_num]);
    LOG_INFO("PCS%d下发功率:%d",pcs_num,0);

    SET_HOLD(27107+sub_num*300,0);
    Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_num],PCS_Trina_SLAVE_ADDR, PCS_Taida_ACPower_addr, 0 , CMD_DELAY_200);  
    LOG_INFO("有功清零");
    Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_num],PCS_Trina_SLAVE_ADDR, PCS_Taida_REPower_addr, 0 , CMD_DELAY_200); 
    LOG_INFO("无功清零");
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
void PCS_Trina_Task(const char *arg)
{
    //初始化
    int pcs_num = *((int *)arg);  // 获取值
    sysPara *sys_cfg = SysConf_GetInfo();
  
    int loop = 0;
    int is_send = 0;
    int numbytes = 0;
    INT8U connect_cnt={0};
    INT8U sys_num=0;
    INT8U timeout_cnt=0;
    int pcs_idx1 = 0;
    int pcs_idx2 = 0;
    LOG_INFO("Trina PCS-%d线程启动",pcs_num);
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
        if ((Trina_Pcs_Read_Socket[pcs_num] = Create_Client_Socket(server_addr, time_out)) == -1)
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

                    sys_num = (pcs_num!= 0) ? 1 : 0;
                    pcs_idx1 = sys_num * 2;
                    pcs_idx2 = sys_num * 2 + 1;
                    if(SYS_EVENT_STOP_PCS==Get_Out_Sys(sys_num))
                    {
                      if (Trina_Pcs_Read_Socket[pcs_idx1] != NULL) {
                        Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx1], PCS_Trina_SLAVE_ADDR, Power_on_addr, 0, CMD_DELAY_200);
                        }
                        if (Trina_Pcs_Read_Socket[pcs_idx2] != NULL) {
                            Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx2], PCS_Trina_SLAVE_ADDR, Power_on_addr, 0, CMD_DELAY_200);
                        }
                        Wait_Out_Sys(sys_num);
                    }
                    else if(SYS_EVENT_START_PCS==Get_Out_Sys(sys_num))
                    {
                      if (Trina_Pcs_Read_Socket[pcs_idx1] != NULL) {
                        Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx1], PCS_Trina_SLAVE_ADDR, Power_on_addr, 1, CMD_DELAY_200);
                        }
                        if (Trina_Pcs_Read_Socket[pcs_idx2] != NULL) {
                            Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx2], PCS_Trina_SLAVE_ADDR, Power_on_addr, 1, CMD_DELAY_200);
                        }
                        Wait_Out_Sys(sys_num);
                    }

                    else if(SYS_EVENT_START_PQ==Get_Out_Sys(sys_num))
                    {
                      if (Trina_Pcs_Read_Socket[pcs_idx1] != NULL) {
                        Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx1], PCS_Trina_SLAVE_ADDR, control_mode_addr, 3, CMD_DELAY_200);
                        }
                        if (Trina_Pcs_Read_Socket[pcs_idx2] != NULL) {
                            Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx2], PCS_Trina_SLAVE_ADDR, control_mode_addr, 3, CMD_DELAY_200);
                        }
                          Wait_Out_Sys(sys_num);
                    }

                    else if(SYS_EVENT_ZERO_POWER==Get_Out_Sys(sys_num))
                    {
                      if (Trina_Pcs_Read_Socket[pcs_idx1] != NULL) {
                        Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx1], PCS_Trina_SLAVE_ADDR, Activate_power_addr, 0, CMD_DELAY_200);
                        }
                        if (Trina_Pcs_Read_Socket[pcs_idx2] != NULL) {
                            Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_idx2], PCS_Trina_SLAVE_ADDR, Activate_power_addr, 0, CMD_DELAY_200);
                        }                    
                
                        Wait_Out_Sys(sys_num);
                    }


					else if(SYS_EVENT_CMD_RESET==Get_Out_Sys(sys_num)) //lj 20260107 此处需要核对
                    {
                        Modbus_TCP_Write06_SingleRegist(Trina_Pcs_Read_Socket[pcs_num],PCS_Trina_SLAVE_ADDR, PCS_Taida_OPRATE_addr, 1 , CMD_DELAY_200);
                        Wait_Out_Sys(sys_num);                    
                    }
               
                    loop++;
                    break;
                case 1:

                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr1, PCS_Trina_04size1, CMD_DELAY_100);                  

                    loop++;
                    break;
                case 2:

                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr2, PCS_Trina_04size2, CMD_DELAY_100);                   

                    loop++;
                    break;
                case 3:

                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr3, PCS_Trina_04size3, CMD_DELAY_100);

                    loop++;
                    break;
                case 4:

                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr4, PCS_Trina_04size4, CMD_DELAY_100);

                    loop++;
                    break;
                case 5:

                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr5, PCS_Trina_04size5, CMD_DELAY_100);

                    loop++;
                    break;
                case 6:    
                       
                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr6, PCS_Trina_04size6, CMD_DELAY_100);
                    
 
                    loop++;
                    break;
                case 7: 
             
                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr1, PCS_Trina_03size1, CMD_DELAY_100);            

                    loop++;
                    break;
                case 8:

                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr2, PCS_Trina_03size2, CMD_DELAY_100);
                    

                    loop++;
                    break;
                case 9:
                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr3, PCS_Trina_03size3, CMD_DELAY_100);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 10:
                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr4, PCS_Trina_03size4, CMD_DELAY_100);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 11:
                   Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr5, PCS_Trina_03size5, CMD_DELAY_100);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;
                case 12:
                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr6, PCS_Trina_03size6, CMD_DELAY_100);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 13:
                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr7, PCS_Trina_03size7, CMD_DELAY_100);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                case 14:
                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_J1_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr8, PCS_Trina_03size8, CMD_DELAY_100);
                   // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;
                case 15:
                    Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_J2_ADDR, MODBUS_READ_TYPE_03, PCS_Trina_03addr8, PCS_Trina_03size8, CMD_DELAY_100); //PCS_Trina_03addr9
                    loop++;
                    break;
                case 16:

                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr7, PCS_Trina_04size7, CMD_DELAY_100);

                    loop++;
                    break;
                case 17:    
                       
                        Modbus_TCP_Read(Trina_Pcs_Read_Socket[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr8, PCS_Trina_04size8, CMD_DELAY_100);
                    
 
                    loop++;
                    break;
                default:
                    loop = 0;
                    break;
                }

                numbytes=Recv_Modbus_Back(Trina_Pcs_Read_Socket[pcs_num], pcs_num, PCS_BUFF_LEN, Trina_DataProcess);         
                // LOG_INFO("numbytes is %d,pcs_num is %d",numbytes,pcs_num);
                Update_Trina_Pcs_Diff_Status(sys_cfg->pcsNum);
                  if (numbytes < 0)  
                {
                     LOG_INFO("PCS-%d:其他套接字故障 ", pcs_num);
                    close(Trina_Pcs_Read_Socket[pcs_num]);
                    Trina_Pcs_Read_Socket[pcs_num] = -1;
                    LOG_INFO("PCS-%d: 连接异常，断开服务端连接 ", pcs_num);
                    break;
                }
                else if (numbytes == 0) {
                     timeout_cnt++;
                   if (timeout_cnt >= 75) {   //超时时间约为400ms
                     LOG_INFO("PCS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                     pcs_num, timeout_cnt);
                     close(Trina_Pcs_Read_Socket[pcs_num]);   
                     Trina_Pcs_Read_Socket[pcs_num] = -1;            
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
    close(Trina_Pcs_Read_Socket[pcs_num]);
    return;
}

/**/
/*台达软件版本、安规标准、PCS模式校验*/

static uint8_t Pcs_Is_Online(uint8_t pcs)
{
    uint16_t online_flag;

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
                                               uint16_t start_offset,
                                               uint16_t end_offset)
{
    uint8_t pcs;
    uint8_t ref_valid;
    uint16_t offset;
    uint16_t ref_val;
    uint16_t cur_val;

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


static uint8_t Check_Pcs_Mode_Diff(uint8_t pcs_total_num,
                                               uint16_t start_offset,
                                               uint16_t end_offset)
{
    uint8_t pcs;
    uint8_t ref_valid;
    uint16_t offset;
    uint16_t ref_val;
    uint16_t cur_val;

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

            cur_val = GET_HOLD(12000 + 700 * (pcs - 1) + offset);

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
static uint8_t Check_Pcs_Master_Slave_diff(uint8_t pcs_total_num)
{
    uint16_t pcs1_val;
    uint16_t pcs2_val;
    uint16_t pcs3_val;
    uint16_t pcs4_val;

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
            pcs1_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 0 + 9);
            pcs2_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 1 + 9);

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
            pcs1_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 0 + 9);
            pcs2_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 1 + 9);

            if (pcs1_val == pcs2_val)
            {
                return 1;
            }
        }

        if (Pcs_Is_Online(3) && Pcs_Is_Online(4))
        {
            pcs3_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 2 + 9);
            pcs4_val = GET_HOLD(PCS_HOLD_BASE + PCS_ADDR_STEP * 3 + 9);

            if (pcs3_val == pcs4_val)
            {
                return 1;
            }
        }

        return 0;
    }

    return 0;
}

void Update_Trina_Pcs_Diff_Status(uint8_t pcs_total_num)
{
    uint16_t input223 = 0;
    uint16_t input224 = 0;
    uint16_t input225 = 0;
    uint16_t sys_num = 0;
    if (pcs_total_num > PCS_MAX_NUM)
    {
        pcs_total_num = PCS_MAX_NUM;
    }

    if (Check_Pcs_software_version_Diff(pcs_total_num, 0, 0))
        input223 |= (1U << 0);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 1, 1))
        input223 |= (1U << 1);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 2, 2))
        input223 |= (1U << 2);

    if (Check_Pcs_software_version_Diff(pcs_total_num, 3, 3))
        input223 |= (1U << 3);



    SET_INPUT(223, input223);

    /*
     * 认证标准
     */
    if (Check_Pcs_Mode_Diff(pcs_total_num, 100, 100))
        input224 |= (1U << 0);

    SET_INPUT(224, input224);

    /*
     * SET_INPUT(225)
     */
   
    if (Check_Pcs_Mode_Diff(pcs_total_num, 0, 0))
        input225 |= (1U << 0);

    if (Check_Pcs_Mode_Diff(pcs_total_num, 2, 2))
        input225 |= (1U << 1);

    if (Check_Pcs_Mode_Diff(sys_num, 3, 3))
        input225 |= (1U << 2);

    if (Check_Pcs_Master_Slave_diff(pcs_total_num))
        input225 |= (1U << 3);

    SET_INPUT(225, input225);
}
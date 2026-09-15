
#include "main.h"
#include "modbus_protocol.h"
unsigned short pcs_state[MAX_PCS_NUM] = {0};
static void PE_DataProcess(unsigned char *ptr, int num)
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

    int index = 0;
    bool is_ok = 0;
    
    sysPara *sys_cfg = SysConf_GetInfo();
    memcpy(pbuf, ptr, PCS_BUFF_LEN);


    // 簇数据 读取寄存器返回
    if (pbuf[7] == 0x03 )
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
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
        if ((temp.D16 >= 0) && (temp.D16 <addresslimit ))
        {
       

            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                index= PE_INPUT_INDEX(temp.D16+i);//按照对外协议点表，需要将PE的这部分数据放到INPUT中，能对EMS
                SET_INPUT(index, RegVal.D16);
                //LOG_INFO("temp.D16 is %d,RegVal.D16 is %d,index is %d",(temp.D16+i),RegVal.D16,index);
                if(sys_status==(temp.D16+i))
                {
                    pcs_state[num] = RegVal.D16;

                 // LOG_INFO("pcs_state is %d,RegVal.D16 is %d",pcs_state[0],RegVal.D16);
                }
            } 
        }
        else
        {
            is_ok = false;
        }
        if(is_ok == true) 
        {
 
           
        }
    }
    else if (pbuf[7] == 0x06)
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
        temp.D8[1] = pbuf[8]; // 寄存器地址H
        temp.D8[0] = pbuf[9]; // 寄存器地址L
        RegVal.D8[1] = pbuf[10];     //（高字节在前、低字节在后）
        RegVal.D8[0] = pbuf[11]; 
        startaddr = temp.D16; // 寄存器地址
        if (((SYS_EVENT_STOP_PCS + 100) == Get_Out_Sys(num)) && (PCS_PE_OPRATE_addr == startaddr))
        {
            Success_Out_Sys(num);
            SET_HOLD(6,4);
        }
        else if (((SYS_EVENT_START_PCS + 100) == Get_Out_Sys(num)) && (PCS_PE_OPRATE_addr == startaddr))
        {
            Success_Out_Sys(num);
            SET_HOLD(6,3);
        }
        else if (((SUB_EVENT_CMD_Power + 100) == Get_Out_Sub(num,0)) && (PCS_PE_ACPower_addr == startaddr))
        {
            Success_Out_Sub(num,0);
            SET_HOLD(27000,RegVal.D16);
        }
        else if (((SUB_EVENT_CMD_Power + 100) == Get_Out_Sub(num,1)) && ((PCS_PE_ACPower_addr+1) == startaddr))
        {
            Success_Out_Sub(num,1);
            SET_HOLD(27300,RegVal.D16);
        }
        else if (((SUB_EVENT_CMD_LIM + 100) == Get_Out_Sub(num,0)) && (PCS_PE_ACPower_addr == startaddr))
        {
            Success_Out_Sub(num,0);
            SET_HOLD(27000,RegVal.D16);
        }
        else if (((SUB_EVENT_CMD_LIM + 100) == Get_Out_Sub(num,1)) && ((PCS_PE_ACPower_addr+1) == startaddr))
        {
            Success_Out_Sub(num,1);
            SET_HOLD(27300,RegVal.D16);
        }
    }
    else if (pbuf[7] == 0x10)
    {
        Set_PCS_Comm(num, IsNoFault, TRUE);
    }
    else
    {
        DebugModbusBuf("PCS Invalid data!\n", pbuf, 30);
    }
}
/**
 * @brief PCS功率下发
 * @param[in] socket_fd 通信文件描述符
 * @param[in] pcs_num PCS序号
 * @retval 0 没发送指令 1 发送了指令
 */
static int PCS_Power(int socket_fd, const int pcs_num,const int sub_num)
{
    INT16S power_percent=0;
    u16_conv pcs_set_power[MAX_PCS_NUM][MAX_SUB_NUM]={0};    
    pcs_set_power[pcs_num][sub_num].D16=GET_HOLD(27000+sub_num*300); //实际下发的功率
    LOG_INFO("实际功率 %d,限制放电功率 %d，限制充电功率%d",GET_HOLD(27000+sub_num*300),LimPdischarge[pcs_num][sub_num],LimPcharge[pcs_num][sub_num]);
    if(pcs_set_power[pcs_num][sub_num].DS16>LimPdischarge[pcs_num][sub_num])
    {
        pcs_set_power[pcs_num][sub_num].DS16=(INT16S)LimPdischarge[pcs_num][sub_num];
    }
    else if(pcs_set_power[pcs_num][sub_num].DS16<LimPcharge[pcs_num][sub_num])
    {
        pcs_set_power[pcs_num][sub_num].DS16=(INT16S)LimPcharge[pcs_num][sub_num];
    }
    //power_percent = (INT16S)pcs_set_power[pcs_num];
    //power_percent=(pcs_set_power[pcs_num][sub_num].DS16*100)/PCS_PE_MAX_POWER;  
    LOG_INFO("PCS%d下发功率:%d",pcs_num,pcs_set_power[pcs_num][sub_num].DS16);
    // if(power_percent<(-4000))
    // {
    //     power_percent = -4000;
    // }
    // if(power_percent>4000)
    // {pcs_set_power[pcs_num][sub_num].DS16
    //     power_percent = 4000;
    //}
    SET_HOLD(27000+sub_num*300,pcs_set_power[pcs_num][sub_num].DS16);
    Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_PE_SLAVE_ADDR, PCS_PE_ACPower_addr+sub_num, pcs_set_power[pcs_num][sub_num].DS16 , CMD_DELAY_200); //下发功率          
}
void PCS_PE_Task(const char *arg)
{
    //初始化
    int pcs_num = *((int *)arg);  // 获取值

    sysPara *sys_cfg = SysConf_GetInfo();
    int socket_fd = 0;
    int loop = 0;
    int is_send = 0;
    int numbytes = 0;
    INT8U connect_cnt={0};
    LOG_INFO("PEPCS-%d线程启动",pcs_num);
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
            continue;
        }
        else
        {
            connect_cnt=0;
            //PCS通讯正常
            LOG_INFO("PCS-%d: 服务端连接成功! ip:%s[port:%d]", pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
           Set_PCS_Comm(pcs_num, IsNoFault, TRUE);
            int recv_bytes = 0;
            while (1)
            {
                
                switch(loop)
                {
                case 0:
                    if(SYS_EVENT_STOP_PCS==Get_Out_Sys(pcs_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_PE_SLAVE_ADDR, PCS_PE_OPRATE_addr,0 , CMD_DELAY_200);
                        Wait_Out_Sys(pcs_num);
                    }
                    else if(SYS_EVENT_START_PCS==Get_Out_Sys(pcs_num))
                    {
                        Modbus_TCP_Write06_SingleRegist(socket_fd,PCS_PE_SLAVE_ADDR, PCS_PE_OPRATE_addr, 1 , CMD_DELAY_200);
                        Wait_Out_Sys(pcs_num);
                    }
                    else if(SUB_EVENT_CMD_Power==Get_Out_Sub(pcs_num,0))
                    {
                        PCS_Power(socket_fd,pcs_num,0);
                        Wait_Out_Sub(pcs_num,0);
                    }
                    else if(SUB_EVENT_CMD_Power==Get_Out_Sub(pcs_num,1))
                    {
                        PCS_Power(socket_fd,pcs_num,1);
                        Wait_Out_Sub(pcs_num,1);
                    }
                    else if(SUB_EVENT_CMD_LIM==Get_Out_Sub(pcs_num,0))
                    {
                        PCS_Power(socket_fd,pcs_num,0);
                        Wait_Out_Sub(pcs_num,0);
                    }
                    else if(SUB_EVENT_CMD_LIM==Get_Out_Sub(pcs_num,1))
                    {
                        PCS_Power(socket_fd,pcs_num,1);
                        Wait_Out_Sub(pcs_num,1);
                    }
                    loop++;
                    break;
                case 1:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr1, PCS_PEsize1, CMD_DELAY_200);
                    loop++;
                    break;
                case 2:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr2, PCS_PEsize2, CMD_DELAY_200);
                    loop++;
                    break;
                case 3:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr3, PCS_PEsize3, CMD_DELAY_200);
                    loop++;
                    break;
                case 4:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr4, PCS_PEsize4, CMD_DELAY_200);
                    loop++;
                    break;
                case 5:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr5, PCS_PEsize5, CMD_DELAY_200);
                    loop++;
                    break;
                case 6:                            
                        Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr6, PCS_PEsize6, CMD_DELAY_200);
                    loop++;
                    break;
                case 7:               
                        Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr7, PCS_PEsize7, CMD_DELAY_200);            
                    loop++;
                    break;
                case 8:
                        Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr8, PCS_PEsize8, CMD_DELAY_200);
                    loop++;
                    break;

                case 9:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr9, PCS_PEsize9, CMD_DELAY_200);
                    loop++;
                    break;
                case 10:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr10, PCS_PEsize10, CMD_DELAY_200);
                    loop++;
                    break;
                case 11:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr11, PCS_PEsize11, CMD_DELAY_200);
                    loop++;
                    break;
                case 12:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr12, PCS_PEsize12, CMD_DELAY_200);
                    loop++;
                    break;
                case 13:                            
                        Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr13, PCS_PEsize13, CMD_DELAY_200);
                    loop++;
                    break;
                case 14:               
                        Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr14, PCS_PEsize14, CMD_DELAY_200);            
                    loop++;
                    break;
                case 15:
                        Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr15, PCS_PEsize15, CMD_DELAY_200);
                    loop++;
                    break;

                case 16:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr16, PCS_PEsize16, CMD_DELAY_200);
                    loop++;
                    break;
                case 17:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr17, PCS_PEsize17, CMD_DELAY_200);
                    loop++;
                    break;
                case 18:
                    Modbus_TCP_Read(socket_fd, PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PEaddr19, PCS_PEsize19, CMD_DELAY_200);
                    loop++;
                    break;
               
                default:
                    loop = 0;
                    break;
                }
                numbytes=Recv_Modbus_Back(socket_fd, pcs_num, PCS_BUFF_LEN, PE_DataProcess);

              // LOG_INFO("numbytes:%d",numbytes);

                  if (numbytes < 0)  
                {
                        LOG_INFO("PCS-%d:其他套接字故障 ", pcs_num);
                   

                    close(socket_fd);
                    LOG_INFO("PCS-%d: 连接异常，断开服务端连接 ", pcs_num);
                    break;
                }
            
                else 
                {
                   Set_PCS_Comm(pcs_num, IsNoFault, TRUE);                                 // PCS通讯正常
                }
                if (Get_PCS_Comm(pcs_num) == 1) //多次timeout 或者 invalid data，已经判了超时
                {
                    close(socket_fd);
                    LOG_INFO("PCS-%d: 通讯异常，断开服务端连接 ", pcs_num);
                    break;
                }
                usleep(50 * 1000);
            }
            //轮询周期
        } //loop read

    } //loop connect

    close(socket_fd);
    return;
}
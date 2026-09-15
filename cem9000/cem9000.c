
#include "main.h"
#include "modbus_protocol.h"
extern  BOOLEAN Cem9000_Write_Flag;
extern INT32U  Cem9000_Control_Value;
extern INT32U Cem9000_Addr;
 INT16U powertestflag = false;
 INT16U actpowervalue = 0;
 INT16U actpowervaddr = 0;
static void Cem_DataProcess(unsigned char *ptr, int num)
{
    // 接收的数据
    unsigned char pbuf[Rec_BUFF_LEN] = {0};
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
    memcpy(pbuf, ptr, Rec_BUFF_LEN);


    // 簇数据 读取寄存器返回
    if (pbuf[7] == 0x04 )
    {
        Set_MEASU_Comm(0,IsNoFault);
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
        if ((temp.D16 >= 1000) && (temp.D16 <1016 ))
        {
       

            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 

                CEM9000_INPUT(temp.D16+i, RegVal.D16);
               // LOG_INFO("temp.D16+i:%d,RegVal.D16:%d",temp.D16+i,RegVal.D16);
           
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
        else if (pbuf[7] == 0x03 )
    {
        Set_MEASU_Comm(0,IsNoFault);
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
        if ((temp.D16 >= 0) && (temp.D16 <189 ))
        {
       

            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 

               index= Cem9000_INPUT_INDEX(temp.D16+i);
               SET_INPUT(index, RegVal.D16);
         //LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,index);
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
    
    else if (pbuf[7] == 0x01)
    {   
        LOG_INFO("CEM9000: 读取寄存器数据");
        Set_MEASU_Comm(0,IsNoFault);        
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16; // 寄存器首地址
         dataddr = 9;
         RegVal.D8[1] = 0;     //（高字节在前、低字节在后）
         RegVal.D8[0] = pbuf[dataddr]; 


        CEM9000_HOLD(RegVal.D16);

           

     //DebugModbusBuf("Cem9000 control data:\n", pbuf, 30);
    }
    else if (pbuf[7] == 0x10)
    {
  
    }
    else if (pbuf[7] == 0x5)
    {
    DebugModbusBuf("Cem9000 data\n", pbuf, 30);
    }
    else
    {
        DebugModbusBuf("Cem9000 Invalid data!\n", pbuf, 30);
    }
}





void Cem9000_Task(const char *arg)
{
    //初始化
    int measure_num = *((int *)arg);  // 获取值

    sysPara *sys_cfg = SysConf_GetInfo();
    int socket_fd = 0;
    int loop = 0;
    int is_send = 0;
    int numbytes = 0;
    int timeout_cnt=0;
    INT8U connect_cnt={0};
    LOG_INFO("Cem9000-%d线程启动",measure_num);

    while (1)
    {
        //配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->measure_port[measure_num]);

        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->measure_ip[measure_num]);
        struct timeval time_out = {0, (400 * 1000)};
        //创建本地客户端socket
        if ((socket_fd = Create_Client_Socket(server_addr, time_out)) == -1)
        {
            connect_cnt=connect_cnt<=LOG_PRINTF_CNT? connect_cnt+1 : connect_cnt;        //小于12自增
              
                LOG_INFO("Cem9000-%d 服务端连接失败! ip:%s[port:%d]", measure_num, sys_cfg->measure_ip[measure_num], sys_cfg->measure_port[measure_num]);

            sleep(CONNECT_DELAY_TIME);
            if(GET_INPUT(P2P_mode)==1)
            { 
                    Set_MEASU_Comm(0,IsNoFault); //进入对点模式     
            }
            continue;
        }
        else
        {
            connect_cnt=0;
            //PCS通讯正常
            LOG_INFO("Cem9000-%d: 服务端连接成功! ip:%s[port:%d]", measure_num, sys_cfg->measure_ip[measure_num], sys_cfg->measure_port[measure_num]);

            int recv_bytes = 0;
            timeout_cnt=0;
            while (1)
            {
              if(GET_INPUT(P2P_mode)==0)
                {               
                switch(loop)
                {
                case 0:
                  if(Cem9000_Write_Flag==true)

                    {
                        Cem9000_Write_Flag=false;


                        Modbus_TCP_Write05_SingleCoil(socket_fd, Cem9000_SLAVE_ADDR, Cem9000_Addr, Cem9000_Control_Value, CMD_DELAY_200);

                    }

                    loop++;
                    break;
                case 1:
                  if(measurecloseflag==true)

                    {
                        measurecloseflag=false;


                        Modbus_TCP_Write05_SingleCoil(socket_fd, Cem9000_SLAVE_ADDR, high_vol_switch_addr, 0XFF00, CMD_DELAY_200);

                    }

                    loop++;
                    break; 

                case 2:
                  Modbus_TCP_Read(socket_fd, Cem9000_SLAVE_ADDR, MODBUS_READ_TYPE_03, Cem9000_Addr1, cem9000size1, CMD_DELAY_200);
                    loop++;
                    break;   
                case 3:
                   Modbus_TCP_Read(socket_fd, Cem9000_SLAVE_ADDR, MODBUS_READ_TYPE_03, Cem9000_Addr4, cem9000size4, CMD_DELAY_200);
                    loop++;
                    break; 
                case 4:
                  Modbus_TCP_Read(socket_fd, Cem9000_SLAVE_ADDR, MODBUS_READ_TYPE_04, Cem9000_Addr2, cem9000size2, CMD_DELAY_200);
                    loop++;
                    break;
                // case 4:
                //   Modbus_TCP_Read(socket_fd, Cem9000_SLAVE_ADDR, MODBUS_READ_TYPE_01, Cem9000_Addr3, cem9000size3, CMD_DELAY_200);
                //     loop++;
                //     break;              
                default:
                    loop = 0;
                    break;
                }
                numbytes=Recv_Modbus_Back(socket_fd, measure_num, PCS_BUFF_LEN, Cem_DataProcess);

                  if (numbytes < 0)  
                {
                    LOG_INFO("Cem9000-%d:其他套接字故障 ", measure_num);
                   

                    close(socket_fd);
                    LOG_INFO("Cem9000-%d: 连接异常，断开服务端连接 ", measure_num);
                    break;
                }
                 else if (numbytes == 0) {

                    timeout_cnt++;
                   if (timeout_cnt >= 150) {  
                     LOG_INFO("MEASURE: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                    timeout_cnt);

                    close(socket_fd);
               
                    break;
                }

                 }
                 else 
                {
                    timeout_cnt=0;
                    Set_MEASU_Comm(0,IsNoFault);                                // PCS通讯正常
                }
                usleep(50 * 1000);
                }
                else
                {
             
                 Set_MEASU_Comm(0,IsNoFault); //进入对点模式
                 usleep(50 * 1000);

                }

            }
            //轮询周期
        } //loop read

    } //loop connect

    close(socket_fd);
    return;
}
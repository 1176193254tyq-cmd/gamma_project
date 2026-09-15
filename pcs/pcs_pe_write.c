#include "main.h"
#include "pcs_pe_write.h"
int socket_Pcs[10]={0};
extern INT8U BMSnum;



INT32U PCS_Addr_Base;
static bool firsteadenable =true;
void* Pcs_Pe_Write_Task(const char *arg);
void  PCS_PE_write_Task(const char *_num)
{
    //初始化
    const char pcs_num = *(_num);

    sysPara* sys_cfg = SysConf_GetInfo();
    int recv_bytes = 0;

    struct sockaddr_in server_addr = {0};
    int pcs_socket = 0;

    INT8U connect_cnt=0;
    
    LOG_INFO("PCS_wirte_task-%d", pcs_num);
  
    // 设置线程属性
    pthread_attr_t thread_attrR;
    pthread_attr_init(&thread_attrR);
    pthread_attr_setdetachstate(&thread_attrR, PTHREAD_CREATE_DETACHED);

    // 创建不同的线程 ID
    pthread_t thread_pcs;
   // 创建线程
    pthread_create(&thread_pcs, &thread_attrR, Pcs_Pe_Write_Task, (void*)&pcs_num);
    while(1)
    {
    usleep(50*1000);

    }

}

static void PCS_PE_Write(int num)
{

INT16U sendbuf[100];
INT16U value;
if (PCS_Write_Flag[num]==false)
{
    return ;

}

if(PCS_Write_Flag[num]==true)
{
    PCS_Write_Flag[num]=false;
    if(PCSWrite_Lenth[num]>1)
    {
        for(int i=0;i<PCSWrite_Lenth[num];i++)
    {

        sendbuf[i]= GET_HOLD(PCSaddr[num]+i);
        LOG_INFO("sendbuf[%d]=%d",i,sendbuf[i]);
    }
    PCS_Addr_Base= PE_HOLD_ADDRESS(PCSaddr[num]);
    LOG_INFO("PCS_Addr_Base is %d,lenth is %d",PCS_Addr_Base,PCSWrite_Lenth[num]);
   Modbus_TCP_Write_Multiple(socket_Pcs[num], PCS_PE_SLAVE_ADDR, PCS_Addr_Base, PCSWrite_Lenth[num], sendbuf, CMD_DELAY_200);
   

    }
    else 
    {

    value= GET_HOLD(PCSaddr[num]);
  
    PCS_Addr_Base= PE_HOLD_ADDRESS(PCSaddr[num]);
    Modbus_TCP_Write06_SingleRegist(socket_Pcs[num],PCS_PE_SLAVE_ADDR,PCS_Addr_Base, value, CMD_DELAY_200);

    LOG_INFO("v is %d,addr is %d",value,PCSaddr[num]);
    }
}
    
}
static void PCS_PE_Write_DataProcess(unsigned char *ptr, int num)
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
    int address = 0;
    bool is_ok = 0;
    INT8U Rack_id;
    sysPara *sys_cfg = SysConf_GetInfo();
    memcpy(pbuf, ptr, PCS_BUFF_LEN);
    // 簇数据 读取寄存器返回
    if (pbuf[7] == 0x03)
    {
       
        Set_PCS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
        // 返回的数据起始地址
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16; // 寄存器首地址
        dataddr = 9;
        is_ok = true;
        sum = 0;
       // LOG_INFO("startaddr is %d",startaddr);
        for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)
        { 
            sum += pbuf[i+dataddr];
        }
        if(sum==0)
        {
            is_ok = false;
        }

       if ((startaddr>= 0) && (startaddr < 48000))
        {
          
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=PE_HOLD_INDEX(temp.D16+i);
              if(firsteadenable==true)
                {
                    firsteadenable=false;
                    SET_HOLD(address, RegVal.D16);

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
        startaddr = temp.D16; // 寄存器地址
    }
    else if (pbuf[7] == 0x10)
    {
        Set_PCS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
    }
    else
    {
        DebugModbusBuf("PCS Invalid data!\n", pbuf, 30);
    }
}
void* Pcs_Pe_Write_Task(const char *arg) {
    

    sysPara *sys_cfg = SysConf_GetInfo();

    int pcs_num = *((int *)arg);  // 获取值


    LOG_INFO("PCS write task started for PCS-%d", pcs_num);
    INT8U loop = 0;
    INT8U Pcs_id = 2;//PE的地址
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i, j;
    char str_char[2] = "";
    INT8U Task_Is_Over = 0;
    INT8U status;
 
   
    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->pcs_port[pcs_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->pcs_ip[pcs_num]);
        struct timeval time_out = {0, (200 * 1000)};
        // 创建本地客户端socket
        if ((socket_Pcs[pcs_num] = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("PCS-%d Create write socket failure! ip:%s[port:%d]", pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
            sleep(5);
            continue;
        }
   else{// BMS通讯正常
        LOG_INFO("pcs-%d: write Connect SUCCESS! ip:%s[port:%d]", pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
        // 原先状态为警告态不做定时器复位
        if (IsWarn == Get_PCS_Comm(pcs_num)) {
            Set_PCS_Comm(pcs_num, IsNoFault, FALSE);
        } else {
            Set_PCS_Comm(pcs_num, IsNoFault, TRUE);
        }
        int read_recv_res = 0;

        while (1) {
         
            switch (loop) {
                case 0:
                    Modbus_TCP_Read(socket_Pcs[pcs_num], PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PE_write_addr1 , PCS_PE_write_size1, CMD_DELAY_200);
                    loop++;
                    break;
                case 1:
                    Modbus_TCP_Read(socket_Pcs[pcs_num], PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PE_write_addr2 , PCS_PE_write_size2, CMD_DELAY_200);
                    loop++;
                    break;

                case 2:
                    Modbus_TCP_Read(socket_Pcs[pcs_num], PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PE_write_addr3 , PCS_PE_write_size3, CMD_DELAY_200);
                    loop++;
                    break;
                case 3:
                    Modbus_TCP_Read(socket_Pcs[pcs_num], PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PE_write_addr4 , PCS_PE_write_size4, CMD_DELAY_200);
                    loop++;
                    break;
                case 4:
                    Modbus_TCP_Read(socket_Pcs[pcs_num], PCS_PE_SLAVE_ADDR, MODBUS_READ_TYPE_03, PCS_PE_write_addr5 , PCS_PE_write_size5, CMD_DELAY_200);
                    loop++;
                    break;
                default:
                    loop = 0;
                    break;
            }
     
            PCS_PE_Write(pcs_num);

            read_recv_res = Recv_Modbus_Back(socket_Pcs[pcs_num], pcs_num, Rack_XIE_BUFF_LEN, PCS_PE_Write_DataProcess);
      
            if (read_recv_res < 0) {
                LOG_INFO("PCS-%d: close the PCS read socket, after receive back data fault", pcs_num);
                close(socket_Pcs[pcs_num]);

                break;
            }
            // BMS通讯正常
            else if (read_recv_res > 0) {
                Set_PCS_Comm(pcs_num, IsNoFault, TRUE);
            } 
            else {
                status = Get_PCS_Comm(pcs_num);
                // 多次timeout 或者 invalid data，已经判了超时
                if (status == IsFault) {
                    LOG_INFO("PCS-%d: close the PCS socket, time out happen", pcs_num);
                    close(socket_Pcs[pcs_num]);

                    break;
                } else if (status == IsWarn) { // 超时之前就做4次重选
                    LOG_INFO("PCS-%d: close the PCS socket, time warn happen", pcs_num);
                    close(socket_Pcs[pcs_num]);
                    usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改

                    break;
                }  
                    usleep(500);
             }

            }
    }
    }
    close(socket_Pcs[pcs_num]);
    return;
}
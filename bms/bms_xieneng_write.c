#include "bms_xieneng_write.h"
#include "main.h"
extern INT8U BMSnum;
extern BOOLEAN BMS_Write_Flag;
extern INT32U Write_Lenth;
extern INT32U Bank_Addr_Base;
extern INT32U Bank_addr;
int socket_Bank[10]={0};
static bool firstreadenable;//只在开机的时候读取一次，后续如果有设置，可以直接从GET_HOID中获取
void* Bank_XIE_Write_Task(const char *arg);
void  BMS_XieNeng_write_Task(const char *_num)
{
    //初始化
    const char bms_num = *((int*)_num);
  
    sysPara* sys_cfg = SysConf_GetInfo();
    int recv_bytes = 0;

    struct sockaddr_in server_addr = {0};
    int bms_socket = 0;

    INT8U connect_cnt=0;
    
    LOG_INFO("协能BMS_wirte_task-%d", bms_num);
  
    // 设置线程属性
    pthread_attr_t thread_attrR;
    pthread_attr_init(&thread_attrR);
    pthread_attr_setdetachstate(&thread_attrR, PTHREAD_CREATE_DETACHED);

    // 创建不同的线程 ID
    pthread_t thread_bank, thread_rank0_5, thread_rank6_10;
   // 创建线程
    pthread_create(&thread_bank, &thread_attrR, Bank_XIE_Write_Task, (void*)&bms_num);
    while(1)
    {
    usleep(100);

    }

}

static void Bank_xie_Write(int num)
{

INT16U sendbuf[100];
INT16U Bank_id=1;
INT16U value;
if (BMS_Write_Flag==false)
{
    return ;

}

if(BMS_Write_Flag==true)
{
    BMS_Write_Flag=false;
    LOG_INFO("num[%d]",num);
    if(Write_Lenth>1)//接收EMS的长度如果大于1，代表EMS下发的是10功能码
    {
        for(int i=0;i<Write_Lenth;i++)
    {

        sendbuf[i]= GET_HOLD(Bank_addr+i);
    }
   Modbus_TCP_Write_Multiple(socket_Bank[num], Bank_id, Bank_Addr_Base, Write_Lenth, sendbuf, CMD_DELAY_200);
    }
    else 
    {

    value= GET_HOLD(Bank_addr);
    Modbus_TCP_Write06_SingleRegist(socket_Bank[num],Bank_id,Bank_Addr_Base, value, CMD_DELAY_200);

    LOG_INFO("v is %d,addr is %d",value,Bank_addr);
    }
}
  

}
static void Bank_XIE_Write_DataProcess(unsigned char *ptr, int num)
{
    // 接收的数据
    unsigned char pbuf[Rack_XIE_BUFF_LEN] = {0};
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
    memcpy(pbuf, ptr, Rack_XIE_BUFF_LEN);
    Rack_id = pbuf[6];
  
    // 簇数据 读取寄存器返回
    if (pbuf[7] == 0x03)
    {
        Set_BMS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
        // 返回的数据起始地址
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16; // 寄存器首地址
        dataddr = 9;
        is_ok = true;
        sum = 0;
        for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)
        { 
            sum += pbuf[i+dataddr];
        }
        if(sum==0)
        {
            is_ok = false;
        }
        if ((temp.D16 >= 0) && (temp.D16 < 100000))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=HOLD_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1

                SET_HOLD(address, RegVal.D16);
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
        Set_BMS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
        temp.D8[1] = pbuf[8]; // 寄存器地址H
        temp.D8[0] = pbuf[9]; // 寄存器地址L
        startaddr = temp.D16; // 寄存器地址
    }
    else if (pbuf[7] == 0x10)
    {
        Set_BMS_Comm(num, IsNoFault, TRUE); // BMS通讯正常
    }
    else
    {
        DebugModbusBuf("BMS Invalid data!\n", pbuf, 30);
    }
}
void* Bank_XIE_Write_Task(const char *arg) {
    

    sysPara *sys_cfg = SysConf_GetInfo();

    int bms_num = *((int *)arg);  // 获取值


    LOG_INFO("Bank write task started for BMS-%d", bms_num);
    INT8U loop = 0;
    INT8U Bank_id = 1;
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
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (200 * 1000)};
        // 创建本地客户端socket
        if ((socket_Bank[bms_num] = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("BMS-%d Create write socket failure! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
            sleep(5);
            continue;
        }
        // BMS通讯正常
        LOG_INFO("BMS-%d: write Connect SUCCESS! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
        // 原先状态为警告态不做定时器复位
        if (IsWarn == Get_BMS_Comm(bms_num)) {
            Set_BMS_Comm(bms_num, IsNoFault, FALSE);
        } else {
            Set_BMS_Comm(bms_num, IsNoFault, TRUE);
        }
        int read_recv_res = 0;

        while (1) {
         
            switch (loop) {
                case 0:
                    Modbus_TCP_Read(socket_Bank[bms_num], Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr0 , Bank_block_hold_size0, CMD_DELAY_200);
                    loop++;
                    break;
                case 1:
                    Modbus_TCP_Read(socket_Bank[bms_num], Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr1 , Bank_block_hold_size1, CMD_DELAY_200);
                    loop++;
                    break;

                case 2:
                    Modbus_TCP_Read(socket_Bank[bms_num], Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr2 , Bank_block_hold_size2, CMD_DELAY_200);
                    loop++;
                    break;
                case 3:
                    Modbus_TCP_Read(socket_Bank[bms_num], Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr3 , Bank_block_hold_size3, CMD_DELAY_200);
                    loop++;
                    break;
                default:
                    loop = 0;
                    break;
            }
        if(bms_num==BMSnum)//当EMS设置不同序号的BMS时，通过该判断来区分，命令下发到哪一个线程（BMS）
        {
             Bank_xie_Write(BMSnum);
        }
            
            // 接收BMS的返回数据
            read_recv_res = Recv_Modbus_Back(socket_Bank[bms_num], bms_num, Rack_XIE_BUFF_LEN, Bank_XIE_Write_DataProcess);
            // BMS通讯异常
            if (read_recv_res < 0) {
                LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                close(socket_Bank[bms_num]);
                Task_Is_Over = 1;
                break;
            }
            // BMS通讯正常
            else if (read_recv_res > 0) {
                Set_BMS_Comm(bms_num, IsNoFault, TRUE);
            } else {
                status = Get_BMS_Comm(bms_num);
                // 多次timeout 或者 invalid data，已经判了超时
                if (status == IsFault) {
                    LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                    close(socket_Bank[bms_num]);
                    Task_Is_Over = 1;
                    break;
                } else if (status == IsWarn) { // 超时之前就做4次重选
                    LOG_INFO("BMS-%d: close the BMS socket, time warn happen", bms_num);
                    close(socket_Bank[bms_num]);
                    usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改
                    Task_Is_Over = 1;
                    break;
                }  
            usleep(1000);
        }
 
            if (Task_Is_Over == 1) {
                break;
            }
        }
    }
    close(socket_Bank[bms_num]);
    return;
}
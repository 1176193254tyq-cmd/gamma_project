
#define _POSIX_C_SOURCE 200809L
#include "main.h"

#include <pthread.h>
#include "heart_beat.h"
#include <time.h>

extern int SubNumGlobe;
void* Bank_G2pro_Read_Task(const char *arg);

void* Rank1_6_G2pro_Read_Task(const char *arg);

void* Rank7_12_G2pro_Read_Task(const char *arg);

void* Bank_G2pro_HB_Task(const char *arg);
/**
 * @brief 处理BmsBank返回数据
 * @param[in] ptr-接收到的缓冲区指针
 * @param[in] num-BMS序号，0代表第一台
 */
static void Bank_G2pro_DataProcess(unsigned char *ptr, int num)
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
    if (pbuf[7] == 0x04)
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
       if ((temp.D16 >=0x1001) && (temp.D16 <=0x3000))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=G2pro_INPUT_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1
                if(address==28000+200*num+37)//BAU输出干接点状态特殊处理，数值取反
                {
                    if(RegVal.D16==1)
                    {
                        SET_INPUT(address, 0);
                    }
                    else if(RegVal.D16==0)
                    {
                        SET_INPUT(address, 1);
                    }
                  
                }
                else
                {
                    SET_INPUT(address, RegVal.D16);
                }
                
                //LOG_INFO("协能：address is %d,RegVal.D16 is %d,temp.D16 is %d",address,RegVal.D16,temp.D16+i);
            } 
        }
        else if ((temp.D16 >= 0x600) && (temp.D16 <= 0x6A3))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=G2pro_INPUT_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1

                SET_INPUT(address, RegVal.D16);
           // LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,address);
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
    else if (pbuf[7] == 0x03)
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
        if ((temp.D16 >= 0x3001) && (temp.D16 < 0x6000))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=G2pro_HOLD_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1

                SET_HOLD(address, RegVal.D16);
              //LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,address);
            } 
        }
        else if ((temp.D16 >= 0x600) && (temp.D16 <= 0x72F))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=G2pro_INPUT_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1

                SET_INPUT(address, RegVal.D16);
         // LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,address);
            } 
        }
        if ((temp.D16 >=0x1001) && (temp.D16 <=0x3000))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=G2pro_INPUT_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1
                SET_INPUT(address, RegVal.D16);
               // LOG_INFO("协能：address is %d,RegVal.D16 is %d,temp.D16 is %d",address,RegVal.D16,temp.D16+i);
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
        RegVal.D8[1] = pbuf[10];     //（高字节在前、低字节在后）
        RegVal.D8[0] = pbuf[11]; 
        startaddr = temp.D16; // 寄存器地址
        if (((SUB_EVENT_CMD_STOP + 100) == Get_Out_Sub(num/SubNumGlobe,num%SubNumGlobe)) && (BMS_G2proRELAY_CTR_addr== startaddr))
        {
            Success_Out_Sub(num/SubNumGlobe,num%SubNumGlobe);
            LOG_INFO("RegVal.D16 is %d",RegVal.D16);
            SET_HOLD(35001+300*(num),RegVal.D16);
        }
        else if (((SUB_EVENT_CMD_START + 100) == Get_Out_Sub(num/SubNumGlobe,num%SubNumGlobe)) && (BMS_G2proRELAY_CTR_addr== startaddr))
        {
            Success_Out_Sub(num/SubNumGlobe,num%SubNumGlobe);
            SET_HOLD(35001+300*(num),RegVal.D16);
        }
        else if(((SUB_EVENT_CMD_RESET + 100) == Get_Out_Sub(num/SubNumGlobe,num%SubNumGlobe)) && (BMS_G2pro_RELAY_RESET_addr== startaddr))
        {
            Success_Out_Sub(num/SubNumGlobe,num%SubNumGlobe);
            SET_HOLD(35011+300*(num),RegVal.D16);
        }
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
/**
 * @brief 处理Bms簇返回数据
 * @param[in] ptr-接收到的缓冲区指针
 * @param[in] num-BMS序号，0代表第一台
 */
static void G2pro_rack_DataProcess(unsigned char *ptr, int num)
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
    INT8U Rack_id;
    int rackaddress = 0;
    int celladdress=0;
    bool is_ok = 0;
    int address = 0;
    sysPara *sys_cfg = SysConf_GetInfo();
    memcpy(pbuf, ptr, Rack_XIE_BUFF_LEN);
    Rack_id = pbuf[6];
    // 簇数据 读取寄存器返回
    if ((pbuf[7] == 0x03) && (Rack_id < Rack_Add_Max)&&(Rack_id >= Rack_Add_Min))
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
        else if ((temp.D16 >= 0x0001) && (temp.D16 <= 0x0B00))
        {
     
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 

                rackaddress=G2pro_INPUT_R_INDEX(num+1,Rack_id-1,temp.D16+i);
                SET_INPUT(rackaddress, RegVal.D16);//从BMS返回的Rack_id的起始值为2
            //LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,rackaddress);
            } 
        }
        else if ((temp.D16 >= 0x1000)&& (temp.D16 < 0x6000))
        {
     
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {


                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                celladdress=G2pro_INPUT_C_INDEX(Rack_id-1,temp.D16+i);//从BMS返回的Rack_id的起始值为2
                SET_C_INPUT(num,celladdress, RegVal.D16);
           //LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,celladdress);
            } 
        }


        else
        {
            is_ok = false;
        }
        if(is_ok == true)
        {
            // Rack_timeout_cnt[num][Rack_id-1] = 0;
           
        }
    }
    else if ((pbuf[7] == 0x04) && (Rack_id < Rack_Add_Max)&&(Rack_id >= Rack_Add_Min))
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
    }
}


void  BMS_G2pro_Task(const char *_num)
{
    //初始化
    const char *bms_num = &(*_num);
    sysPara* sys_cfg = SysConf_GetInfo();
    int recv_bytes = 0;

    struct sockaddr_in server_addr = {0};
    int bms_socket = 0;

    INT8U connect_cnt=0;
    
    LOG_INFO("G2pro BMS-%d", *bms_num);

    // 设置线程属性
    pthread_attr_t thread_attrR;
    pthread_attr_init(&thread_attrR);
    pthread_attr_setdetachstate(&thread_attrR, PTHREAD_CREATE_DETACHED);

    // 创建不同的线程 ID
    pthread_t thread_bank, thread_rank1_6, thread_rank7_12,thread_hb_bank;
   // 创建线程

    pthread_create(&thread_bank, &thread_attrR, Bank_G2pro_Read_Task, (void*)bms_num);

    pthread_create(&thread_rank1_6, &thread_attrR, Rank1_6_G2pro_Read_Task, (void*)bms_num);

   pthread_create(&thread_rank7_12, &thread_attrR, Rank7_12_G2pro_Read_Task, (void*)bms_num);
   // pthread_create(&thread_hb_bank, &thread_attrR, Bank_G2pro_HB_Task, (void*)bms_num);

    while(1)
    {
    usleep(500*1000);

    }

}

//读取BMS的BANK信息
void* Bank_G2pro_Read_Task(const char *arg) {
    

    sysPara *sys_cfg = SysConf_GetInfo();

    int bms_num = *((int *)arg);  // 获取值
    LOG_INFO("Bank task started for BMS-%d", bms_num);
    INT8U loop = 0;
    INT8U Bank_id = 1;
    INT8U Bank_Env_id = 0x7F;
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i, j;
    char str_char[2] = "";
    INT8U timeout_cnt=0;
    INT8U Task_Is_Over = 0;
    INT8U status;
    int socket_Bank = 0;
    u16_conv RegVal; 
    BmsWriteReq req;
    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (400 * 1000)};
        // 创建本地客户端socket
        if ((socket_Bank = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("BMS-%d Create read socket failure! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
            sleep(5);
            if(GET_INPUT(P2P_mode)==1)
            { 
                Set_BMS_Comm(bms_num, IsNoFault, TRUE); //对点模式 
            }
            continue;
        }
        // BMS通讯正常
        LOG_INFO("BMS-%d: Read Connect SUCCESS! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
        // 原先状态为警告态不做定时器复位
        if (IsWarn == Get_BMS_Comm(bms_num)) {
            Set_BMS_Comm(bms_num, IsNoFault, FALSE);
        } else {
            Set_BMS_Comm(bms_num, IsNoFault, TRUE);
        }
        int read_recv_res = 0;
        timeout_cnt=0;
        while (1) {
          if(GET_INPUT(P2P_mode)==0)
          { 
            switch (loop) {
                case 0:
                    if(SUB_EVENT_CMD_STOP==Get_Out_Sub(bms_num/SubNumGlobe,bms_num%SubNumGlobe))
                    {
                        Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id, BMS_G2proRELAY_CTR_addr, BMS_XIENENG_RELAY_CLOSE, CMD_DELAY_200);
                        Wait_Out_Sub(bms_num/SubNumGlobe,bms_num%SubNumGlobe);                  
                    }
                    else if(SUB_EVENT_CMD_START==Get_Out_Sub(bms_num/SubNumGlobe,bms_num%SubNumGlobe))
                    {
                        Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id, BMS_G2proRELAY_CTR_addr, BMS_XIENENG_RELAY_OPEN, CMD_DELAY_200);
                        Wait_Out_Sub(bms_num/SubNumGlobe,bms_num%SubNumGlobe);
                    }
                    else if(SUB_EVENT_CMD_RESET==Get_Out_Sub(bms_num/SubNumGlobe,bms_num%SubNumGlobe))
                    {
                        Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id, BMS_G2pro_RELAY_RESET_addr, 1, CMD_DELAY_200);
                        Wait_Out_Sub(bms_num/SubNumGlobe,bms_num%SubNumGlobe);
                    }
                     while (bms_write_try_dequeue(bms_num, &req)) {
                    INT16U addr  = req.addr;
                    INT16U value = req.value;
                    /* 常规：单寄存器 0x06 */
                    INT16U idx = G2pro_Addr_From_Index(bms_num+1, addr);
                    Modbus_TCP_Write06_SingleRegist(socket_Bank, PCS_Taida_SLAVE_ADDR, idx, value, CMD_DELAY_200);
                     LOG_INFO("BMS[%d] 0x06 write ok: hold[%u->idx %u] = %u", bms_num, addr, idx, value);
                 }  
                    loop++;
                    break;
                case 1:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr0 , G2PROBank_block_size0, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 2:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr1 , G2PROBank_block_size1, CMD_DELAY_200);
                   Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                        
                case 3:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr2 , G2PROBank_block_size2, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 4:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr3 , G2PROBank_block_size3, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;

                case 5:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr4 , G2PROBank_block_size4, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 6:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr5, G2PROBank_block_size5, CMD_DELAY_200);
                     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;

                case 7:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr6, G2PROBank_block_size6, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 8:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr7 , G2PROBank_block_size7, CMD_DELAY_200);
                     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 9:
                   Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                // Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr8 , G2PROBank_block_size8, CMD_DELAY_200);
                    loop++;
                    break;   
                case 10:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr9, G2PROBank_block_size9, CMD_DELAY_200);
                     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 11:
                    Modbus_TCP_Read(socket_Bank, Bank_Env_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr10 , G2PROBank_block_size10, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 12:
                    // Modbus_TCP_Read(socket_Bank, Bank_Env_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr11 , G2PROBank_block_size11, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;  
                 case 13:
                    // Modbus_TCP_Read(socket_Bank, Bank_Env_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr12 , G2PROBank_block_size12, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 14:
                    Modbus_TCP_Read(socket_Bank, Bank_Env_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr13 , G2PROBank_block_size13, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;  
                case 15:
                    Modbus_TCP_Read(socket_Bank, Bank_Env_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr14 , G2PROBank_block_size14, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                case 16:
                    Modbus_TCP_Read(socket_Bank, Bank_Env_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr15 , G2PROBank_block_size15, CMD_DELAY_200);
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;  
                case 17:
                
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, G2PROBank_block_addr23 , G2PROBank_block_size23, CMD_DELAY_200);
                    loop++;
                    break;
                       

                default:
                    loop = 0;
                    break;
            }

            // 接收BMS的返回数据
            read_recv_res = Recv_Modbus_Back(socket_Bank, bms_num, Rack_XIE_BUFF_LEN, Bank_G2pro_DataProcess);
            // BMS通讯异常
            if (read_recv_res < 0) {
                LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                close(socket_Bank);
                break;
            }
            else if (read_recv_res == 0) {

                    timeout_cnt++;
                   if (timeout_cnt >= 75) {   
                     LOG_INFO("BMS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                     bms_num, timeout_cnt);
                    close(socket_Bank);
               
                    break;
            }

            }
            // BMS通讯正常
            else if (read_recv_res > 0) {
                timeout_cnt=0;
                Set_BMS_Comm(bms_num, IsNoFault, TRUE);
            } else {
                status = Get_BMS_Comm(bms_num);
                // 多次timeout 或者 invalid data，已经判了超时
                if (status == IsFault) {
                    LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                    close(socket_Bank);
                 
                    break;
                } else if (status == IsWarn) { // 超时之前就做4次重选
                    LOG_INFO("BMS-%d: close the BMS socket, time warn happen", bms_num);
                    close(socket_Bank);
                    usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改
               
                    break;
                }
            }
            usleep(20*1000);
          }
            else
            {
                  Set_BMS_Comm(bms_num, IsNoFault, TRUE);  //进入对点模式
                  usleep(50 * 1000);
            }
        }
    }
    close(socket_Bank);
    return;
}
void* Rank1_6_G2pro_Read_Task(const char *arg) {
    sysPara *sys_cfg = SysConf_GetInfo();

    // 确保正确的类型转换和内存释放
    int bms_num = (*arg);  // 获取值

  LOG_INFO("Rank1_6_G2pro_Read_Task for BMS-%d", bms_num);
    INT8U loop = 0;
    INT8U Rack_id;
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i, j;
    int socket_Rack1_6 = 0;
    char str_char[2] = "";
    INT8U timeout_cnt=0;
    INT8U Task_Is_Over = 0;
    INT8U status;
    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (400 * 1000)};
        // 创建本地客户端socket
        if ((socket_Rack1_6 = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("BMS-%d Create read socket failure! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
            sleep(5);
            continue;
        }
        // BMS通讯正常
        LOG_INFO("BMS-%d: Read Connect SUCCESS! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
        // 原先状态为警告态不做定时器复位
        if (IsWarn == Get_BMS_Comm(bms_num)) {
            Set_BMS_Comm(bms_num, IsNoFault, FALSE);
        } else {
            Set_BMS_Comm(bms_num, IsNoFault, TRUE);
        }
        int read_recv_res = 0;

        Task_Is_Over = 0;

        timeout_cnt=0;
        while (1) {
          if(GET_INPUT(P2P_mode)==0)
          {  
            loop = 0;
            Rack_id = Rack_Add_Min - 1;
            while ((Rack_id < Rack_Add_Max) && (Rack_id < sys_cfg->bms_cluster[bms_num] + 2)) {
                #ifdef ISDEBUG_
                LOG_INFO("BMS-%d: Rack_id==%d", bms_num, Rack_id);
                #endif

                switch (loop) {
                    case 0:
                        Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr0, G2PRORack_block_size0, CMD_DELAY_200);
                        loop++;
                        break;
                    case 1:
                      //  Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr1, G2PRORack_block_size1, CMD_DELAY_200);
                        loop++;
                        break;
                    case 2:
                      //  Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr2, G2PRORack_block_size2, CMD_DELAY_200);
                        loop++;
                        break;
                    case 3:
                       // Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr3, G2PRORack_block_size3, CMD_DELAY_200);
                        loop++;
                        break;
                    case 4:
                      //  Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr4, G2PRORack_block_size4, CMD_DELAY_200);
                        loop++;
                        break;
                    case 5:
                       //     Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr5, G2PRORack_block_size5, CMD_DELAY_200);
                      
                        loop++;
                        break;
                    case 6:
                     
                        //    Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr6, G2PRORack_block_size6, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 7:
                     
                         //   Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr7, G2PRORack_block_size7, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 8:
                     
                      
                         //   Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr8, G2PRORack_block_size8, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 9:
                 
                        //     Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr9, G2PRORack_block_size9, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 10:
                       
                          //  Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr10, G2PRORack_block_size10, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 11:
                     
                         //   Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr11, G2PRORack_block_size11, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    case 12:
                     
                          //  Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr12, G2PRORack_block_size12, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    case 13:
                     
                         //   Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr13, G2PRORack_block_size13, CMD_DELAY_200);
 
                        loop++;
              
                        break;
                    case 14:
                     
                            Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr14, G2PRORack_block_size14, CMD_DELAY_200);

                        loop++;
                        break;
                    case 15:
                     
                            Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr15, G2PRORack_block_size15, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    // case 16:
                     
                    //         Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr16, G2PRORack_block_size16, CMD_DELAY_200);
                     
                    //     loop++;
                    //     break;
                    // case 17:
                     
                    //         Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr17, G2PRORack_block_size17, CMD_DELAY_200);
 
                    //    loop++;
              
                    // break;
                    // case 18:
                     
                    //         Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr18, G2PRORack_block_size18, CMD_DELAY_200);

                    //   loop++;
                    // break;

                    // case 19:
                     
                    //         Modbus_TCP_Read(socket_Rack1_6, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr19, G2PRORack_block_size19, CMD_DELAY_200);

                    //   loop++;
                    // break;


                    default:
                        loop = 0;
                        break;
                }
                if (is_jump == 1) {
                    if (loop == 0) {
                        ++Rack_id;
                    }
                    if (Rack_id >6) { // 只读取6簇数据
                        Rack_id = Rack_Add_Min - 1;
                    }
                    is_jump = 0;
                    continue;
                }
                // 接收BMS的返回数据
                read_recv_res = Recv_Modbus_Back(socket_Rack1_6, bms_num, Rack_XIE_BUFF_LEN, G2pro_rack_DataProcess);
                // BMS通讯异常
                if (read_recv_res < 0) {
                    LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                    close(socket_Rack1_6);
                    Task_Is_Over = 1;
                    break;
                }
                // BMS通讯正常
                else if (read_recv_res > 0) {
                      timeout_cnt=0;
                    Set_BMS_Comm(bms_num, IsNoFault, TRUE);
                } 
                else if (read_recv_res == 0) {

                    timeout_cnt++;
                   if (timeout_cnt >= 75) {   
                     LOG_INFO("BMS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                     bms_num, timeout_cnt);
                    //  Set_PCS_Comm(pcs_num, IsNoFault, FALSE);

                    close(socket_Rack1_6);
               
                    break;
                    }

                }
                else {
                    status = Get_BMS_Comm(bms_num);
                    // 多次timeout 或者 invalid data，已经判了超时
                    if (status == IsFault) {
                        LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                        close(socket_Rack1_6);
                        Task_Is_Over = 1;
                        break;
                    } else if (status == IsWarn) { // 超时之前就做4次重选
                        LOG_INFO("BMS-%d: close the BMS socket, time warn happen", bms_num);
                        close(socket_Rack1_6);
                        usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改
                        Task_Is_Over = 1;
                        break;
                    }
                }
                usleep(1000);

                if (loop == 0) {
                    ++Rack_id;
                }
                if (Rack_id >7) {
                    Rack_id = Rack_Add_Min - 1;
                }
            }
            if (Task_Is_Over == 1) {
                break;
            }
        }
        else
        {
                Set_BMS_Comm(bms_num, IsNoFault, TRUE);  //进入对点模式
                usleep(50 * 1000);


        }
        }
    }
    close(socket_Rack1_6);
    return;
}
//读取BMS的6~10簇信息
void* Rank7_12_G2pro_Read_Task(const char *arg) {
  sysPara *sys_cfg = SysConf_GetInfo();
    int bms_num = *((int *)arg);  // 获取值

    
    LOG_INFO("Rank7_12_XIE_Read_Task for BMS-%d", bms_num);
  
    INT8U loop = 0;
    INT8U Rack_id;
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i,j;
    int socket_Rack7_12 = 0;
    char str_char[2] = "";

    INT8U Task_Is_Over = 0;
    INT8U status;

    while (1)
    {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (400 * 1000)};
        // 创建本地客户端socket
        if ((socket_Rack7_12 = Create_Client_Socket(server_addr, time_out)) == -1)
        {
            LOG_INFO("BMS-%d Create read socket failure! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
            sleep(5);
            continue;
        }
        // BMS通讯正常
        LOG_INFO("BMS-%d: Read Connect SUCCESS! ip:%s[port:%d]", bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
        // 原先状态为警告态不做定时器复位
        if (IsWarn == Get_BMS_Comm(bms_num))
        {
            Set_BMS_Comm(bms_num, IsNoFault, FALSE);
        }
        else
        {
            Set_BMS_Comm(bms_num, IsNoFault, TRUE);
        }
        int read_recv_res = 0;
    
        Task_Is_Over = 0;


       while (1) {
          if(GET_INPUT(P2P_mode)==0)
          {  
            loop = 0;
            Rack_id = 6;
            while (Rack_id < Rack_Add_Max) {
                #ifdef ISDEBUG_
                LOG_INFO("BMS-%d: Rack_id==%d", bms_num, Rack_id);
                #endif

                switch (loop) {
                    case 0:
                        Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr0, G2PRORack_block_size0, CMD_DELAY_200);
                        loop++;
                        break;
                    case 1:
                      //  Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr1, G2PRORack_block_size1, CMD_DELAY_200);
                        loop++;
                        break;
                    case 2:
                       // Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr2, G2PRORack_block_size2, CMD_DELAY_200);
                        loop++;
                        break;
                    case 3:
                      //  Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr3, G2PRORack_block_size3, CMD_DELAY_200);
                        loop++;
                        break;
                    case 4:
                      //  Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr4, G2PRORack_block_size4, CMD_DELAY_200);
                        loop++;
                        break;
                    case 5:
                        //    Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr5, G2PRORack_block_size5, CMD_DELAY_200);
                      
                        loop++;
                        break;
                    case 6:
                     
                       //     Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr6, G2PRORack_block_size6, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 7:
                     
                         //   Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr7, G2PRORack_block_size7, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 8:
                     
                      
                        //    Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr8, G2PRORack_block_size8, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 9:
                 
                         //    Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr9, G2PRORack_block_size9, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 10:
                       
                        //    Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr10, G2PRORack_block_size10, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 11:
                     
                          //  Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr11, G2PRORack_block_size11, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    case 12:
                     
                         //   Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr12, G2PRORack_block_size12, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    case 13:
                     
                         //   Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr13, G2PRORack_block_size13, CMD_DELAY_200);
 
                        loop++;
              
                        break;
                    case 14:
                     
                            Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr14, G2PRORack_block_size14, CMD_DELAY_200);

                        loop++;
                        break;
                    case 15:
                     
                            Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr15, G2PRORack_block_size15, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    // case 16:
                     
                    //         Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr16, G2PRORack_block_size16, CMD_DELAY_200);
                     
                    //     loop++;
                    //     break;
                    // case 17:
                     
                    //         Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr17, G2PRORack_block_size17, CMD_DELAY_200);
 
                    //    loop++;
              
                    // break;
                    // case 18:
                     
                    //         Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr18, G2PRORack_block_size18, CMD_DELAY_200);

                    //   loop++;
                    // break;

                    // case 19:
                     
                    //         Modbus_TCP_Read(socket_Rack7_12, Rack_id+1, MODBUS_READ_TYPE_03, G2PRORack_block_addr19, G2PRORack_block_size19, CMD_DELAY_200);

                    //   loop++;
                    // break;


                    default:
                        loop = 0;
                        break;
                }
                 if(is_jump == 1)
                    {
                        if(loop==0)
                        {
                            ++Rack_id;
                        }
                        if(Rack_id>13)//读取7～12簇
                        {
                            Rack_id = 7;
                        }
                        is_jump = 0;
                        continue; 
                    }
                 // 接收BMS的返回数据
                 read_recv_res = Recv_Modbus_Back(socket_Rack7_12, bms_num, Rack_XIE_BUFF_LEN, G2pro_rack_DataProcess);
                // BMS通讯异常
    
    
                if (read_recv_res < 0)
                {
                    LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                    close(socket_Rack7_12);

                    Task_Is_Over = 1;
                    break;
                }

                
                // BMS通讯正常
                else if (read_recv_res > 0)
                {
                    Set_BMS_Comm(bms_num, IsNoFault, TRUE);

                }
                else
                {
                    status = Get_BMS_Comm(bms_num);
                    // 多次timeout 或者 invalid data，已经判了超时
                    if (status == IsFault)
                    {
                        LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                        close(socket_Rack7_12);

                        Task_Is_Over = 1;
                        break;
                    }
                    else if (status == IsWarn) // 超时之前就做4次重选
                    {
                        LOG_INFO("BMS-%d: close the BMS socket, time warn happen", bms_num);
                        close(socket_Rack7_12);
                        usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改
                        Task_Is_Over = 1;
                        break;
                    }
                }
                usleep(1000);
    
                if(loop==0)
                {
                    ++Rack_id;
                }
                if(Rack_id>12)
                {
                    Rack_id = 6;
                }
                
            }
            if (Task_Is_Over == 1)
            {
                break;
            }
        }
        else
        {
                Set_BMS_Comm(bms_num, IsNoFault, TRUE);  //进入对点模式
                  usleep(50 * 1000);

        }
    }
close(socket_Rack7_12);
return;
}
}
/*BMS心跳包发送*/
void* Bank_G2pro_HB_Task(const char *arg)
{
    sysPara *sys_cfg = SysConf_GetInfo();
    int bms_num = *((int *)arg);

    LOG_INFO("HB task started for BMS-%d", bms_num);

    HBHandle* hb = hb_create(bms_num, 1000, BMS_G2pro_Heartbit_addr);
    if (!hb){
        LOG_INFO("BMS-%d: hb_create failed", bms_num);
        return NULL;
    }

    int socket_Bank = -1;
    INT8U Bank_id = 1;
    bool hb_started = false;
    int backoff_sec = 1;

    for (;;){  // === 外层：断线重连循环 ===
        // —— 建链（带退避）——
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);

        struct timeval time_out = {0, (200 * 1000)};
        socket_Bank = Create_Client_Socket(server_addr, time_out);
        if (socket_Bank == -1){
            LOG_INFO("BMS-%d Create HB socket failure! ip:%s[port:%d]",
                     bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);
            if (hb_started) hb_pause(hb);      // 没链路：暂停心跳
            sleep(backoff_sec);
            if (backoff_sec < 5) backoff_sec++;
            continue;                           // 继续尝试连接
        }
        backoff_sec = 1; // 成功后重置退避

        LOG_INFO("BMS-%d: HB Connect SUCCESS! ip:%s[port:%d]",
                 bms_num, sys_cfg->bms_ip[bms_num], sys_cfg->bms_port[bms_num]);

        // 清告警/置状态
        if (IsWarn == Get_BMS_Comm(bms_num)){
            Set_BMS_Comm(bms_num, IsNoFault, FALSE);
        } else {
            Set_BMS_Comm(bms_num, IsNoFault, TRUE);
        }

       
        if (!hb_started) {
            if (hb_start(hb, socket_Bank, Bank_id,hb_thread) != 0){
                LOG_INFO("BMS-%d: hb_start failed", bms_num);
                close(socket_Bank);
                break; // 或者改为 continue 重试
            }
            hb_started = true;
        } else {
         hb_update_channel(hb, socket_Bank, Bank_id);
        }
     

     
        for (;;){

            // bool pcscomm_status = (Get_PCS_Comm(0) == 1) || (Get_PCS_Comm(2) == 1);

            // if (pcscomm_status){
            //     LOG_WARN("BMS-%d: comm broken, pause HB ", bms_num);
            //     hb_pause(hb);            // 暂停心跳（不中断线程）              
            // }
            // else 
            // {
            //      hb_resume(hb);//保持运行

            // }
            // bool  bmscommstatus = Get_BMS_Comm(bms_num);
            //         // 多次timeout 或者 invalid data，已经判了超时
            // if (bmscommstatus == IsFault)
            //     {
            //    LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
            //     close(socket_Bank);
            //       break;
            //     }

            usleep(1000 * 1000); // 1s 轮询
        }

    }


    if (hb_started) hb_stop(hb);
    hb_destroy(hb);
    return NULL;
}

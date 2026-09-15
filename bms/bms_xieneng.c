
#define _POSIX_C_SOURCE 200809L
#include "main.h"
#include "bms_xieneng.h"
#include <pthread.h>
#include "heart_beat.h"
#include <time.h>
static volatile INT8U MS_CMD_REPEAT = 60;   
static char allstr[MAX_BMS_NUM][Rack_XIE_BUFF_LEN * 3 + 15];
INT8U volatile Bms_Contactor_Lock[MAX_BMS_NUM] = {0};
static unsigned char Recv_Buf_Catch[MAX_BMS_NUM][(Rack_XIE_BUFF_LEN + 15) * 5];
static unsigned int Recv_Len_Catch[MAX_BMS_NUM][5];
                                //命令尝试次数上限
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER; //互斥锁
//ini中获取的片数据的大小
int Ini_Rack_Blank_Size[MAX_BMS_NUM][14] = {0};

void* Bank_XIE_Read_Task(const char *arg);

void* Rank0_5_XIE_Read_Task(const char *arg);

void* Rank6_10_XIE_Read_Task(const char *arg) ;
void* Bank_XIE_HB_Task(const char *arg);
/**
 * @brief 处理BmsBank返回数据
 * @param[in] ptr-接收到的缓冲区指针
 * @param[in] num-BMS序号，0代表第一台
 */
static void Bank_XIE_DataProcess(unsigned char *ptr, int num)
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
       if ((temp.D16 >= Bank_Block_Addr(0)) && (temp.D16 < (0x40D0 + 3)))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=INPUT_B_test_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1
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
        if ((temp.D16 >= 0) && (temp.D16 < 100000))
        {

         
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {

                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                address=HOLD_B_INDEX(num+1,temp.D16+i);//程序中的num是从0开始的，所以+1

                SET_HOLD(address, RegVal.D16);
              //LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,address);
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
        if (((SUB_EVENT_CMD_STOP + 100) == Get_Out_Sub(num/2,num%2)) && (BMS_XIENENG_RELAY_CTR_addr== startaddr))
        {
            Success_Out_Sub(num/2,num%2);
            LOG_INFO("RegVal.D16 is %d",RegVal.D16);
            SET_HOLD(35001+300*(num),RegVal.D16);
        }
        else if (((SUB_EVENT_CMD_START + 100) == Get_Out_Sub(num/2,num%2)) && (BMS_XIENENG_RELAY_CTR_addr== startaddr))
        {
            Success_Out_Sub(num/2,num%2);
            SET_HOLD(35001+300*(num),RegVal.D16);
        }
        else if(((SUB_EVENT_CMD_RESET + 100) == Get_Out_Sub(num/2,num%2)) && (BMS_XIENENG_RELAY_RESET_addr== startaddr))
        {
            Success_Out_Sub(num/2,num%2);
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
static void XIE_DataProcess(unsigned char *ptr, int num)
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
    if ((pbuf[7] == 0x04) && (Rack_id < Rack_Add_Max)&&(Rack_id >= Rack_Add_Min))
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
        else if ((temp.D16 >= 8193) && (temp.D16 < 8239)||(temp.D16)==12812)
        {
     
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 

                rackaddress=INPUT_R_INDEX(num+1,Rack_id-1,temp.D16+i);
                SET_INPUT(rackaddress, RegVal.D16);//从BMS返回的Rack_id的起始值为2
            //LOG_INFO("协能：temp.D16 is %d,RegVal.D16 is %d,index is %d",temp.D16,RegVal.D16,rackaddress);
            } 
        }
        else if ((temp.D16 >= 8736)&& (temp.D16 < 10240))
        {
     
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < Rack_XIE_BUFF_LEN); i++)           
            {


                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                celladdress=INPUT_C_INDEX(Rack_id-1,temp.D16+i);//从BMS返回的Rack_id的起始值为2
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


void  BMS_XieNeng_Task(const char *_num)
{
    //初始化
    const char *bms_num = &(*_num);

    sysPara* sys_cfg = SysConf_GetInfo();
    int recv_bytes = 0;

    struct sockaddr_in server_addr = {0};
    int bms_socket = 0;

    INT8U connect_cnt=0;
    
    LOG_INFO("协能BMS-%d", *bms_num);
  
 

    // 设置线程属性
    pthread_attr_t thread_attrR;
    pthread_attr_init(&thread_attrR);
    pthread_attr_setdetachstate(&thread_attrR, PTHREAD_CREATE_DETACHED);

    // 创建不同的线程 ID
    pthread_t thread_bank, thread_rank0_5, thread_rank6_10,thread_hb_bank;
   // 创建线程
    // int *arg1;
    // *arg1 = *bms_num;
    pthread_create(&thread_bank, &thread_attrR, Bank_XIE_Read_Task, (void*)bms_num);

    // int *arg2 ;
    // *arg2 = *bms_num;
    // pthread_create(&thread_rank0_5, &thread_attrR, Rank0_5_XIE_Read_Task, (void*)bms_num);

    // // int *arg3 ;
    // // *arg3 = *bms_num;
    // pthread_create(&thread_rank6_10, &thread_attrR, Rank6_10_XIE_Read_Task, (void*)bms_num);
   //pthread_create(&thread_hb_bank, &thread_attrR, Bank_XIE_HB_Task, (void*)bms_num);

    while(1)
    {
    usleep(50*1000);

    }

}

//读取BMS的BANK信息
void* Bank_XIE_Read_Task(const char *arg) {
    

    sysPara *sys_cfg = SysConf_GetInfo();

    int bms_num = *((int *)arg);  // 获取值

    INT8U hb;
    LOG_INFO("Bank task started for BMS-%d", bms_num);
    INT8U loop = 0;
    INT8U Bank_id = 1;
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i, j;
    char str_char[2] = "";
    INT8U timeout_cnt=0;

    Bms_Contactor_Lock[bms_num] = OFF;


    INT8U Task_Is_Over = 0;
    INT8U status;
    int socket_Bank = 0;
    u16_conv RegVal; 

    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (200 * 1000)};
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
        // EMS_HB_start(socket_Bank,Bank_id,BMS_XIENENG_Heartbit_addr);//往BMS发送心跳
         timeout_cnt=0;

        while (1) {
          if(GET_INPUT(P2P_mode)==0)
          { 
            switch (loop) {
                case 0:


                    if(SUB_EVENT_CMD_STOP==Get_Out_Sub(bms_num/2,bms_num%2))
                    {
                        Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id, BMS_XIENENG_RELAY_CTR_addr, BMS_XIENENG_RELAY_CLOSE, CMD_DELAY_200);
                        Wait_Out_Sub(bms_num/2,bms_num%2);
                   
                    }
                    else if(SUB_EVENT_CMD_START==Get_Out_Sub(bms_num/2,bms_num%2))
                    {
                        Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id, BMS_XIENENG_RELAY_CTR_addr, BMS_XIENENG_RELAY_OPEN, CMD_DELAY_200);
                        Wait_Out_Sub(bms_num/2,bms_num%2);
                    }
                    else if(SUB_EVENT_CMD_RESET==Get_Out_Sub(bms_num/2,bms_num%2))
                    {
                        Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id, BMS_XIENENG_RELAY_RESET_addr, 1, CMD_DELAY_200);
                        Wait_Out_Sub(bms_num/2,bms_num%2);
                    }
                    loop++;
                    break;
                case 1:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(0) , Bank_Block_Size(0), CMD_DELAY_200);
                    loop++;
                    break;
                case 2:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(1) , Bank_Block_Size(1), CMD_DELAY_200);
                    loop++;
                    break;

                case 3:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(2) , Bank_Block_Size(2), CMD_DELAY_200);
                    loop++;
                    break;
                case 4:
                    Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(3) , Bank_Block_Size(3), CMD_DELAY_200);
                    loop++;
                    break;

                case 5:
            //     if((bms_num==0)||(bms_num==1)||(bms_num==2))
            //    {     //Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(4) , Bank_Block_Size(4), CMD_DELAY_200);
              if (socket_Bank > 0){
                Modbus_COM_Write06_SingleRegist(socket_Bank, Bank_id,
                                            BMS_XIENENG_Heartbit_addr, (INT16U)hb,
                                            /*CMD_DELAY_200*/ 200);
                        hb++; // 0..255
                     }
                    // LOG_INFO("发心跳BMS：%d",bms_num);
                //}
                    loop++;
                    break;
                // case 6:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(5) , Bank_Block_Size(5), CMD_DELAY_200);
                //     loop++;
                //     break;

                // case 7:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(6) , Bank_Block_Size(6), CMD_DELAY_200);
                //     loop++;
                //     break;
                // case 8:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(7) , Bank_Block_Size(7), CMD_DELAY_200);
                //     loop++;
                //     break;
                // case 9:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(8) , Bank_Block_Size(8), CMD_DELAY_200);
                //     loop++;
                //     break;
                // case 10:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(9) , Bank_Block_Size(9), CMD_DELAY_200);
                //     loop++;
                //     break;

                // case 11:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(10) , Bank_Block_Size(10), CMD_DELAY_200);
                //     loop++;
                //     break;
                // // case 12:
                // //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_04, Bank_Block_Addr(11) , Bank_Block_Size(11), CMD_DELAY_200);
                // //     loop++;
                // //     break;


                // case 12:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr0 , Bank_block_hold_size0, CMD_DELAY_200);
                //     loop++;
                //     break;
                // case 13:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr1 , Bank_block_hold_size1, CMD_DELAY_200);
                //     loop++;
                //     break;
                // case 14:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03, Bank_block_hold_addr2, Bank_block_hold_size2, CMD_DELAY_200);
                //     loop++;
                //     break;

                // case 15:
                //     Modbus_TCP_Read(socket_Bank, Bank_id, MODBUS_READ_TYPE_03,Bank_block_hold_addr3, Bank_block_hold_size3, CMD_DELAY_200);
                //     loop++;
                //     break;



                default:
                    loop = 0;
                    break;
            }

            // 接收BMS的返回数据
            read_recv_res = Recv_Modbus_Back(socket_Bank, bms_num, Rack_XIE_BUFF_LEN, Bank_XIE_DataProcess);
            // BMS通讯异常
            if (read_recv_res < 0) {
                LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                close(socket_Bank);
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

                    close(socket_Bank);
               
                    break;
            }

            }
            else {
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
            usleep(100*1000);
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
void* Rank0_5_XIE_Read_Task(const char *arg) {
    sysPara *sys_cfg = SysConf_GetInfo();

    // 确保正确的类型转换和内存释放
    int bms_num = (*arg);  // 获取值

  LOG_INFO("Rank0_5_XIE_Read_Task for BMS-%d", bms_num);
    INT8U loop = 0;
    INT8U Rack_id;
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i, j;
    int socket_Rack0_5 = 0;
    char str_char[2] = "";
    Bms_Contactor_Lock[bms_num] = OFF;
    INT8U Task_Is_Over = 0;
    INT8U status;
    INT8U timeout_cnt=0;


    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (200 * 1000)};
        // 创建本地客户端socket
        if ((socket_Rack0_5 = Create_Client_Socket(server_addr, time_out)) == -1) {
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
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr0, Rack_block_size0, CMD_DELAY_200);
                        loop++;
                        break;
                    case 1:
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr1, Rack_block_size1, CMD_DELAY_200);
                        loop++;
                        break;
                    case 2:
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr2, Rack_block_size2, CMD_DELAY_200);
                        loop++;
                        break;
                    case 3:
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr3, Rack_block_size3, CMD_DELAY_200);
                        loop++;
                        break;
                    case 4:
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr4, Rack_block_size4, CMD_DELAY_200);
                        loop++;
                        break;
                    case 5:
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr5, Rack_block_size5, CMD_DELAY_200);
                      
                        loop++;
                        break;
                    case 6:
                     
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr6, Rack_block_size6, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 7:
                     
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr7, Rack_block_size7, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 8:
                     
                      
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr8, Rack_block_size8, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 9:
                 
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr9, Rack_block_size9, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 10:
                       
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr10, Rack_block_size10, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 11:
                     
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr11, Rack_block_size11, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    case 12:
                     
                            Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr12, Rack_block_size12, CMD_DELAY_200);
                     
                        loop++;
                        break;
                    case 13:
                     
                        Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr13, Rack_block_size13, CMD_DELAY_200);
 
                       loop++;
              
                    break;
                    case 14:
                     
                       Modbus_TCP_Read(socket_Rack0_5, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr14, Rack_block_size14, CMD_DELAY_200);

                      loop++;
                    break;
                    default:
                        loop = 0;
                        break;
                }
                if (is_jump == 1) {
                    if (loop == 0) {
                        ++Rack_id;
                    }
                    if (Rack_id >5) { // 只读取5簇数据
                        Rack_id = Rack_Add_Min - 1;
                    }
                    is_jump = 0;
                    continue;
                }
                // 接收BMS的返回数据
                read_recv_res = Recv_Modbus_Back(socket_Rack0_5, bms_num, Rack_XIE_BUFF_LEN, XIE_DataProcess);
                // BMS通讯异常
                if (read_recv_res < 0) {
                    LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                    close(socket_Rack0_5);
                    Task_Is_Over = 1;
                    break;
                }
                // BMS通讯正常
                else if (read_recv_res > 0) {
                    Set_BMS_Comm(bms_num, IsNoFault, TRUE);
                    timeout_cnt=0;
                } 
                else if (read_recv_res == 0) {

                    timeout_cnt++;
                   if (timeout_cnt >= 75) {   
                     LOG_INFO("BMS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                     bms_num, timeout_cnt);
                    //  Set_PCS_Comm(pcs_num, IsNoFault, FALSE);

                    close(socket_Rack0_5);
               
                    break;
                 }

                }
                else {
                    status = Get_BMS_Comm(bms_num);
                    // 多次timeout 或者 invalid data，已经判了超时
                    if (status == IsFault) {
                        LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                        close(socket_Rack0_5);
                        Task_Is_Over = 1;
                        break;
                    } else if (status == IsWarn) { // 超时之前就做4次重选
                        LOG_INFO("BMS-%d: close the BMS socket, time warn happen", bms_num);
                        close(socket_Rack0_5);
                        usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改
                        Task_Is_Over = 1;
                        break;
                    }
                }
                usleep(5*1000);

                if (loop == 0) {
                    ++Rack_id;
                }
                if (Rack_id >5) {
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
    close(socket_Rack0_5);
    return;
}
//读取BMS的6~10簇信息
void* Rank6_10_XIE_Read_Task(const char *arg) {
  sysPara *sys_cfg = SysConf_GetInfo();
    int bms_num = *((int *)arg);  // 获取值

    
    LOG_INFO("Rank6_10_XIE_Read_Task for BMS-%d", bms_num);
  
    INT8U loop = 0;
    INT8U Rack_id;
    INT8U is_jump = 0;
    int read_recv_res = 0;
    int i,j;
    int socket_Rack6_10 = 0;
    char str_char[2] = "";
    Bms_Contactor_Lock[bms_num] = OFF;
    INT8U Task_Is_Over = 0;
    INT8U status;
    INT8U timeout_cnt=0;
    while (1)
    {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(sys_cfg->bms_port[bms_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->bms_ip[bms_num]);
        struct timeval time_out = {0, (200 * 1000)};
        // 创建本地客户端socket
        if ((socket_Rack6_10 = Create_Client_Socket(server_addr, time_out)) == -1)
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
        timeout_cnt=0;

        while (1)
        {
        if(GET_INPUT(P2P_mode)==0)
          {            
            loop = 0;
            Rack_id = 6;
            while ((Rack_id < Rack_Add_Max)&&(Rack_id<sys_cfg->bms_cluster[bms_num]+2))
            {
                #ifdef ISDEBUG_
                LOG_INFO("BMS-%d: Rack_id==%d",bms_num,Rack_id);
                #endif
      
              
                switch (loop)
                    {
                    case 0:
                        Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(0), Rack_block_size0, CMD_DELAY_200);
                        loop++;
                        break;
                    case 1:
                        Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(1), Rack_block_size1, CMD_DELAY_200);
                        loop++;
                        break;
                    case 2:
                        Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(2), Rack_block_size2, CMD_DELAY_200);
                        loop++;
                        break;
                    case 3:
                        Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(3), Rack_block_size3, CMD_DELAY_200);
                        loop++;
                        break;
                    case 4:
                        Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(4), Rack_block_size4, CMD_DELAY_200);
                        loop++;
                        break;
    
                        case 5:
               
                     
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(5), Rack_block_size5, CMD_DELAY_200);
                        
                        loop++;
                        break;
                        case 6:
                  
                     
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(6), Rack_block_size6, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 7:
                  
                       
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(7), Rack_block_size7, CMD_DELAY_200);
                      
                        loop++;
                        break;
                    case 8:
                  
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(8), Rack_block_size8, CMD_DELAY_200);
                       
                        loop++;
                        break;
                    case 9:
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(9), Rack_block_size9, CMD_DELAY_200);
                        loop++;
                        break;
                    case 10:
                 
                   
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(10),Rack_block_size10, CMD_DELAY_200);
                   
                        loop++;
                        break;
                    case 11:
                
                   
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(11), Rack_block_size11, CMD_DELAY_200);
                      
                        loop++;
                        break;
                    case 12:
                
                     
                            Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_Block_Addr(12), Rack_block_size12, CMD_DELAY_200);
                     
                        loop++;
                        break;
                        case 13:
                         
                        Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr13, Rack_block_size13, CMD_DELAY_200);
    
                    loop++;
                    break;
                    case 14:
                     
                         Modbus_TCP_Read(socket_Rack6_10, Rack_id+1, MODBUS_READ_TYPE_04, Rack_block_addr14, Rack_block_size14, CMD_DELAY_200);
    
                loop++;
                break;
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
                        if(Rack_id>10)//读取6～10簇
                        {
                            Rack_id = 6;
                        }
                        is_jump = 0;
                        continue; 
                    }
                 // 接收BMS的返回数据
                 read_recv_res = Recv_Modbus_Back(socket_Rack6_10, bms_num, Rack_XIE_BUFF_LEN, XIE_DataProcess);
                // BMS通讯异常
    
    
                if (read_recv_res < 0)
                {
                    LOG_INFO("BMS-%d: close the BMS read socket, after receive back data fault", bms_num);
                    close(socket_Rack6_10);

                    Task_Is_Over = 1;
                    break;
                }
                // BMS通讯正常
                else if (read_recv_res > 0)
                {
                    Set_BMS_Comm(bms_num, IsNoFault, TRUE);
                    timeout_cnt=0;
                }
                   else if (read_recv_res == 0) {

                    timeout_cnt++;
                   if (timeout_cnt >= 75) {   
                     LOG_INFO("BMS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                     bms_num, timeout_cnt);
                    //  Set_PCS_Comm(pcs_num, IsNoFault, FALSE);

                    close(socket_Rack6_10);
               
                    break;
                 }

                }
                else
                {
                    status = Get_BMS_Comm(bms_num);
                    // 多次timeout 或者 invalid data，已经判了超时
                    if (status == IsFault)
                    {
                        LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                        close(socket_Rack6_10);

                        Task_Is_Over = 1;
                        break;
                    }
                    else if (status == IsWarn) // 超时之前就做4次重选
                    {
                        LOG_INFO("BMS-%d: close the BMS socket, time warn happen", bms_num);
                        close(socket_Rack6_10);
                        usleep(200 * 1000); // 此时间需要和Check_Dev_Timeout取余运算的除数对应，不可随便改
                        Task_Is_Over = 1;
                        break;
                    }
                }
                usleep(5*1000);
    
                if(loop==0)
                {
                    ++Rack_id;
                }
                if(Rack_id>10)
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
close(socket_Rack6_10);
return;
}
}
/*BMS心跳包发送*/
void* Bank_XIE_HB_Task(const char *arg)
{
    sysPara *sys_cfg = SysConf_GetInfo();
    int bms_num = *((int *)arg);

    LOG_INFO("HB task started for BMS-%d", bms_num);

    HBHandle* hb = hb_create(bms_num, 1000, BMS_XIENENG_Heartbit_addr);
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

            bool pcscomm_status = (Get_PCS_Comm(0) == 1) || (Get_PCS_Comm(2) == 1);

            if (pcscomm_status){
               // LOG_WARN("BMS-%d: comm broken, pause HB ", bms_num);
                hb_pause(hb);            // 暂停心跳（不中断线程）              
            }
            else 
            {
                 hb_resume(hb);//保持运行

            }
            bool  bmscommstatus = Get_BMS_Comm(bms_num);
                    // 多次timeout 或者 invalid data，已经判了超时
            if (bmscommstatus == IsFault)
                {
               LOG_INFO("BMS-%d: close the BMS socket, time out happen", bms_num);
                close(socket_Bank);
                  break;
                }

            usleep(1000 * 1000); // 1s 轮询
        }

    }
    if (hb_started) hb_stop(hb);
    hb_destroy(hb);
    return NULL;
}

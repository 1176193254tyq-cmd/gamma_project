#include "pcs_shangneng.h"
#include "main.h"
#include "modbus_protocol.h"
static void SN_DataProcess(unsigned char *ptr, int num)
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
    if (pbuf[7] == 0x04 )
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
        else if ((temp.D16 >= PCS_addr1) && (temp.D16 <PCS_addr8 ))
        {
            for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)           
            {
                RegVal.D8[1] = pbuf[dataddr++];     //（高字节在前、低字节在后）
                RegVal.D8[0] = pbuf[dataddr++]; 
                SET_INPUT(temp.D16+i, RegVal.D16);//为什么地址不偏移
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

void PCS_ShangNeng_Task(void *arg)
{
    //初始化
    int pcs_num = *((int *)arg);  // 获取值

    sysPara *sys_cfg = SysConf_GetInfo();
    int socket_fd = 0;
    int loop = 0;
    int is_send = 0;
    int numbytes = 0;
    INT8U connect_cnt={0};
    LOG_INFO("上能PCS-%d线程启动",pcs_num);
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
                    Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr1, PCS_size1, CMD_DELAY_200);
                    loop++;
                    break;
                case 1:
                    Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr2, PCS_size2, CMD_DELAY_200);
                    loop++;
                    break;
                case 2:
                    Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr3, PCS_size3, CMD_DELAY_200);
                    loop++;
                    break;
                case 3:
                    Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr4, PCS_size4, CMD_DELAY_200);
                    loop++;
                    break;
                case 4:
                    Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr5, PCS_size5, CMD_DELAY_200);
                    loop++;
                    break;
                case 5:                            
                        Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr6, PCS_size6, CMD_DELAY_200);
                    loop++;
                    break;
                case 6:               
                        Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr7, PCS_size7, CMD_DELAY_200);            
                    loop++;
                    break;
                case 7:
                        Modbus_TCP_Read(socket_fd, PCS_ShangNeng_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_addr8, PCS_size8, CMD_DELAY_200);
                    loop++;
                    break;

                default:
                    loop = 0;
                    break;
                }
                numbytes=Recv_Modbus_Back(socket_fd, pcs_num, Rack_XIE_BUFF_LEN, SN_DataProcess);
                if ((numbytes < 0)  || (numbytes == 0))
                {
                    if (numbytes == 0) //连接关闭
                    {
                        LOG_INFO("PCS-%d: LC关闭套接字 ", pcs_num);
                    }

                    if (numbytes < 0) //其他错误
                    {
                        LOG_INFO("PCS-%d:其他套接字故障 ", pcs_num);
                    }

                    close(socket_fd);
                    LOG_INFO("PCS-%d: 连接异常，断开服务端连接 ", pcs_num);
                    break;
                }
                else if (numbytes < 0) //超时
                {
   
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
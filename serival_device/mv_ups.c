#include "main.h"


void MV_UPS_DataProcess(poll_data_str *p1);
static INT8U tx_buffer[256+6]={0};
static INT8U rx_buffer[256+6]={0};
static INT16U read_data[256+6]={0};
 
static poll_data_str MV_UPS_List[]={
//* 序号  register_value    poll_state      wait_time repeat_ask   slave_id    cmd_type             register_addr               register_num*//
    {0,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr1,          MV_UPS_size1},    
    {1,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr2,          MV_UPS_size2},     
    {2,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr3,          MV_UPS_size3},
    {3,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr4,          MV_UPS_size4},
    {4,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr5,          MV_UPS_size5},
    {5,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr6,          MV_UPS_size6},    
    {6,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr7,          MV_UPS_size7},     
    {7,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr8,          MV_UPS_size8},
    {8,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr9,          MV_UPS_size9},
    {9,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr10,         MV_UPS_size10},
    {10,  &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr11,         MV_UPS_size11},
    {11,   &read_data[0],   SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr12,         MV_UPS_size12},
    {12,   &read_data[0],   SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr13,         MV_UPS_size13},
    {13,   &read_data[0],   SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    MV_UPS_addr14,         MV_UPS_size14},
};

#define MV_UPS_SIZE  (sizeof(MV_UPS_List)/sizeof(poll_data_str))              //计算巡检命令的条数

void MV_UPS_Task(const void* item)
{
    //初始化
    if (item == NULL) {
        return;
    }
    DEVICE_GROUP_T* dev_group = (DEVICE_GROUP_T*)item;
    INT8U recv_buff[SERIAL_RECV_LEN] = {0};
    int dev_fd = dev_group->dev_fd;
    int saddr = dev_group->dev_saddr;
    int recv_bytes = 0;
    dev_group->dev_start++;
    INT16U list_num=0;
    poll_data_str *p1 = MV_UPS_List;
    //开启任务处理
    while (1) {
        if ((saddr <= 0) || (saddr >= MAX_UPS_NUM)) {
          
            usleep(1000);
            break;
        }
        Modbus_RTU_POLL(tx_buffer,(p1+list_num),rx_buffer,dev_fd);
        switch ((p1+list_num)->poll_state)
        {
            case SEND_READY:
                break;
            case SEND_END:
                break;
            case WAIT_ACK:
                break;
            case ACK_OK:
                MV_UPS_DataProcess(p1+list_num);
                (p1+list_num)->poll_state=SEND_READY;
                list_num++;
                break;   
            case ACK_ERR:
        
                (p1+list_num)->poll_state=SEND_READY;
                list_num++;
                break;                                                
            default:
                break;
        }
        list_num=list_num>=MV_UPS_SIZE?0:list_num;

        //轮询周期
        usleep(20*1000);

    } //loop R/W
}
void MV_UPS_DataProcess(poll_data_str *p1)
{
    u16_conv temp,RegVal;
    INT16U index=0,null_data=0;
    INT8U *f_p;
    float f_data=0;
    INT32U data_conver=0;
    INT32S s32_data=0;
    uint64_t last_time=0;

if(p1->cmd_type==0x03)
{
 Set_UPS_Comm(0, IsNoFault); 


for (int i = 0; i < p1->register_num; i++)
{
    RegVal.D16 = p1->register_value[i];

    // 按照对外协议点表转换地址
    index = UPS_INPUT_INDEX(p1->register_addr + i);

    if (index == 27113)
    {
        // UPS模式高低字节交换
        RegVal.D16 = (INT16U)((RegVal.D16 << 8) |
                             (RegVal.D16 >> 8));

        SET_INPUT(index, RegVal.D16);
    }
    else if (index == 27109)
    {
        // 清除收到的bit11
        RegVal.D16 &= (INT16U)~(1U << 11);

        // 保留本地由27131映射过来的bit11
        RegVal.D16 |= GET_INPUT(27109) & (1U << 11);

        SET_INPUT(index, RegVal.D16);
       //  LOG_INFO("index:%d, value:%d\n", index, RegVal.D16);
    }
    else if (index == 27131)
    {
        if ((RegVal.D16 & (1U << 11)) != 0U)
        {
            // 收到的bit11为1，将27109的bit11置1
            SET_INPUT(27109, GET_INPUT(27109) | (1U << 11));
        }
        else
        {
            // 收到的bit11为0，将27109的bit11清零
            SET_INPUT(27109,
                      GET_INPUT(27109) & (INT16U)~(1U << 11));
        }

        // 清除bit11后再存入27131
        RegVal.D16 &= (INT16U)~(1U << 11);

        SET_INPUT(index, RegVal.D16);
        // LOG_INFO("index:%d, value:%d\n", index, RegVal.D16);
    }
    else
    {
        SET_INPUT(index, RegVal.D16);
        //LOG_INFO("index:%d, value:%d\n", index, RegVal.D16);
    }
}


}
 else
    {
      DebugModbusBuf("PCS Invalid data!\n", p1->register_value, 30);
    }


}
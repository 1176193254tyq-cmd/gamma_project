#ifndef MODBUS_RTU_POLL_H
#define	MODBUS_RTU_POLL_H

#include "main.h"
#include "system.h"

#define MODBUS_CMD_01       (0x01)
#define MODBUS_CMD_02       (0x02)
#define MODBUS_CMD_03       (0x03)
#define MODBUS_CMD_04       (0x04)
#define MODBUS_CMD_05       (0x05)
#define MODBUS_CMD_06       (0x06)
#define MODBUS_CMD_15       (0x0F)
#define MODBUS_CMD_16       (0x10)

/*主机单条命令发送状态分解*/
typedef enum
{
    SEND_READY  =0,
    SEND_END    =1,
    WAIT_ACK    =2,
    ACK_OK      =3,
    ACK_ERR     =4,
    WRITE_READY =5,
}poll_state_enum;


typedef struct
{
    INT16U            list_num;
    INT16U            *register_value;      //读出数据存放buffer或需要写数据存放的buffer
    poll_state_enum     poll_state;             //发送状态 0：空闲 1：等待发送完成 2：等待回复 3：回复完成，解析 4：超时处理
    uint64_t            wait_time;              //轮询等待时间  
    INT8U             repeat_ask;             //重复次数
    INT8U             slave_id;               //需要轮询的设备ID
    INT8U             cmd_type;           //命令类型  
    INT16U            register_addr;          //需要操作的寄存器地址
    INT16U            register_num;           //需要操作的寄存器数量   
}poll_data_str;



// void Modbus_Poll(usart_str *p1);
void Modbus_RTU_POLL(INT8U *send_buf,poll_data_str *p2,INT8U *rx_buf,int fd);
#endif /*__MODBUS_POLL_H */


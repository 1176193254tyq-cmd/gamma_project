/* ------------------------------------------------------------
 * Copyright (C) 2023
 * File Name : sensor_device_task.c
 * Project : 串口设备处理项目
 * Description : 传感器设备处理程序
 * File Created : 2023/03/29
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/29
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/29 文件创建
 */

#include "main.h"
#include "trina_log.h"
#include "sensor_device_task.h"
#include "modbus_rtu_poll.h"

static INT8U tx_buffer[256+6]={0};
static INT8U rx_buffer[256+6]={0};
static INT16U read_data[256+6]={0};
    // INT16U            list_num;
    // INT16U            *register_value;      //读出数据存放buffer或需要写数据存放的buffer
    // poll_state_enum     poll_state;             //发送状态 0：空闲 1：等待发送完成 2：等待回复 3：回复完成，解析 4：超时处理
    // uint64_t            wait_time;              //轮询等待时间  
    // INT8U             repeat_ask;             //重复次数
    // INT8U             slave_id;               //需要轮询的设备ID
    // INT8U             cmd_type;           //命令类型  
    // INT16U            register_addr;          //需要操作的寄存器地址
    // INT16U            register_num;           //需要操作的寄存器数量   
static poll_data_str WS_N01_List[]={
//* 序号  register_value    poll_state      wait_time repeat_ask   slave_id    cmd_type             register_addr               register_num*//
    {0,   &read_data[0],    SEND_READY,       0,       0,          1,       MODBUS_CMD_03,    RS_WS_N01_block1_addr,          RS_WS_N01_block1_size},      //这里地址定义有弊端，如果读取的地址数量多，会有问题
    {1,   &read_data[0],    SEND_READY,       0,       0,          2,       MODBUS_CMD_03,    RS_WS_N01_block1_addr,          RS_WS_N01_block1_size},      //这里地址定义有弊端，如果读取的地址数量多，会有问题
};

#define WS_LIST_SIZE  (sizeof(WS_N01_List)/sizeof(poll_data_str))              //计算巡检命令的条数

void RS_WS_N01_Sensor_Task(const void* item)
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
    poll_data_str *p1 = WS_N01_List;
    //开启任务处理
    while (1) {
        if ((saddr <= 0) || (saddr >= MAX_TEMP_NUM)) {
            // LOG_INFO("建大仁科RS-WS-N01温湿度传感器 invalid device slave address : %d ", saddr);
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
                Set_TEMP_Comm((p1+list_num)->slave_id-1, IsNoFault);
                SystemTotalData[53803 + 5 * list_num].D16 = read_data[0];
                SystemTotalData[53801 + 5 * list_num].D16 = read_data[1];
                (p1+list_num)->poll_state=SEND_READY;
                list_num++;
                break;   
            case ACK_ERR:
                // Set_TEMP_Comm(list_num, IsFault);
                (p1+list_num)->poll_state=SEND_READY;
                list_num++;
                break;                                                
            default:
                break;
        }
        list_num=list_num>=WS_LIST_SIZE?0:list_num;
        // //获取温湿度传感器数据
        // Modbus_RTU_Read03_HoldRegist(dev_fd, saddr, RS_WS_N01_block1_addr, RS_WS_N01_block1_size,CMD_DELAY_200);

        // //接收返信
        // memset((char*)recv_buff, 0, sizeof(recv_buff));
        // recv_bytes = Serial_ReadComPort(dev_fd, recv_buff, sizeof(recv_buff));
        // //接收失败
        // if (recv_bytes < 0) {
        //     // LOG_INFO("温湿度传感器 receive data failed!");
        // }
        // //接收成功
        // else if (recv_bytes > 0) {
        //     //数据解析
        //     RS_WS_N01_DataProcess(recv_buff);
        // }

        // //下一个从设备
        // saddr++;
        // if (saddr >= (dev_group->dev_saddr + dev_group->dev_num)) {
        //     saddr = dev_group->dev_saddr;
        // }
        //轮询周期
        usleep(100*1000);

    } //loop R/W
}

void RS_WS_N01_DataProcess(const INT8U* pbuf)
{
    //初始化
    if (pbuf == NULL) {
        return;
    }
#if 0
    printf("[%s:%d]  Recv : ", __FUNCTION__, __LINE__);
    for (int i = 0; i < 8; i++) {
        printf("0x%X ", pbuf[i]);
    }
    printf("\n");
#endif
    u16_conv temp = {0};
    INT16U saddr = pbuf[0];
    if (saddr >= MAX_TEMP_NUM) {
        LOG_INFO("建大仁科RS-WS-N01温湿度传感器 slave address transfinite : %d ", saddr);
        return;
    }

    if (pbuf[2] == 4) {
        //通讯正常
        Set_TEMP_Comm(saddr - 1, IsNoFault);
        //湿度
        temp.D8[1] = pbuf[3];
        temp.D8[0] = pbuf[4];
        SystemTotalData[53803 + 5 * (saddr - 1)].D16 = temp.D16;
        //温度
        temp.D8[1] = pbuf[5];
        temp.D8[0] = pbuf[6];
        SystemTotalData[53801 + 5 * (saddr - 1)].D16 = temp.D16;
    }
}


//气体传感器初始化状态，每个设备独立判断
static INT8U SGA_INIT_STATE[MAX_SERIAL_DEV_NUM] = {0};

void SGA_Sensor_Task(const void* item)
{
    //初始化
    if (item == NULL) {
        return;
    }
    DEVICE_GROUP_T* dev_group = (DEVICE_GROUP_T*)item;
    INT8U recv_buff[SERIAL_RECV_LEN] = {0};
    int dev_fd = dev_group->dev_fd;
    int saddr = dev_group->dev_saddr;
    int Loop = 0;
    int initLoop = 0;
    int recv_bytes = 0;
    dev_group->dev_start++;

    //开启任务处理
    while (1) {
        if ((saddr <= 0) || (saddr >= MAX_GAS_NUM)) {
            LOG_INFO("深国安气体传感器 invalid device slave address : %d ", saddr);
            usleep(1000);
            break;
        }
        //初始化气体传感器
        if ((initLoop < 100) && (SGA_INIT_STATE[saddr] == 0)) {
            switch (initLoop++) {
            case 0:
                //0-4（0：无；1：1位小数；2：2位小数；3：3位小数；4：4位小数）
                Modbus_RTU_Write06_SingleRegist(dev_fd, saddr, SGA_Decimal_Pos_addr, 1,CMD_DELAY_200);
                break;
            case 1:
                //1可热气体
                Modbus_RTU_Write06_SingleRegist(dev_fd, saddr, SGA_Type_addr, 1,CMD_DELAY_200);
                break;
            case 2:
                //0-6；（0：%LEL；1：%VOL等于mol/mol ； 2：PPM等于μmol/mol；3：PPB；4：无；
                //5、mg/m3；6、μg/m3；）
                Modbus_RTU_Write06_SingleRegist(dev_fd, saddr, SGA_Uint_addr, 0,CMD_DELAY_200);
                break;
            case 3:
                //00-65535
                Modbus_RTU_Write06_SingleRegist(dev_fd, saddr, SGA_Range_addr, 0xff,CMD_DELAY_200);
                break;
            default:
                initLoop = 100;
                SGA_INIT_STATE[saddr] = 1;
                break;
            }
        }
        //循环执行
        else {
            switch (Loop++) {
            case 0:
                //读取传感器状态
                Modbus_RTU_Read03_HoldRegist(dev_fd, saddr, SGA_block1_addr, SGA_block1_size,CMD_DELAY_200);
                break;
            case 1:
                //寄存器初始默认都为0,未有设置此句一直不会执行
                if ((SystemTotalData[54103 + (saddr - 1) * 40].D16 != SystemTotalData[54133 + (saddr - 1) * 40].D16)) {
                    Modbus_RTU_Write06_SingleRegist(dev_fd, saddr, SGA_Upper_addr, SystemTotalData[54133 + (saddr - 1) * 40].D16,CMD_DELAY_200);
                }
                break;
            case 2:
                //寄存器初始默认都为0,未有设置此句一直不会执行
                if ((SystemTotalData[54104 + (saddr - 1) * 40].D16 != SystemTotalData[54134 + (saddr - 1) * 40].D16)) {
                     Modbus_RTU_Write06_SingleRegist(dev_fd, saddr, SGA_Lower_addr, SystemTotalData[54134 + (saddr - 1) * 40].D16,CMD_DELAY_200);
                }
                break;
            default:
                //下一个从设备
                Loop = 0;
                saddr++;
                break;
            }
        }

        //接收返信
        memset((char*)recv_buff, 0, sizeof(recv_buff));
        recv_bytes = Serial_ReadComPort(dev_fd, recv_buff, sizeof(recv_buff));
        //接收失败
        if (recv_bytes < 0) {
            LOG_INFO("RS485: receive data failed!");
        }
        //接收成功
        else if (recv_bytes > 0) {
            //数据解析
            SGA_DataProcess(recv_buff);
        }

        //从地址都已经遍历完毕
        if (saddr >= (dev_group->dev_saddr + dev_group->dev_num)) {
            saddr = dev_group->dev_saddr;
        }
        usleep(1000);

    } //loop R/W
}

void SGA_DataProcess(const INT8U* pbuf)
{
    //初始化
    if (pbuf == NULL) {
        return;
    }
#if 0
    printf("[%s:%d]  Recv : ", __FUNCTION__, __LINE__);
    for (int i = 0; i < 8; i++) {
        printf("0x%X ", pbuf[i]);
    }
    printf("\n");
#endif
    u16_conv temp = {0};
    INT16U saddr = pbuf[0];
    if (saddr >= MAX_GAS_NUM) {
        LOG_INFO("深国安气体传感器 slave address transfinite : %d ", saddr);
        return;
    }

    //深国安气体传感器所有参数
    if (pbuf[2] == 18) {
        //通讯正常
        Set_GAS_Comm(saddr - 1, IsNoFault);
        //工作状态
        temp.D8[1] = pbuf[3];
        temp.D8[0] = pbuf[4];
        SystemTotalData[54101 + (saddr - 1) * 40].D16 = temp.D16;
        //测量浓度值
        temp.D8[1] = pbuf[5];
        temp.D8[0] = pbuf[6];
        SystemTotalData[54102 + (saddr - 1) * 40].D16 = temp.D16;
        //高报浓度值
        temp.D8[1] = pbuf[7];
        temp.D8[0] = pbuf[8];
        SystemTotalData[54103 + (saddr - 1) * 40].D16 = temp.D16;
        //低报浓度值
        temp.D8[1] = pbuf[9];
        temp.D8[0] = pbuf[10];
        SystemTotalData[54104 + (saddr - 1) * 40].D16 = temp.D16;
    }
}

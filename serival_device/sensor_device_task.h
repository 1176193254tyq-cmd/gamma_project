/* ------------------------------------------------------------
 * Copyright (C) 2023
 * File Name : sensor_device_task.h
 * Project : 串口设备处理项目
 * Description : 传感器设备处理程序
 * File Created : 2023/03/28
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/28
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/28 文件创建
 */

#ifndef SENSOR_DEVICE_TASK_H
#define SENSOR_DEVICE_TASK_H

#include <stdint.h>
#include "system.h"
 

/***************************************建大仁科RS-WS-N01温湿度传感器定义************************************/

/**
 * @brief 建大仁科RS-WS-N01温湿度传感器任务处理
 * @param[in] item 设备项目属性
 */
void RS_WS_N01_Sensor_Task(const void* item);

/**
 * @brief 处理建大仁科RS-WS-N01温湿度传感器的返回数据
 * @param[in] pbuf 接收到的缓冲区指针
 */
void RS_WS_N01_DataProcess(const INT8U* pbuf);


//建大仁科RS-WS-N0温湿度传感器
#define RS_WS_N01_block1_addr   (0x00)   //0x00 ~ 0x01，2个寄存器
#define RS_WS_N01_block1_size   (2)


/***************************************深国安气体传感器定义************************************/

/**
 * @brief 深国安气体传感器任务处理
 * @param[in] item 设备项目属性
 */
void SGA_Sensor_Task(const void* item);

/**
 * @brief 处理深国安气体传感器的返回数据
 * @param[in] pbuf 接收到的缓冲区指针
 */
void SGA_DataProcess(const INT8U* pbuf);


//深国安SGA气体传感器
#define SGA_block1_addr         (0x100)  //0x100 ~ 0x108，9个数据地址表
#define SGA_block1_size         (9)
#define SGA_Decimal_Pos_addr    (0x102)  //小数点位置
#define SGA_Type_addr           (0x103)  //物质名称
#define SGA_Uint_addr           (0x104)  //测量单位
#define SGA_Upper_addr          (0x105)  //高限报警值
#define SGA_Lower_addr          (0x106)  //低限报警值
#define SGA_Range_addr          (0x108)  //检测量程


#endif // SENSOR_DEVICE_TASK_H

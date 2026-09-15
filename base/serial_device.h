/* ------------------------------------------------------------
 * Copyright (C) 2023
 * File Name : serial_device.h
 * Project : 串口设备任务处理项目
 * Description : 串口设备任务处理
 * File Created : 2023/03/27
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/27
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY :2023/03/27 文件创建
 */

#ifndef SERIAL_DEVICE_TASK_H
#define SERIAL_DEVICE_TASK_H

#include <stdint.h>
#include "system.h"
/**
 * @brief 初始化串口硬件
 * @param[in]
 */
void Serial_Devie_Set_Mode(void);
/**
 * @brief 串口设备任务处理
 * @param[in] item 串口序号
 */
void Serial_Device_Task(INT8U port_num);

#endif // SERIAL_DEVICE_TASK_H

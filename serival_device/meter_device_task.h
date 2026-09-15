/* ------------------------------------------------------------
 * Copyright (C) 2023
 * File Name : meter_device_task.h
 * Project : 串口设备处理项目
 * Description : 电表设备处理程序
 * File Created : 2023/03/28
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/28
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/28 文件创建
 */

#ifndef METER_DEVICE_TASK_H
#define METER_DEVICE_TASK_H

#include <stdint.h>
#include "system.h"


/**
 * @brief 安科瑞PZ系列交流电能表任务处理
 * @param[in] item 设备项目属性
 */
void ACREL_PZ_Meter_Task(const void* item);

/**
 * @brief 处理安科瑞PZ系列交流电能表的返回数据
 * @param[in] pbuf 接收到的缓冲区指针
 */
void ACREL_PZ_DataProcess(const INT8U* pbuf);

/**
 * @brief 倍率调整
 * @param[in] a 倍率
 * @param[in] b 增量
 * @return 调整的倍率
 */
double Magnification_Adjustment(int a, int b);


//安科瑞PZ系列交流电能表
#define ACREL_PZ_block1_addr    (0x00)   //0000H ~ 0011H，18个设置
#define ACREL_PZ_block1_size    (18)
#define ACREL_PZ_block2_addr    (0x22)   //0022H ~ 003EH，29个状态
#define ACREL_PZ_block2_size    (29)
#define ACREL_PZ_block3_addr    (0x3F)   //003FH ~ 004EH，16个电能地址表
#define ACREL_PZ_block3_size    (16)
#define ACREL_PZ_PT_addr        (0x03)   //PT变比设置
#define ACREL_PZ_CT_addr        (0x04)   //CT变比设置


#endif // METER_DEVICE_TASK_H

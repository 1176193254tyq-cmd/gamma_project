/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : UPS_device_task.h
 * Project : 串口设备通信处理任务
 * Description : UPS设备通信处理任务
 * File Created : 2023/03/27
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/27
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/27 新规
 */

#ifndef UPS_DEVICE_TASK_H
#define UPS_DEVICE_TASK_H

#include <stdint.h>
#include "system.h"

/**
 * @brief 山特UPS任务处理
 * @param[in] item 设备项目属性
 */
void MHC_UPS_Device_Task(const void* item);

/**
 * @brief 处理山特UPS的返回数据
 * @param[in] pbuf 接收到的缓冲区指针
 * @param[in] ups_index 设备的序号
 * @note
            TX: 51 31 OD
            RX: (208.4 000.0 208.4 034 59.9 2.05 35.0 00110000<cr>
 * @retval
 */
void MHC_UPS_DataProcess(const INT8U* pbuf, const INT8U ups_index);

#endif // UPS_DEVICE_TASK_H

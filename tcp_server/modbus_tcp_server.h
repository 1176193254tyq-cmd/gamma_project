/* ------------------------------------------------------------
 * Copyright (C) 2023
 * File Name: modbus_tcp_server.h
 * Project: 国内液冷V2项目 Domestic liquid cooling V2 project
 * Description: Modbus服务器任务 Modbus server tasks
 * File Created: 2023/03/02
 * Author: jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/02
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/02 文件创建 File creation
 */

#ifndef MODBUS_TCP_SERVER_H
#define MODBUS_TCP_SERVER_H

/**
 * @brief EMS服务器通信线程
 */

void Task_EMS_Server(void);
INT16U PCSnum_calculate(INT16U address);
extern volatile BOOLEAN Group1_power_multi_Flag;
extern volatile BOOLEAN Group2_power_multi_Flag;
#define PCS_RATED_POWER_KVA    10000
/***************************************EMS服务器通信定义************************************/

//读取
#define TCP_SERVER_RECV_LEN (300)  //接收的缓存长度

#endif // MODBUS_TCP_SERVER_H

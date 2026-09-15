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

#ifndef P2P_SERVER_H
#define P2P_SERVER_H

/**
 * @brief EMS服务器通信线程
 */
 void Task_P2P_Server(void);
#define P2P_mode 159
typedef struct {
    int fd;
} ClientCtx;
/***************************************EMS服务器通信定义************************************/

//读取
#define TCP_SERVER_RECV_LEN (300)  //接收的缓存长度

#endif // MODBUS_TCP_SERVER_H

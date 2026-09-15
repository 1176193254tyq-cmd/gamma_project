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

 #ifndef WEB_SERVER_H
 #define WEB_SERVER_H
 
 /**
  * @brief EMS服务器通信线程
  */
 void Task_WEB_Server(void);
 
 extern BOOLEAN PCS_BMS_Enable_Flag;
 extern BOOLEAN BMS_PCS_Enable_Flag;
 /***************************************EMS服务器通信定义************************************/
 
 //读取
 #define TCP_SERVER_RECV_LEN (300)  //接收的缓存长度
 
 #endif // MODBUS_TCP_SERVER_H
 
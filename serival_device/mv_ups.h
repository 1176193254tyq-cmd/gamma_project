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

#ifndef MV_UPS_H
#define MV_UPS_H

#include <stdint.h>
 
#include "modbus_rtu_poll.h"





#define RS_WS_N01_block1_addr   (0x00)   //0x00 ~ 0x01，2个寄存器
#define RS_WS_N01_block1_size   (2)





void MV_UPS_Task(const void* item);



 
#define MV_UPS_addr1   (0xA9)  //Battery current ~ Output load percent
#define MV_UPS_size1         (7)

#define MV_UPS_addr2   (0xD0)  //UPS Mode inquiry
#define MV_UPS_size2         (1)

#define MV_UPS_addr3   (0x036F)  //Unit Battery Capacity
#define MV_UPS_size3         (1)

#define MV_UPS_addr4   (0x02A3)  //Fault kind
#define MV_UPS_size4         (1)

#define MV_UPS_addr5   (0x03F2)  //Battery Piece Number~
#define MV_UPS_size5         (1)

#define MV_UPS_addr6   (0x03F4)  //Input phase~Output rated VA
#define MV_UPS_size6         (4)

#define MV_UPS_addr7   (0xB2)  //P Battery voltage
#define MV_UPS_size7         (1)

#define MV_UPS_addr8   (0x048A)  //Battery Voltage~Rating Output Frequency
#define MV_UPS_size8         (3)

#define MV_UPS_addr9   (0x04F0)  //audible alarm
#define MV_UPS_size9         (1)

#define MV_UPS_addr10   (0x0B4)  //UPS Internal Temperature~Ups status
#define MV_UPS_size10         (3)

#define MV_UPS_addr11   (0x0BF)  //Battery capacity~Battery remain time
#define MV_UPS_size11         (2)
#define MV_UPS_addr12   (0x00)  //UPS warning
#define MV_UPS_size12         (1)

#define MV_UPS_addr13   (0x03F9)  
#define MV_UPS_size13         (2)
#define MV_UPS_addr14  (0x03)  //UPS warning
#define MV_UPS_size14         (3)
#endif 

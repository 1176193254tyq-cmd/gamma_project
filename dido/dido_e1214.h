/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : dido_e1214.h
 * Project :
 * Description : E1214 DI转eth设备
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/14
 * Modified By : jia.liu
 * ------------------------------------------------------------
 * HISTORY : 2022/12/14 修改函数声明
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 修改宏定义DIDO_E1214_RECV_DO
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改宏定义
 */

#ifndef DIDO_E1210_H
#define DIDO_E1210_H
#include "system.h"

/**
 * @brief DIDO转eth设备处理线程
 * @param[in] _num 设备序号
 */
void DIDO_E1214_Task(INT8U dido_num);


/***************************************DIDO通信定义************************************/

//E1214 DI
#define DIDO_E1214_block1_addr  (0x00)    //0x00 ~ 0x05，6个DI离散量
#define DIDO_E1214_block1_size  (6)

//E1214 DO
#define DIDO_E1214_block2_addr  (0x00)    //0x00 ~ 0x05，6个DO线圈
#define DIDO_E1214_block2_size  (6)

//输出指令
#define DIDO_E1214_RELAY_ON     (0xFF00)  //控制继电器ON
#define DIDO_E1214_RELAY_OFF    (0x0000)  //控制继电器OFF
#define DIDO_E1214_RELAY_NOP    (0x55AA)  //空操作

#define DIDO_E1214_RECV_DI      (0x02)    //读取DI返回
#define DIDO_E1214_RECV_DO      (0x01)    //读取D0返回

//读写定义
#define DIDO_E1214_BUFF_LEN     (64)      //接收的缓存长度


//*****************************************************//

#endif //DIDO_E1210_H

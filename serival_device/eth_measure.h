/*
 * Copyright (C) 2022
 * File Name: eth_measure.c
 * Project:
 * Description: 测控装置，协议使用modbusTcp
 * File Created:
 * Author: lj
 * -----
 * Last Modified:
 * Modified By:XuBo
 * -----
 * HISTORY:
 */

#ifndef ETH_MEASURE_H
#define ETH_MEASURE_H

#include "system.h"

/**
 * @brief 测控装置线程
 * @param _item 线程入参
 */
void ETH_MEASURE_Task(const char *_item);

// MEASURE 遥测
//#define MEASURE_block1_addr 1108 // 1个保存寄存器
//#define MEASURE_block1_size 1

#define MEASURE_SLAVE_ADDR  (1)
// MEASURE 遥信
#define MEASURE_block0_addr 10034 // 10034~10040,7个寄存器
#define MEASURE_block0_size 7
#define MEASURE_block2_addr 10085 // 10085~10090,6个寄存器
#define MEASURE_block2_size 6
#define MEASURE_block4_addr 10109 // 10109~10115,7个寄存器
#define MEASURE_block4_size 7

// MEASURE 遥控
#define MEASURE_block3_addr 1 // 地址1， 1个
#define MEASURE_block3_size 1

//输入状态
#define RELAY_ON 0xFF00  //输入ON状态
#define RELAY_OFF 0x0000 //输入OFF状态
#define RELAY_ERR 0x55AA //输入错误状态

//读取-等待 间隔
#define MEASURE_CMD_DELAY (200 * 1000) //命令下发后延迟读取

#endif // ETH_MEASURE_H

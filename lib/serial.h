/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : serial.h
 * Project :
 * Description : U8410自带的port口操作
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 添加cpp适配宏
 */

#ifndef _SERIAL_H
#define _SERIAL_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "system.h"


// /**
//  * @brief 串口初始化
//  */
// void Serial_Init();

/**
 * @brief 读取串口数据
 * @param[in] fd 串口fd
 * @param[in] data 数据缓冲区
 * @param[in] datalength 读取的最大字节数
 * @return 读取的字节数， -1代表失败
 */
int Serial_ReadComPort(int fd, void* data, int datalength);

int __Serial_ReadComPort(int fd, void *data, int datalength);
/**
 * @brief 发送串口数据
 * @param[in] fd 串口fd
 * @param[in] data 数据缓冲区
 * @param[in] datalength 最大字节数
 * @return 发送的字节数， -1代表失败
 */
int Serial_WriteComPort(int fd, const char* data, int datalength);

/**
 * @brief 打开串口
 * @param[in] ComPort 物理端口编号，数字 0-7
 * @param[in] baudrate 波特率数字
 * @param[in] databit 数据位
 * @param[in] stopbit 停止位
 * @param[in] parity 奇偶校验位
 * @return fd or -1
 */
int Serial_OpenComPort(int ComPort, int baudrate, int databit, int stopbit, INT8U parity);

/**
 * @brief 关闭串口
 * @param[in] fd 串口fd
 */
void Serial_CloseComPort(int fd);

#ifdef __cplusplus
}
#endif

#endif //_SERIAL_H

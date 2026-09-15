/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : port.h
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

#ifndef PORT_H
#define PORT_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#define SPEED_TYPE_SIZE  (8)  //波特率种类

/**
 * @brief  设置串口波特率
 * @param[in] fd 串口fd
 * @param[in] speed 波特率
 */
void PORT_Set_Speed(int fd, int speed);

/**
 * @brief  设置串口数据位、停止位、奇偶校验位
 * @param[in] fd 串口fd
 * @param[in] databits 数据位
 * @param[in] stopbits 停止位
 * @param[in] parity 奇偶校验位
 * @retval 结果，0是失败，1是成功
 */
int PORT_Set_Parity(int fd, int databits, int stopbits, int parity);

/**
 * @brief  串口初始化工作
 * @param[in] port ttyMI端口号
 * @param[in] baud 波特率
 * @retval 结果，串口fd
 */
int PORT_Init(int port, int baud);

#ifdef __cplusplus
}
#endif

#endif //PORT_H

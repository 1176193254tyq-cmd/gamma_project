/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : tcp_socket.h
 * Project : 本地控制器
 * Description : 创建TCP服务，处理基本的TCP业务
 * File Created : 2022/10/11
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 添加cpp适配宏
 * ------------------------------------------------------------
 * HISTORY : 2022/10/11 新规
 */

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include "system.h"


/***************************************TCP服务定义************************************/

//客户端线程参数
typedef struct {
    int     client_fd;      //客户端套接字
    int     server_num;     //服务端序号
} ClientPara;

//通用缓存的容量
#define NORMAL_BUF_LEN  1024

//线程执行函数声明类型
typedef void (*PTHREAD_HANDLER)(const int* pthread_value);

//数据处理函数声明类型
typedef void (*DATA_PROCESS)(const INT8U* data_buf, const int dev_num);

//数据处理函数声明类型
typedef void (*SERIAL_DATA_PROCESS)(const INT8U* data_buf, const int dev_port, const int dev_type);


//通讯状态设置函数声明类型
typedef void (*SET_DEV_COMM)(INT8U dev_num, INT8U status);

/**
 * @brief 创建本地客户端socket
 * @param[in] server_addr 服务端地址和端口
 * @param[in] time_out 收发时限
 * @return 本地客户端fd
 */
int Create_Client_Socket(struct sockaddr_in server_addr, struct timeval time_out);

/**
 * @brief 创建本地服务端socket
 * @param[in] sin_port 服务端端口，将主机的无符号短整形数转换成网络字节顺序
 * @param[in] connections 监听数量，允许连接客户端的最大数量
 * @return 本地服务端fd
 */
int Create_Server_Socket(in_port_t sin_port, int connections);

/**
 * @brief 关闭本地服务端的客户端链接
 * @param[in] server_fd 服务端fd
 */
void Accept_Client_Outline(int server_fd);

// /**
//  * @brief 创建本地服务端的客户通信线程
//  * @param[in] server_fd 服务端fd
//  * @param[in] handler 客户端通信线程，带参数的线程执行函数
//  */
// void Accept_Client_Connect(int server_fd, PTHREAD_HANDLER handler);

/**
 * @brief 创建本地服务端的客户通信线程
 * @param[in] server_num 服务端序号
 * @param[in] server_fd 服务端fd
 * @param[in] handler 客户端通信线程，带参数的线程执行函数
 */
void Accept_Socket_Connect(const int server_num, int server_fd, PTHREAD_HANDLER handler);

/**
 * @brief 监视文件描述符（套接字）
 * @param[in] socket_fd 套接字
 * @param[in] time_out 监视时限
 * @return 监视结果，大于是有读写，-1是无读写，0是超时
 */
int Monitor_Socket(int socket_fd, struct timeval time_out);

/**
 * @brief 接收MODBUS返回数据的通用处理
 * @param[in] server_fd 服务端fd
 * @param[in] dev_num 接收设备的编号
 * @param[in] recv_size 接收的数据大小
 * @param[in] handler 返回数据的处理函数，带参数的执行函数
 * @return 接收结果：小于0是通信错误，0是超时，大于0是正常
 */
int Recv_Modbus_Back(int socket_fd, int dev_num, int recv_size, DATA_PROCESS handler);

/**
 * @brief 接收MODBUS返回数据的通用处理
 * @param[in] server_fd 服务端fd
 * @param[in] dev_num 接收设备的编号
 * @param[in] recv_size 接收的数据大小
 * @param[in] handler 返回数据的处理函数，带参数的执行函数
 * @param[in] set_timer 设置通讯正常的处理函数，带参数的执行函数
 * @return 接收结果：小于0是通信错误，0是超时，大于0是正常
 */
int Recv_Modbus_Timer(int socket_fd, int dev_num, int recv_size, DATA_PROCESS handler, SET_DEV_COMM set_timer);



/**
 * @brief 接收MODBUS返回数据的通用处理
 * @param[in] server_fd 服务端fd
 * @param[in] dev_port 接收设备的编号
 * @param[in] recv_size 接收的数据大小
 * @param[in] dev_type 接收数据的设备类型
 * @param[in] handler 返回数据的处理函数，带参数的执行函数
 * @param[in] set_timer 设置通讯正常的处理函数，带参数的执行函数
 * @return 接收结果：小于0是通信错误，0是超时，大于0是正常
 */
int Recv_Modbus_Serial_Timer(int socket_fd, int dev_port, int recv_size, int dev_type, SERIAL_DATA_PROCESS handler, SET_DEV_COMM set_timer);

#ifdef __cplusplus
}
#endif

#endif //TCP_SOCKET_H

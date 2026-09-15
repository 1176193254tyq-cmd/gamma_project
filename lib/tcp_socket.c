/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : tcp_socket.c
 * Project : 本地控制器
 * Description : 创建TCP服务，处理基本的TCP业务
 * File Created : 2022/10/11
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2022/10/11
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 新规
 */

#include "main.h"
#include "tcp_socket.h"

#define TCP_SOCKET_PRINTF_EN 0

static int tcp_socket_printf(const char *__restrict __fmt, ...)
{
#if TCP_SOCKET_PRINTF_EN
    va_list args;
    va_start(args, __fmt);
    vprintf(__fmt, args);
    va_end(args);
#endif
}

int Create_Client_Socket(struct sockaddr_in server_addr, struct timeval time_out)
{
    // 初始化socket
    int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { // AF_INET IPv4协议,SOCK_STREAM TCP协议
        tcp_socket_printf("Create client socket Error : %s\n", strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    // 设置收发超时
    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&time_out, sizeof(time_out)) == -1)
    { // 过滤规则，设置发送超时
        tcp_socket_printf("Set client sockopt SO_SNDTIMEO Error : %s\n", strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out, sizeof(time_out)) == -1)
    { // 过滤规则，设置接收超时
        tcp_socket_printf("Set client sockopt SO_RCVTIMEO Error : %s\n", strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    // 连接服务端，默认IPV4
    if (connect(socket_fd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    { // 连接到目标地址
        tcp_socket_printf("Connect Server[ip:%s,port:%d]  Error : %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    return socket_fd;
}

int Create_Server_Socket(in_port_t sin_port, int connections)
{
    // 初始化socket
    int socket_fd = 0;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { // AF_INET IPv4协议,SOCK_STREAM TCP协议
        tcp_socket_printf("[%s:%d]  Create server socket Error : %s\n", __FUNCTION__, __LINE__, strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    // 设置socket属性
    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1)
    { // 设置socket_fd文件状态为非阻塞式操作
        tcp_socket_printf("[%s:%d]  Set server socket fcntl Error : %s\n", __FUNCTION__, __LINE__, strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    // 打开地址复用功能
    int optval = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) == -1)
    {
        tcp_socket_printf("[%s:%d]  Set client sockopt SO_REUSEADDR Error : %s\n", __FUNCTION__, __LINE__, strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    // 绑定本地服务端端口，默认绑定本地所有IP
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                // IP4协议
    server_addr.sin_port = sin_port;                 // 设置端口
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定本地所有IP
    if (bind(socket_fd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1)
    { // 开启对应端口的服务端
        tcp_socket_printf("[%s:%d]  Local server bind port:%d, Error : %s\n", __FUNCTION__, __LINE__, ntohs(server_addr.sin_port), strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    // 打开网络监听
    if (listen(socket_fd, connections) == -1)
    { // 打开网络监听，设置连接服务端的客户端数量
        tcp_socket_printf("[%s:%d]  Local server listen[%d] Error : %s\n", __FUNCTION__, __LINE__, connections, strerror(errno));
        close(socket_fd);
        socket_fd = -1;
        return socket_fd;
    }

    return socket_fd;
}

void Accept_Client_Outline(int server_fd)
{
    int new_client_fd = 0;
    struct sockaddr_in client_addr = {0};
    socklen_t sin_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 0, sin_size);
    // 相当于新接入的连接把他关闭掉，不让连
    if ((new_client_fd = accept(server_fd, (struct sockaddr *)(&client_addr), &sin_size)) == -1)
    {
        new_client_fd = 0;
    }
    close(new_client_fd);
    new_client_fd = 0;
}

// void Accept_Client_Connect(int server_fd, PTHREAD_HANDLER handler)
// {
//     //等待客户端的连接
//     int new_client_fd = 0;
//     struct sockaddr_in client_addr = {0};
//     socklen_t sin_size = sizeof(struct sockaddr_in);
//     memset(&client_addr, 0, sin_size);
//     if ((new_client_fd = accept(server_fd, (struct sockaddr*)(&client_addr), &sin_size)) == -1) {
//         new_client_fd = 0;
//     }
//     //开线程处理数据
//     else {
//         tcp_socket_printf("New Client[%d] connect\n", new_client_fd);
//         pthread_t  client_thread_id = 0;
//         pthread_attr_t client_thread_attr;
//         pthread_attr_init(&client_thread_attr);
//         pthread_attr_setdetachstate(&client_thread_attr, PTHREAD_CREATE_DETACHED);
//         if (pthread_create(&client_thread_id, &client_thread_attr, (void*)handler, (void*)&new_client_fd) == -1) {
//             tcp_socket_printf("Client[%d] pthread create Error:%s\n", new_client_fd, strerror(errno));
//             close(new_client_fd);
//             new_client_fd = 0;
//         }
//         else {
//             tcp_socket_printf("Client[%d] Connect from %s:%d\n", new_client_fd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
//             usleep(10 * 1000);
//         }
//     }
// }

void Accept_Socket_Connect(const int server_num, int server_fd, PTHREAD_HANDLER handler)
{
    // 等待客户端的连接
    int new_client_fd = 0;
    struct sockaddr_in client_addr = {0};
    socklen_t sin_size = sizeof(struct sockaddr_in);
    memset(&client_addr, 0, sin_size);
    if ((new_client_fd = accept(server_fd, (struct sockaddr *)(&client_addr), &sin_size)) == -1)
    {
        new_client_fd = 0;
    }
    // 开线程处理数据
    else
    {
        // 初始化线程参数和属性
        ClientPara new_client = {0};
        new_client.client_fd = new_client_fd;
        new_client.server_num = server_num;
        pthread_attr_t client_thread_attr;
        pthread_attr_init(&client_thread_attr);
        pthread_attr_setdetachstate(&client_thread_attr, PTHREAD_CREATE_DETACHED);
        // 创建客户端线程
        pthread_t client_thread_id = 0;
        if (pthread_create(&client_thread_id, &client_thread_attr, (void *)handler, (void *)&new_client) == -1)
        { // 为新连接的客户端创立新线程
            tcp_socket_printf("Client[%d] pthread create Error:%s\n", new_client_fd, strerror(errno));
            close(new_client_fd);
            new_client_fd = 0;
        }
        else
        {
            tcp_socket_printf("Sever[%d] New Connect from %s:%d  fd:%d \n", server_num, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), new_client_fd);
            usleep(10 * 1000);
        }
    }
}

int Monitor_Socket(int socket_fd, struct timeval time_out)
{
    int retval = 0;
    fd_set fs_read;

    FD_ZERO(&fs_read);
    FD_SET(socket_fd, &fs_read);

    retval = select(socket_fd + 1, &fs_read, NULL, NULL, &time_out);
    // 正常
    if (retval > 0)
    {
        return retval;
    }
    // 异常
    else
    {
        // 超时
        if (0 == retval)
        {
            return 0;
        }
        // 发生错误
        else
        {
            return -1;
        }
    }
}

int Recv_Modbus_Back(int socket_fd, int dev_num, int recv_size, DATA_PROCESS handler)
{
    // 初始化socket
    int result = -1;
    int recv_bytes = 0;
    INT8U recv_buf[NORMAL_BUF_LEN] = {0};

    // 接收返回数据
    memset((char *)recv_buf, 0, sizeof(recv_buf));
    recv_bytes = recv(socket_fd, recv_buf, recv_size, 0);

    // 接收数据异常
    if (((recv_bytes < 0) && (errno != EAGAIN)) || (recv_bytes == 0))
    {
        // 连接关闭
        if (recv_bytes == 0)
        {
            tcp_socket_printf("[%s:%d]  Dev[%d] socket[%d] receive 0\n", __FUNCTION__, __LINE__, dev_num, socket_fd);
            result = -1;
        }
        // 其他错误
        if ((recv_bytes < 0) && (errno != EAGAIN))
        {
            tcp_socket_printf("[%s:%d]  Dev[%d] Other socket[%d] fault! err[%d]:%s\n", __FUNCTION__, __LINE__, dev_num, socket_fd, errno, strerror(errno));
            result = -2;
        }
        close(socket_fd);
    }
    // 超时
    else if ((recv_bytes < 0) && (errno == EAGAIN))
    {
        // tcp_socket_printf("[%s:%d]  Receive Timeout : err[%d]:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        result = 0;
    }
    // 对接收到数据进行处理, 需要对数据进行格式校验
    else
    {
        result = recv_bytes;
        handler(recv_buf, dev_num); // 处理数据
    }

    return result;
}

int Recv_Modbus_Timer(int socket_fd, int dev_num, int recv_size, DATA_PROCESS handler, SET_DEV_COMM set_timer)
{
    // 初始化socket
    int result = 0;
    int recv_bytes = 0;
    INT8U recv_buf[NORMAL_BUF_LEN] = {0};

    // 接收返回数据
    memset((char *)recv_buf, 0, sizeof(recv_buf));
    recv_bytes = recv(socket_fd, recv_buf, recv_size, 0);

    // 接收数据异常
    if (((recv_bytes < 0) && (errno != EAGAIN)) || (recv_bytes == 0))
    {
        // 连接关闭
        if (recv_bytes == 0)
        {
            tcp_socket_printf("[%s:%d]  Dev[%d] socket[%d] receive 0\n", __FUNCTION__, __LINE__, dev_num, socket_fd);
            result = -1;
        }
        // 其他错误
        if ((recv_bytes < 0) && (errno != EAGAIN))
        {
            tcp_socket_printf("[%s:%d]  Dev[%d] Other socket[%d] fault! err[%d]:%s\n", __FUNCTION__, __LINE__, dev_num, socket_fd, errno, strerror(errno));
            result = -2;
        }

        close(socket_fd);
    }
    // 超时
    else if ((recv_bytes < 0) && (errno == EAGAIN))
    {
        // tcp_socket_printf("[%s:%d]  Receive Timeout : err[%d]:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        result = 0;
    }
    // 设置通讯正茬，对接收到数据进行处理
    else
    {
        result = recv_bytes;
        set_timer(dev_num, IsNoFault); // 设置通讯正常
        handler(recv_buf, dev_num);    // 处理数据
    }

    return result;
}



int Recv_Modbus_Serial_Timer(int socket_fd, int dev_port, int recv_size, int dev_type, SERIAL_DATA_PROCESS handler, SET_DEV_COMM set_timer)
{
    // 初始化socket
    int result = 0;
    int recv_bytes = 0;
    INT8U recv_buf[NORMAL_BUF_LEN] = {0};

    // 接收返回数据
    memset((char *)recv_buf, 0, sizeof(recv_buf));
    recv_bytes = recv(socket_fd, recv_buf, recv_size, 0);

    // 接收数据异常
    if (((recv_bytes < 0) && (errno != EAGAIN)) || (recv_bytes == 0))
    {
        // 连接关闭
        if (recv_bytes == 0)
        {
            tcp_socket_printf("[%s:%d]  Dev[%d] socket[%d] receive 0\n", __FUNCTION__, __LINE__, dev_port, socket_fd);
            result = -1;
        }
        // 其他错误
        if ((recv_bytes < 0) && (errno != EAGAIN))
        {
            tcp_socket_printf("[%s:%d]  Dev[%d] Other socket[%d] fault! err[%d]:%s\n", __FUNCTION__, __LINE__, dev_port, socket_fd, errno, strerror(errno));
            result = -2;
        }

        close(socket_fd);
    }
    // 超时
    else if ((recv_bytes < 0) && (errno == EAGAIN))
    {
        // tcp_socket_printf("[%s:%d]  Receive Timeout : err[%d]:%s\n", __FUNCTION__, __LINE__, errno, strerror(errno));
        result = 0;
    }
    // 设置通讯正茬，对接收到数据进行处理
    else
    {
        result = recv_bytes;
        set_timer(dev_port, IsNoFault);        // 设置通讯正常
        handler(recv_buf, dev_port, dev_type); // 处理数据
    }

    return result;
}

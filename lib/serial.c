/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : serial.c
 * Project :
 * Description : U8410自带的port口操作
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified :
 * Modified By :
 * ------------------------------------------------------------
 * HISTORY :
 */

#include "main.h"
#include "serial.h"

/*
 * Decription for TIMEOUT_SEC(buflen,baud);
 * baud bits per second, buflen bytes to send.
 * buflen*20 (20 means sending an octect-bit data by use of the maxim bits 20)
 * eg. 9600bps baudrate, buflen=1024B, then TIMEOUT_SEC = 1024*20/9600+1 = 3
 * don't change the two lines below unless you do know what you are doing.
*/
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+1)
#define TIMEOUT_USEC 0
#define BUFFER_LEN  1024    /* sendfile() */

//串口参数配置
typedef struct SERIAL_PARAM_TYPE_ {
    int fd;                     //串口文件描述符
    struct termios termiosOld;  //串口属性
    int nowBps;                 //串口波特率
} SERIAL_PARAM_TYPE;

static SERIAL_PARAM_TYPE serialParam[MAX_SERIAL_NUM];  //串口类型

/**
 * @brief 通过 fd 获取 串口内容的序号
 * @param[in]  fd 串口fd
 * @retval -1 or index 0-7
 */
static INT8S Serial_GetIndexFromFd(int fd)
{
    INT8S i;

    for (i = 0; i < MAX_SERIAL_NUM; i++) {
        if (serialParam[i].fd == fd) {
            break;
        }
    }

    if (i == MAX_SERIAL_NUM) {
        return -1;
    }
    else {
        return i;
    }
}

/**
 * @brief  get serial port baudrate
 * @param[in]  n: 串口序号
 * @retval baudrate
 */
static int Serial_GetBaudrate(INT8S n)
{
    return serialParam[n].nowBps;
}

/**
 * @brief  baudrate xxx to Bxxx
 * @param[in]  baudrate xxx
 * @return -1 for error
 */
static int baudrate2Bxx(int baudrate)
{
    switch (baudrate) {
    case 0:
        return (B0);
    case 50:
        return (B50);
    case 75:
        return (B75);
    case 110:
        return (B110);
    case 134:
        return (B134);
    case 150:
        return (B150);
    case 200:
        return (B200);
    case 300:
        return (B300);
    case 600:
        return (B600);
    case 1200:
        return (B1200);
    case 2400:
        return (B2400);
    case 4800:
        return (B4800);
    case 9600:
        return (B9600);
    case 19200:
        return (B19200);
    case 38400:
        return (B38400);
    case 57600:
        return (B57600);
    case 115200:
        return (B115200);
    default:
        return -1;
    }
}

/**
 * @brief  set serial port baudrate by use of file descriptor fd
 * @param[out] opt : serial termioss
 * @param[in]  baudrate
 * @retval -1 for error
 */
static int set_baudrate(struct termios* opt, int baudrate)
{
    int bps = baudrate2Bxx(baudrate);

    if (bps == -1) {
        return -1;
    }
    else {
        opt->c_cflag = bps;   /* set baudrate */
        return bps;
    }
}

/**
 * @brief 串口数据位的设置
 * @param[in] opt : serial termioss
 * @param[in] databit 数据位
 */
static void set_data_bit(struct termios* opt, int databit)
{
    opt->c_cflag &= ~CSIZE;

    switch (databit) {
    case 8:
        opt->c_cflag |= CS8;
        break;
    case 7:
        opt->c_cflag |= CS7;
        break;
    case 6:
        opt->c_cflag |= CS6;
        break;
    case 5:
        opt->c_cflag |= CS5;
        break;
    default:
        opt->c_cflag |= CS8;
        break;
    }
}

/**
 * @brief 串口停止位的设置
 * @param[in] opt : serial termioss
 * @param[in] stopbit 停止位
 */
static void set_stopbit(struct termios* opt, int stopbit)
{
    if (stopbit == 1) {
        opt->c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
    else if (stopbit == 2) {
        opt->c_cflag |= CSTOPB;  /* 2 stop bits */
    }
    else {
        opt->c_cflag &= ~CSTOPB; /* 1 stop bit */
    }
}

/**
 * @brief 串口奇偶位的设置
 * @param[in] opt : serial termioss
 * @param[in] parity 奇偶位
 */
static void set_parity(struct termios* opt, INT8U parity)
{
    switch (parity) {
    case 'N':                  /* no parity check */
        opt->c_cflag &= ~PARENB;
        break;
    case 'E':                  /* even */
        opt->c_cflag |= PARENB;
        opt->c_cflag &= ~PARODD;
        break;
    case 'O':                  /* odd */
        opt->c_cflag |= PARENB;
        opt->c_cflag |= PARODD;
        break;
    default:                   /* no parity check */
        opt->c_cflag &= ~PARENB;
        break;
    }
}

/* -1 for error */
/**
 * @brief 串口的属性设置
 * @param fd 串口fd
 * @param baudrate 波特率
 * @param databit 数据位
 * @param stopbit 停止位
 * @param parity 奇偶校验位
 * @return -1 for error
 */
static int set_port_attr(int fd, int baudrate, int databit, int stopbit, INT8U parity)
{
    struct termios termios_new;
    bzero(&termios_new, sizeof(termios_new));
    cfmakeraw(&termios_new);

    if (set_baudrate(&termios_new, baudrate) == -1) {
        return -1;
    }

    termios_new.c_cflag |= CLOCAL | CREAD;      /* | CRTSCTS */
    set_data_bit(&termios_new, databit);
    set_parity(&termios_new, parity);
    set_stopbit(&termios_new, stopbit);
    termios_new.c_oflag = 0;
    termios_new.c_lflag |= 0;
    termios_new.c_oflag &= ~OPOST;
    termios_new.c_cc[VTIME] = 1;        /* unit: 1/10 second. */
    termios_new.c_cc[VMIN] = 255; /* minimal characters for reading */
    tcflush(fd, TCIFLUSH);

    return (tcsetattr(fd, TCSANOW, &termios_new));
}


// void Serial_Init()
// {
//     INT8S i;

//     for (i = 0; i < MAX_SERIAL_NUM; i++) {
//         serialParam[i].fd = -1;
//     }
// }

int Serial_ReadComPort(int fd, void *data, int datalength)
{
    int retval = 0;
    struct timeval tv_timeout;
    fd_set fs_read;
    INT8S n = Serial_GetIndexFromFd(fd);

    if (n == -1) {
        return -1;
    }

    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);
    tv_timeout.tv_sec = TIMEOUT_SEC(datalength, Serial_GetBaudrate(n));     
    tv_timeout.tv_usec = TIMEOUT_USEC;
    // tv_timeout.tv_sec = 0;              //修改轮询间隔
    // tv_timeout.tv_usec = 200;   
    retval = select(fd + 1, &fs_read, NULL, NULL, &tv_timeout);
    if (retval > 0) {
        retval = read(fd, data, datalength);
        return (retval);
    }
    else {
        if (0 == retval) {
            return (0); //超时
        }
        else {
            return (-1);
        }
    }
}

int __Serial_ReadComPort(int fd, void *data, int datalength)
{
    int retval = 0;
    struct timeval tv_timeout;
    fd_set fs_read;
    FD_ZERO(&fs_read);
    FD_SET(fd, &fs_read);
    tv_timeout.tv_sec = 1;     
    tv_timeout.tv_usec = TIMEOUT_USEC;
    // tv_timeout.tv_sec = 0;              //修改轮询间隔
    // tv_timeout.tv_usec = 200;   
    retval = select(fd + 1, &fs_read, NULL, NULL, &tv_timeout);
    if (retval > 0) {
        retval = read(fd, data, datalength);
        return (retval);
    }
    else {
        if (0 == retval) {
            return (0); //超时
        }
        else {
            return (-1);
        }
    }
}

int Serial_WriteComPort(int fd, const char* data, int datalength)
{
    int retval, len = 0, total_len = 0;
    struct timeval tv_timeout;
    fd_set fs_write;
    INT8S n = Serial_GetIndexFromFd(fd);

    if (n == -1) {
        return -1;
    }

    FD_ZERO(&fs_write);
    FD_SET(fd, &fs_write);
    tv_timeout.tv_sec = 0;    //TIMEOUT_SEC(datalength, Serial_GetBaudrate(n));
    tv_timeout.tv_usec = 5000;

    for (total_len = 0, len = 0; total_len < datalength;) {
        retval = select(fd + 1, NULL, &fs_write, NULL, &tv_timeout);
        if (retval) {
            len = write(fd, &data[total_len], datalength - total_len);
            if (len > 0) {
                total_len += len;
            }
        }
        else {
            tcflush(fd, TCOFLUSH);      /* flush all output data */
            break;
        }
    }

    return (total_len);
}

int Serial_OpenComPort(int ComPort, int baudrate, int databit, int stopbit, INT8U parity)
{
    char* pComPort = NULL;
    int retval = 0;
    int fd = 0;

    switch (ComPort) {
    case 0:
        pComPort = "/dev/ttyRS485-1";
        break;
    case 1:
        pComPort = "/dev/ttyRS485-2";
        break;
    case 2:
        pComPort = "/dev/ttyRS485-3";
        break;
    case 3:
        pComPort = "/dev/ttyRS485-4";
        break;
    case 4:
        pComPort = "/dev/ttyRS485-5";
        break;
    case 5:
        pComPort = "/dev/ttyRS485-6";
        break;
    case 6:
        pComPort = "/dev/ttyRS485-7";
        break;
    case 7:
        pComPort = "/dev/ttyRS485-8";
        break;
    default:
        return -1;
    }

    fd = open(pComPort, O_RDWR | O_NOCTTY | O_NDELAY);
    // fd = open(pComPort, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (-1 == fd) {
        LOG_INFO("cannot open port %s", pComPort);
        return (-1);
    }

    tcgetattr(fd, &serialParam[ComPort].termiosOld);        /* save old termios value */
    retval = set_port_attr(fd, baudrate, databit, stopbit, parity);

    if (-1 == retval) {
        LOG_INFO("port %s cannot set baud rate to %d", pComPort, baudrate);
        return (-1);
    }
    else {
        serialParam[ComPort].nowBps = baudrate;
        serialParam[ComPort].fd = fd;
    }

    return fd;
}

void Serial_CloseComPort(int fd)
{
    INT8S n = Serial_GetIndexFromFd(fd);

    if (n >= 0) {
        /* flush output data before close and restore old attribute */
        tcsetattr(fd, TCSADRAIN, &serialParam[n].termiosOld);
        close(fd);
    }
}

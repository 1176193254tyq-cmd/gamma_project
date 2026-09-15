/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : port.c
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
#include "port.h"

//串口设置
//波特率
static int speed_arr[SPEED_TYPE_SIZE] = {B115200,
                                         B57600,
                                         B38400,
                                         B19200,
                                         B9600,
                                         B4800,
                                         B2400,
                                         B1200};
//波特率
static int name_arr[SPEED_TYPE_SIZE] = {115200,
                                        57600,
                                        38400,
                                        19200,
                                        9600,
                                        4800,
                                        2400,
                                        1200};

void PORT_Set_Speed(int fd, int speed)
{
    int i = 0;
    int status = 0;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for (i= 0;  i < SPEED_TYPE_SIZE;  i++) {
        if (speed == name_arr[i]) {
            tcflush(fd, TCIOFLUSH);                //刷清输入缓存或输出缓存
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if (status != 0) {
                perror("tcsetattr fd1");
            }
            return;
        }
        tcflush(fd,TCIOFLUSH);
    }
}

/**
 * @brief  设置串口数据位、停止位、奇偶校验位
 * @note
 * @retval
 */
int PORT_Set_Parity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if ( tcgetattr( fd,&options)  !=  0) {
        perror("SetupSerial 1");
        return(FALSE);
    }

    options.c_cflag &= ~CSIZE;

    switch (databits) {
    case 7:
        options.c_cflag |= CS7;
        break;
    case 8:
        options.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr,"Unsupported data size\n");
        return (FALSE);
    }

    switch (parity) {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;
        options.c_iflag &= ~INPCK;
        break;
    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);
        options.c_iflag |= INPCK;
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
        options.c_iflag |= INPCK;
        break;
    case 'S':
    case 's':
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported parity\n");
        return (FALSE);
    }

    switch (stopbits) {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr,"Unsupported stop bits\n");
        return (FALSE);
    }

    options.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);

    if (parity != 'n') {
        options.c_iflag |= INPCK;
    }
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0) {
        perror("SetupSerial 3");
        return (FALSE);
    }
    return (TRUE);
}

/**
 * @brief  串口初始化工作
 * @note
 * @retval
 */
int PORT_Init(int port, int baud)
{
    int ttyPort = -1;
    char temp[100] = {0};
    port -= 1;
    sprintf(temp, "/dev/ttyMI%d", port);

    ttyPort = open(temp, O_RDWR | O_NOCTTY);
    if (ttyPort < 0) {
        printf("open device /dev/ttyMI%d faild\n",port);
        return -1;
    }
    else {
        printf("open device /dev/ttyMI%d success\n",port);
    }

    PORT_Set_Speed(ttyPort, baud);       //设置串口波特率
    PORT_Set_Parity(ttyPort, 8, 1, 'N'); //设置8位数据位，1位停止位，无校验等其他设置。

    return ttyPort;
}

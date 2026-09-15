/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : ModbusProtocol.c
 * Project : 本地控制器
 * Description : Modbus通信协议
 * File Created : 2022/09/23
 * Author: jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 使用系统定义类型
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改函数定义 - 增加设备号，修改宏定义
 * ------------------------------------------------------------
 * HISTORY : 2022/09/23 新规
 */
#define _POSIX_C_SOURCE 200809L
#include "main.h"
#include "modbus_protocol.h"
#include <stdint.h>           // 
#include <pthread.h>
#include "time.h"
struct timespec t0;

void Modbus_COM_Read01_SingleCoil(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Read01_SingleCoil(socket_fd, device_id, start_addr, length, delay_time);
    #else
    Modbus_TCP_Read01_SingleCoil(socket_fd, device_id, start_addr, length, delay_time);
    #endif
    
}

void Modbus_COM_Read02_InputDiscrete(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Read02_InputDiscrete(socket_fd, device_id, start_addr, length, delay_time);
    #else
    Modbus_TCP_Read02_InputDiscrete(socket_fd, device_id, start_addr, length, delay_time);
    #endif
}

void Modbus_COM_Read03_HoldRegist(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Read03_HoldRegist(socket_fd, device_id, start_addr, length, delay_time);
    #else
    Modbus_TCP_Read03_HoldRegist(socket_fd, device_id, start_addr, length, delay_time);
    #endif
}

void Modbus_COM_Read04_InputRegist(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Read04_InputRegist(socket_fd, device_id, start_addr, length, delay_time);
    #else
    Modbus_TCP_Read04_InputRegist(socket_fd, device_id, start_addr, length, delay_time);
    #endif
}

void Modbus_COM_Write05_SingleCoil(int socket_fd, INT8U device_id, INT16U start_addr, INT16U value, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Write05_SingleCoil(socket_fd, device_id, start_addr, value, delay_time);
    #else
    Modbus_TCP_Write05_SingleCoil(socket_fd, device_id, start_addr, value, delay_time);
    #endif
}

void Modbus_COM_Write06_SingleRegist(int socket_fd, INT8U device_id, INT16U start_addr, INT16U value, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Write06_SingleRegist(socket_fd, device_id, start_addr, value, delay_time);
    #else
    Modbus_TCP_Write06_SingleRegist(socket_fd, device_id, start_addr, value, delay_time);
    #endif
}

void Modbus_COM_Write_Multiple(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT16U* pbuf, INT32U delay_time)
{
    #ifdef ISPLANT_V
    Modbus_RTU_Write_Multiple(socket_fd, device_id, start_addr, length, pbuf, delay_time);
    #else
    Modbus_TCP_Write_Multiple(socket_fd, device_id, start_addr, length, pbuf, delay_time);
    #endif
}




void Modbus_TCP_Read(int socket_fd, INT8U device_id, INT8U cmd_type, INT16U start_addr, INT16U length, INT32U delay_time)
{
    //初期化
    u16_conv addr, len;
    addr.D16 = start_addr;
    len.D16  = length;
    unsigned char SendBuffer[16] = {0};
    INT8S write_flag=0;
    //事务元识别符，2byte
    SendBuffer[0] = addr.D8[1];
    SendBuffer[1] = addr.D8[0];
    //协议标识符，2byte
    SendBuffer[2] = 0;
    SendBuffer[3] = 0;
    //长度，2byte
    SendBuffer[4] = 0;
    SendBuffer[5] = 0x06;
    //单元标识符，1byte
    SendBuffer[6] = device_id;
    //功能码，1byte
    SendBuffer[7] = cmd_type;
    //寄存器地址，高在前，2byte
    SendBuffer[8] = addr.D8[1];
    SendBuffer[9] = addr.D8[0];
    //寄存器数量，高在前，2byte
    SendBuffer[10] = len.D8[1];
    SendBuffer[11] = len.D8[0];

    //发送
    write_flag=write(socket_fd, SendBuffer, 12);
   // LOG_INFO("socket_fd:%d,device_id:%d,cmd_type:%d,start_addr:%d,length:%d",socket_fd,device_id,cmd_type,start_addr,length);
    if(write_flag==-1)
    {
        return;
    }
    //在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
}

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int led_on(void)
{
    int fd = open("/sys/class/leds/led2/brightness", O_WRONLY);
    if (fd < 0)
    {
        perror("open failed");
        return -1;
    }

    const char *value = "1";

    if (write(fd, value, strlen(value)) < 0)
    {
        perror("write failed");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}



void Modbus_TCP_Write_Single(int socket_fd, INT8U device_id, INT8U cmd_type, INT16U start_addr, INT16U value, INT32U delay_time)
{
    //初期化
    u16_conv addr, val;
    addr.D16 = start_addr;
    val.D16  = value;
    unsigned char SendBuffer[16] = {0};
    char LogStr[100] = {0};
    int j=0;
    char str_char[2] = "";

    //事务元识别符，2byte
    SendBuffer[0] = addr.D8[1];
    SendBuffer[1] = addr.D8[0];
    //协议标识符，2byte
    SendBuffer[2] = 0;
    SendBuffer[3] = 0;
    //长度，2byte
    SendBuffer[4] = 0;
    SendBuffer[5] = 0x06;
    //单元标识符，1byte
    SendBuffer[6] = device_id;
    //功能码，1byte
    SendBuffer[7] = cmd_type;
    //寄存器地址，高在前，2byte
    SendBuffer[8] = addr.D8[1];
    SendBuffer[9] = addr.D8[0];
    //写入的值，高在前，2byte
    SendBuffer[10] = val.D8[1];
    SendBuffer[11] = val.D8[0];
    if(socket_fd<0)
    {
        return;
    }
    //发送
//    led_on();
    write(socket_fd, SendBuffer, 12);
  //led_off();
    //LOG_INFO("socket_fd:%d,device_id:%d,cmd_type:%d,start_addr:%d,value:%d",socket_fd,device_id,cmd_type,start_addr,value);
    //  LOG_INFO("LC send is %s",SendBuffer);
     clock_gettime(CLOCK_MONOTONIC_RAW, &t0);
    // led_on();
    memset(LogStr, 0, sizeof(LogStr));
    for (j = 0; (j < sizeof(LogStr)) && (j < 12); j++)
    {
         sprintf(str_char, "%x", SendBuffer[j]);
         strcat(LogStr, str_char);
         strcat(LogStr, " ");
     }
     LOG_INFO("F06,fd:%d,id:%d, buf => %s", socket_fd, device_id, LogStr);
    //在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
}
void Modbus_TCP_Write_Heart(int socket_fd, INT8U device_id, INT8U cmd_type, INT16U start_addr, INT16U value, INT32U delay_time)
{
    //初期化
    u16_conv addr, val;
    addr.D16 = start_addr;
    val.D16  = value;
    unsigned char SendBuffer[16] = {0};
    char LogStr[100] = {0};
    int j=0;
    char str_char[2] = "";
    //事务元识别符，2byte
    SendBuffer[0] = addr.D8[1];
    SendBuffer[1] = addr.D8[0];
    //协议标识符，2byte
    SendBuffer[2] = 0;
    SendBuffer[3] = 0;
    //长度，2byte
    SendBuffer[4] = 0;
    SendBuffer[5] = 0x06;
    //单元标识符，1byte
    SendBuffer[6] = device_id;
    //功能码，1byte
    SendBuffer[7] = cmd_type;
    //寄存器地址，高在前，2byte
    SendBuffer[8] = addr.D8[1];
    SendBuffer[9] = addr.D8[0];
    //写入的值，高在前，2byte
    SendBuffer[10] = val.D8[1];
    SendBuffer[11] = val.D8[0];

    //发送
    write(socket_fd, SendBuffer, 12);
    //LOG_INFO("socket_fd:%d,device_id:%d,cmd_type:%d,start_addr:%d,value:%d",socket_fd,device_id,cmd_type,start_addr,value);
    //  LOG_INFO("LC send is %s",SendBuffer);
    memset(LogStr, 0, sizeof(LogStr));
    for (j = 0; (j < sizeof(LogStr)) && (j < 12); j++)
    {
         sprintf(str_char, "%x", SendBuffer[j]);
         strcat(LogStr, str_char);
         strcat(LogStr, " ");
     }
   //  LOG_INFO("F06,fd:%d,id:%d, buf => %s", socket_fd, device_id, LogStr);
    //在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
}
void Modbus_TCP_Write_Multiple(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT16U* pbuf, INT32U delay_time)
{
    //越界检查
    if((length * 2 + 13) >= 256)
    {
        return;
    }

    //初期化
    u16_conv addr, len;
    addr.D16 = start_addr;
    len.D16  = length;
    unsigned char SendBuffer[256] = {0};
    char LogStr[100] = {0};
    int j=0;
    char str_char[2] = "";
    //事务元识别符，2byte
    SendBuffer[0] = addr.D8[1];
    SendBuffer[1] = addr.D8[0];
    //协议标识符，2byte
    SendBuffer[2] = 0;
    SendBuffer[3] = 0;
    //长度，2byte
    SendBuffer[4] = 0;
    SendBuffer[5] = (7 + length * 2);
    //单元标识符，1byte
    SendBuffer[6] = device_id;
    //功能码，1byte
    SendBuffer[7] = 0x10;        //写多个寄存器
    //寄存器地址，高在前，2byte
    SendBuffer[8] = addr.D8[1];
    SendBuffer[9] = addr.D8[0];
    //寄存器数量，高在前，2byte
    SendBuffer[10] = len.D8[1];
    SendBuffer[11] = len.D8[0];
    //字节长度
    SendBuffer[12] = length * 2;
    //寄存器值
    int i = 0;
    u16_conv value;
    for(; i < length; i++)
    {
        value.D16 = *(pbuf + i);
        SendBuffer[13 + i * 2] = value.D8[1];
        SendBuffer[14 + i * 2] = value.D8[0];
    }

    //发送
    write(socket_fd, SendBuffer, (length * 2 + 13));
    //在服务器断开链接时，防止程序中断
        memset(LogStr, 0, sizeof(LogStr));
    for (j = 0; (j < sizeof(LogStr)) && (j < 40); j++)
    {
         sprintf(str_char, "%x", SendBuffer[j]);
         strcat(LogStr, str_char);
         strcat(LogStr, " ");
     }
     LOG_INFO("F10,fd:%d,id:%d, buf => %s", socket_fd, device_id, LogStr);
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
   
}

void Modbus_TCP_Write_MultipleCoil(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT8U *pbuf, INT32U delay_time)
{
    // 初期化
    u16_conv addr, len;
    INT16U exlen = 0;
    unsigned char SendBuffer[256] = {0};
    addr.D16 = start_addr;
    len.D16 = length;
    if((len.D16%8)>0)
        exlen = 1;
    exlen += (len.D16/8);
     // 越界检查
    if ((exlen + 13) >= 256)
    {
        return;
    }
    

    // 事务元识别符，2byte
    SendBuffer[0] = addr.D8[1];
    SendBuffer[1] = addr.D8[0];
    // 协议标识符，2byte
    SendBuffer[2] = 0;
    SendBuffer[3] = 0;
    // 长度，2byte
    SendBuffer[4] = 0;
   
    
    SendBuffer[5] = (7 + exlen);
    // 单元标识符，1byte
    SendBuffer[6] = device_id;
    // 功能码，1byte
    SendBuffer[7] = 15; // 写多个寄存器
    // 寄存器地址，高在前，2byte
    SendBuffer[8] = addr.D8[1];
    SendBuffer[9] = addr.D8[0];
    // 寄存器数量，高在前，2byte
    SendBuffer[10] = len.D8[1];
    SendBuffer[11] = len.D8[0];
    // 字节长度
    SendBuffer[12] = exlen;
    // 寄存器值
    int i = 0;
    for (; i < exlen; i++)
    {
        SendBuffer[13 + i] = *(pbuf + i);
    }

    // 发送
    write(socket_fd, SendBuffer, (exlen + 13));
    // 在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    // 命令下发后延迟读取
    usleep(delay_time);
}


void Modbus_TCP_Read01_SingleCoil(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_TCP_Read(socket_fd, device_id, MODBUS_READ_TYPE_01, start_addr, length, delay_time);
}

void Modbus_TCP_Read02_InputDiscrete(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_TCP_Read(socket_fd, device_id, MODBUS_READ_TYPE_02, start_addr, length, delay_time);
}

void Modbus_TCP_Read03_HoldRegist(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_TCP_Read(socket_fd, device_id, MODBUS_READ_TYPE_03, start_addr, length, delay_time);
}

void Modbus_TCP_Read04_InputRegist(int socket_fd, INT8U device_id, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_TCP_Read(socket_fd, device_id, MODBUS_READ_TYPE_04, start_addr, length, delay_time);
}

void Modbus_TCP_Write05_SingleCoil(int socket_fd, INT8U device_id, INT16U start_addr, INT16U value, INT32U delay_time)
{
    Modbus_TCP_Write_Single(socket_fd, device_id, MODBUS_WRITE_TYPE_05, start_addr, value, delay_time);
}

void Modbus_TCP_Write06_SingleRegist(int socket_fd, INT8U device_id, INT16U start_addr, INT16U value, INT32U delay_time)
{
    Modbus_TCP_Write_Single(socket_fd, device_id, MODBUS_WRITE_TYPE_06, start_addr, value, delay_time);
}





//RS485部分

static void Modbus_RTU_Read(int serial_fd, INT8U cmd_type, INT8U station_no, INT16U start_addr, INT16U length, INT32U delay_time)
{
    //初期化
    u16_conv addr, len;
    addr.D16 = start_addr;
    len.D16  = length;
    unsigned char SendBuffer[16] = {0};

    //站号，1byte
    SendBuffer[0] = station_no;
    //功能码，1byte
    SendBuffer[1] = cmd_type;
    //寄存器地址，高在前，2byte
    SendBuffer[2] = addr.D8[1];
    SendBuffer[3] = addr.D8[0];
    //寄存器数量，高在前，2byte
    SendBuffer[4] = len.D8[1];
    SendBuffer[5] = len.D8[0];
    //CRC16，低在前，2byte
    SendBuffer[6] = crc16(SendBuffer, 6) & 0xFF;
    SendBuffer[7] = (crc16(SendBuffer, 6) >> 8) & 0xFF;

    //发送
    write(serial_fd, SendBuffer, 8);
    //在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
}

void Modbus_RTU_Read01_SingleCoil(int serial_fd, INT8U station_no, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_RTU_Read(serial_fd, MODBUS_READ_TYPE_01, station_no, start_addr, length, delay_time);
}

void Modbus_RTU_Read02_InputDiscrete(int serial_fd, INT8U station_no, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_RTU_Read(serial_fd, MODBUS_READ_TYPE_02, station_no, start_addr, length, delay_time);
}

void Modbus_RTU_Read03_HoldRegist(int serial_fd, INT8U station_no, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_RTU_Read(serial_fd, MODBUS_READ_TYPE_03, station_no, start_addr, length, delay_time);
}

void Modbus_RTU_Read04_InputRegist(int serial_fd, INT8U station_no, INT16U start_addr, INT16U length, INT32U delay_time)
{
    Modbus_RTU_Read(serial_fd, MODBUS_READ_TYPE_04, station_no, start_addr, length, delay_time);
}

static void Modbus_RTU_Write_Single(int serial_fd, INT8U cmd_type, INT8U station_no, INT16U start_addr, INT16U value, INT32U delay_time)
{
    //初期化
    u16_conv addr, val;
    addr.D16 = start_addr;
    val.D16  = value;
    unsigned char SendBuffer[16] = {0};

    //站号，1byte
    SendBuffer[0] = station_no;
    //功能码，1byte
    SendBuffer[1] = cmd_type;
    //寄存器地址，高在前，2byte
    SendBuffer[2] = addr.D8[1];
    SendBuffer[3] = addr.D8[0];
    //寄存器值，高在前，2byte
    SendBuffer[4] = val.D8[1];
    SendBuffer[5] = val.D8[0];
    //CRC16，低在前，2byte
    SendBuffer[6] = crc16(SendBuffer, 6) & 0xFF;
    SendBuffer[7] = (crc16(SendBuffer, 6) >> 8) & 0xFF;

    //发送
    write(serial_fd, SendBuffer, 8);
    //在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
}

void Modbus_RTU_Write05_SingleCoil(int serial_fd, INT8U station_no, INT16U start_addr, INT16U value, INT32U delay_time)
{
    Modbus_RTU_Write_Single(serial_fd, MODBUS_WRITE_TYPE_05, station_no, start_addr, value, delay_time);
}

void Modbus_RTU_Write06_SingleRegist(int serial_fd, INT8U station_no, INT16U start_addr, INT16U value, INT32U delay_time)
{
    Modbus_RTU_Write_Single(serial_fd, MODBUS_WRITE_TYPE_06, station_no, start_addr, value, delay_time);
}

void Modbus_RTU_Write_Multiple(int serial_fd, INT8U station_no, INT16U start_addr, INT16U length, INT16U* pbuf, INT32U delay_time)
{
    //越界检查
    if((length * 2 + 13) >= 256)
    {
        return;
    }

    //初期化
    u16_conv addr, len;
    addr.D16 = start_addr;
    len.D16  = length;
    unsigned char SendBuffer[256] = {0};


    //站号，1byte
    SendBuffer[0] = station_no;
    //功能码，1byte
    SendBuffer[1] = 0x10;        //写多个寄存器
    //寄存器地址，高在前，2byte
    SendBuffer[2] = addr.D8[1];
    SendBuffer[3] = addr.D8[0];
    //寄存器数量，高在前，2byte
    SendBuffer[4] = len.D8[1];
    SendBuffer[5] = len.D8[0];
    //字节长度
    SendBuffer[6] = length * 2;
    //寄存器值
    int i = 0;
    u16_conv value;
    for(; i < length; i++)
    {
        value.D16 = *(pbuf + i);
        SendBuffer[7 + i * 2] = value.D8[1];
        SendBuffer[8 + i * 2] = value.D8[0];
    }
    i--;
    //CRC16，低在前，2byte
    SendBuffer[9 + i * 2] = crc16(SendBuffer, (9 + i * 2)) & 0xFF;
    SendBuffer[10 + i * 2] = (crc16(SendBuffer, (9 + i * 2)) >> 8) & 0xFF;

    //发送
    write(serial_fd, SendBuffer, (i * 2 + 11));
    //在服务器断开链接时，防止程序中断
    signal(SIGPIPE, SIG_IGN);
    //命令下发后延迟读取
    usleep(delay_time);
}


INT8U CRC_Check(const INT8U *data)
{
    INT8U resault=0;
    INT16U crc_value=0;
    INT16U rx_crc=0;
    INT8U type_data=*(data+1);
    INT16U data_lenth=0;
    data_lenth=*(data+2);
    if(type_data&0x80)
    {
        printf("从机回复错误\n");
        return resault;
    }
    crc_value=crc16((INT8U *)(data),data_lenth+3);
    rx_crc=(data[data_lenth+3]<<0)|(data[data_lenth+3+1]<<8);
    printf("CRC的值：%x %x\n",crc_value,rx_crc);
    if(crc_value==rx_crc) 
    {
        resault=1;
        printf("命令-%d CRC校验成功\n",type_data);
    }
    else
    {
        printf("命令-%d CRC校验失败\n",type_data);
    }
    return resault;
}
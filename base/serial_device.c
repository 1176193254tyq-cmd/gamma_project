/* ------------------------------------------------------------
* Copyright (C) 2023
* File Name : serial_device.c
* Project : 串口设备任务处理项目
* Description : 串口设备任务处理
* File Created : 2023/03/27
* Author : jinxue.zhao
* ------------------------------------------------------------
* Last Modified : 2023/03/27
* Modified By : jinxue.zhao
* ------------------------------------------------------------
* HISTORY :2023/03/27 文件创建
*/

#include "main.h"
#include "trina_log.h"
#include "serial_device.h"
#include "meter_device_task.h"
#include "sensor_device_task.h"
#include "ups_device_task.h"


pthread_mutex_t mutex_serival[8];

//****************初始化8410的串口，0:232 1:485 2:422 3:485-4*******************//
void Serial_Devie_Set_Mode(void)
{
Do_System("setinterface /dev/ttyMI0 0", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI1 1", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI2 1", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI3 1", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI4 1", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI5 1", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI6 1", 1);
usleep(20);
Do_System("setinterface /dev/ttyMI7 1", 1);
usleep(20);
}

void Serial_Device_Task(INT8U port_num)
{
// 初始化
//  const int port_num = (*(int*)item);
sysPara *sys_cfg = SysConf_GetInfo();
int ttyPort = 0;
ThreadConfirm(__func__);
if (port_num >= MAX_SERIAL_NUM)
{
LOG_INFO("串口端口设定超限, port_num:%d", port_num);
return;
}
// 串口初始化
volatile int baudrate = sys_cfg->serial_baud[port_num];
volatile int databit = sys_cfg->serial_databit[port_num];
volatile int stopbit = sys_cfg->serial_stopbit[port_num];
volatile int parity = sys_cfg->serial_parity[port_num];
ttyPort = Serial_OpenComPort(port_num, baudrate, databit, stopbit, parity);
if (ttyPort < 0)
{
LOG_INFO("不能打开串口 port %d, %d, %d, %d, %c, errno:%d", port_num, baudrate, databit, stopbit, parity, errno);
Serial_CloseComPort(ttyPort);
return;
}
else
{
LOG_INFO("Port-%d 串口打开成功, attr:%d, %d, %d, %c fd:%d", port_num, baudrate, databit, stopbit, parity, ttyPort);
}

// 设置分离式线程属性
pthread_attr_t thread_attr;
pthread_attr_init(&thread_attr);
pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

// 统一串口设备信息
pthread_t thread_id;
volatile INT32U timeout_count = 0;
SERIAL_LIST_T *dev_list = &sys_cfg->serial_list[port_num];
typedef void *(*serial_device_thread_func)(void *); // 线程执行函数声明类型
/*20230801添加线程互斥锁，控制串口资源*/
pthread_mutex_init(&mutex_serival[port_num], NULL); // 每个串口都初始化一个互斥锁，防止多个线程共同调用同一个串口


// 创建任务
for (int i = 0; (i < sys_cfg->serial_group[port_num]) && (i < MAX_SERIAL_NUM); i++)
{
// 设备组属性初始化
DEVICE_GROUP_T *dev_group = &dev_list->group_list[i];
dev_group->dev_fd = ttyPort;
dev_group->dev_port = port_num;
dev_group->dev_start = 0;
serial_device_thread_func start_routine = NULL;
timeout_count = 0;


if (dev_group->dev_type == MV_UPS)
{
LOG_INFO("创建串口线程");
start_routine = (void *)MV_UPS_Task;
}


LOG_INFO("创建中的是%d",dev_group->dev_fd );
// 创建任务线程
pthread_create(&thread_id, &thread_attr, start_routine, (void *)dev_group);
do
{
usleep(100);
timeout_count++;
// 线程启动成功
if (dev_group->dev_start > 0)
{
LOG_INFO("device: %d, ID: %lu 线程启动成功", dev_group->dev_type, thread_id);
break;
}
// 10秒钟超时，线程启动失败
if ((timeout_count * 100) >= (TIMEOUT_10sec_CNT * 10 * 1000))
{
LOG_INFO("device: %d, ID: %lu 线程启动失败", dev_group->dev_type, thread_id);
break;
}
} while (1);
}
while (1)
{
// 线程重启功能
sleep(10); // 延时
}
}

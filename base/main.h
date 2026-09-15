/* ------------------------------------------------------------
 * Copyright (C) 2020
 * File Name : main.h
 * Project :
 * Description : 程序通用配置
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/11/11
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/10/08 修改宏定义
 */

#ifndef MAIN_H
#define MAIN_H

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netdb.h>
#include <pthread.h>
#include <termios.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/reboot.h>
#include <linux/rtc.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/sem.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>

//#define ISPLANT_V          //

#include "assist_func.h"
#include "app_config.h"
#include "ini.h"
#include "log.h"
#include "modbus_protocol.h"
#include "port.h"
#include "serial.h"
#include "system.h"
#include "tcp_socket.h"
#include "timer.h"


#include "bms_xieneng.h"
#include "pcs_pe.h"
#include "dido_e1214.h"
#include "di_e1210.h"
#include "modbus_tcp_server.h"
#include "serial_device.h"
#include "state_task.h"
#include "timed_check.h"
#include "timed_work.h"
#include "trina_log.h"

#include "cJSON_app.h"
#include "ir_log.h"
#include "address_map.h"

#include "share_ram.h"
#include "pcs_shangneng.h"
#include "pcs_pe_write.h"
#include "pcs_pe.h"
#include "bms_xieneng_write.h"

#include "web_server.h"

#include "pcs_trina.h"

#include "pcs_taida.h"
#include "cem9000.h"
#include "pcs_taida_write.h"
#include "pcs_trina_write.h"
#include "mv_ups.h"
#include "pcs_write_queue.h"
#include "whitelist.h"
#include "heart_beat.h"
#include <mysql.h>
#include "xxtea.h"
#include "crypto_util.h"
#include "acl_db.h"

#include "bms_g2pro.h"
#include "p2p_server.h"
#include "bms_write_queue.h"
#include "pcs_globals.h"
#include "log_g2_mv.h"
#include "reactive_allocate.h"
//开关标志 Switch flag
#define ON  (1)
#define OFF (0)

//执行结果 Execution result
#define EXEC_OK (1) // 成功 (Success)
#define EXEC_FAIL (-1) // 失败 (Failure)
#define FALSE (0) // 错误 (Error)
#define TRUE (1) // 正确 (Correct)

//系统的数据总长度  Total length of system data
#define MAX_SYSTEM_TOTAL_DATA_NUM (65530) // 点表数据长度 (Length of point table data)
#define MAX_NETWORK_DATA_NUM (1024) // 网址数据长度 (Length of network address data)
#define MAX_THREAD_FUNC_POOL_NUM (64) // 线程池容量 (Thread pool capacity)m

//总/子系统控制状态 Total/subsystem control state
#define Ery_CON (0) // 子控为0,切入子系统,发0下来 (Sub-control is 0, enter subsystem, send 0 down)
#define ALL_CON (1) // 总控为1,切出子系统,发1下来,初始化默认为1 (Total control is 1, exit subsystem, send 1 down, initialized as 1 by default)
//串口收发缓存定义 Serial port send/receive buffer definition
#define SERIAL_SEND_LEN (16) // 发送的缓存长度 (Length of the send buffer)
#define SERIAL_RECV_LEN (300) // 接收的缓存长度 (Length of the receive buffer)
#define STATUS_WORD1  (218)
#define STATUS_WORD2  (219)
#define STATUS_WORD3  (220)
#define STATUS_WORD4  (221)
 #define LC_PROTOCOL_VERSION 1 
 #define SOFTWARE_MAJOR_VERSION 2
 #define SOFTWARE_MINOR_VERSION_H 3
 #define SOFTWARE_MINOR_VERSION_L 4
 #define SOFTWARE_SPC_VERSION 5
 #define REMOTE_LOCAL_CONTROL_ENABLE 4
  #define REMOTE_LOCAL_CONTROL_MODE 5

  #define NOMINAL_CAPCTIY_H  100  
   #define NOMINAL_CAPCTIY_L  101  

  #define NOMINAL_ENERGY_H  102  
   #define NOMINAL_ENERGY_L  103  

 #define SYSTEM_SOC 104
 #define SYSTEM_SOH 105
 #define SYSTEM_STATUS 106
 #define SYSTEM_FAULT_SUMMARY 107
 #define AVAILABLE_CHARGE_ENERGYH 109
 #define AVAILABLE_CHARGE_ENERGYL 110
 #define AVAILABLE_DISCHARGE_ENERGYH 111
 #define AVAILABLE_DISCHARGE_ENERGYL 112

 #define AVAILABLE_CHARGE_POWERH 113
 #define AVAILABLE_CHARGE_POWERL 114

 #define AVAILABLE_DISCHARGE_POWERH 115
 #define AVAILABLE_DISCHARGE_POWERL 116
 #define STATE_OF_THE_ENERGY_STATION 121
 #define POWER_FACTOR 122
#define APPARENT_POWERH 123
 #define APPARENT_POWERL 124
#define ATVICE_POWERH 125
 #define ATVICE_POWERL 126

 #define REACTIVE_POWERH 127
 #define REACTIVE_POWERL 128

 #define AC_FREQUENCYH 129
 #define AC_FREQUENCY 130

 #define DC_POWERH 131
 #define DC_POWERL 132 

 #define DC_CURRENTH 133
 #define DC_CURRENTL 134

  #define DC_AVERAGE_VOLTAGEH 135
 #define DC_AVERAGE_VOLTAGEL 136

 #define Daily_charged_EnergyH 137
 #define Daily_charged_EnergyL 138


  #define Daily_discharged_EnergyH 139
  #define Daily_discharged_EnergyL 140

  #define Total_charged_EnergyH   141
  #define Total_charged_EnergyL   142
  #define Total_discharged_EnergyH   143
  #define Total_discharged_EnergyL   144


 #define AC_Daily_charged_EnergyH 145
 #define AC_Daily_charged_EnergyL 146


  #define AC_Daily_discharged_EnergyH 147
  #define AC_Daily_discharged_EnergyL 148

  #define AC_Total_charged_EnergyH   149
  #define AC_Total_charged_EnergyL   150
  #define AC_Total_discharged_EnergyH   151
  #define AC_Total_discharged_EnergyL   152


 #define AC_Daily_Cap_Reactive_EnergyH 161
 #define AC_Daily_Cap_Reactive_EnergyL 162


  #define AC_Daily_Ind_Reactive_EnergyH 163
  #define AC_Daily_Ind_Reactive_EnergyL 164

  #define AC_Total_Cap_Reactive_EnergyH   165
  #define AC_Total_Cap_Reactive_EnergyL   166

  #define AC_Total_Ind_Reactive_EnergyH   167
  #define AC_Total_Ind_Reactive_EnergyL   168




  #define AVAILABLE_INDUCTIVE_REACTIVE_POWERH 117
  #define AVAILABLE_INDUCTIVE_REACTIVE_POWERL 118

  #define AVAILABLE_CAPACTIVE_REACTIVE_POWERH 119
  #define AVAILABLE_CAPACTIVE_REACTIVE_POWERL 120


 #define Year 153
 #define Month 154 

 #define Day 155
 #define Hour 156

 #define Minute 157
 #define Second 158
#define Blackstart_mode 170

#define High_voltage_switch_status 171

#define NUMBER_OF_PCSs 200
#define NUMBER_OF_RUNNING_PCSs 201
#define NUMBER_OF_WARNING_PCSs 202
#define NUMBER_OF_FAULT_PCSs 203

#define NUMBER_OF_BMSs 204
#define NUMBER_OF_RUNNING_BMSs 205
#define NUMBER_OF_WARNING_BMSs 206
#define NUMBER_OF_FAULT_BMSs 207

#define PCS_GROUP_STATUS 221


#define STANDBY 4
#define FAULT 5
#define ALARM 6
#define WARN 7
#define HAVE_STOPPED 1
#define BE_STOPPING 11


#define S_BUS (0xF)
#define D_BUS (0xFF)
#define MSG "change@/devices/platform/gpio-charger/power_supply/gpio-charger"
#define DEVPATH "/sys/class/power_supply/gpio-charger/online"
//故障状态枚举类型 Fault status enumeration type
enum enumIOFaultFlag {
IsNoFault = 0, // 没有故障 (No fault)
IsFault, // 故障发生 (Fault occurred)
IsStandby, // 状态保持 (State held)
IsWarn,
IsnoWarn,
};

typedef enum
{
    Init = 0,
    Stop = 1,
    Starting = 2,
    Run = 3,
    Standby =4 ,
    Fault = 5,
}Subsystem_State_ENUM;

typedef struct
{
    INT16U Type; 		//类型
    INT16U RegAdd; 		//协议中寄存器地址
    INT16U WRegAdd;     //协议中写寄存器地址
    u16_conv Dat;		//数据内容
    u16_conv WDat;		//设置数据
    INT16U Sta;		    //当前设置状态
} reg_st, *preg_st;

//System data cache
extern volatile u16_conv SystemTotalData[MAX_SYSTEM_TOTAL_DATA_NUM]; // 点表数据 (Point table data)
extern volatile u16_conv NetworkTotalData[MAX_NETWORK_DATA_NUM]; // 网址数据 (Network address data)



//交流电表CT AC meter CT
extern INT8U MS_AC_METER_CT;

extern Subsystem_State_ENUM system_work_state[MAX_GROUP_NUM];


//线程池函数定义结构体
typedef struct {
    char        func_name[128];                 //线程函数名称 Thread function name
    INT8U     func_start;                     //线程启动标志 Thread start flag
    int         para_index;                     //线程参数 Thread parameter
    void*       (*start_routine) (void*);       //线程启动函数地址 Thread start function address
    pthread_t   thread_id;                      //线程创建ID值 Thread create ID value
} PthreadFuncPool;
//线程池函数结构体数组 Thread pool function structure array 
extern PthreadFuncPool THREAD_FUNC_POOL[MAX_THREAD_FUNC_POOL_NUM];
//线程池 Thread pool
extern INT16U thread_func_pool_pos;
//线程池结构内容初始化定义宏 Thread pool structure content initialization definition macro
#define InitThtreadFunc(pthread_start, pthread_name, pthread_param)    do {         \
    memset(&THREAD_FUNC_POOL[thread_func_pool_pos], 0, sizeof(PthreadFuncPool));    \
    strcpy(THREAD_FUNC_POOL[thread_func_pool_pos].func_name, pthread_name);         \
    [thread_func_pool_pos].func_start = 0;                          \
    THREAD_FUNC_POOL[thread_func_pool_pos].para_index = pthread_param;              \
    THREAD_FUNC_POOL[thread_func_pool_pos].start_routine = (void*)pthread_start;    \
    thread_func_pool_pos++;                                                         \
} while (0) 
//线程函数启动确认定义宏 Thread function startup confirmation definition macro
#define ThreadConfirm(pthread_name)    do {                                 \
    for (int i = 0; i < 64; i++) {                                          \
        if (strcmp(THREAD_FUNC_POOL[i].func_name, pthread_name) == 0) {     \
            THREAD_FUNC_POOL[i].func_start++;                               \
        }                                                                   \
    }                                                                       \
} while(0)


/**
 * @brief   system函数封装
 * @note
 *      cmd :指令内容
 *      times :执行次数
 *      当同时满足下面三个条件时才是执行成功
 *           （1）-1 != status
 *           （2）WIFEXITED(status)为真
 *           （3）0 == WEXITSTATUS(status)
 * @retval true 成功，false 失败
 * @brief   Encapsulation of system function
 * @note
 *      cmd : command content
 *      times : execution times
 *      The execution is successful only when all of the following three conditions are met:
 *           (1) -1 != status
 *           (2) WIFEXITED(status) is true
 *           (3) 0 == WEXITSTATUS(status)
 * @retval true for success, false for failure
 */
bool Do_System(const char* cmd, int times);
#endif //MAIN_H

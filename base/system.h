/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : system.h
 * Project :
 * Description : 系统相关的设定
 * File Created :
 * Author : jinjin.wu
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 新增network配置文件路径宏
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改宏定义
 */

#ifndef SYSTEM_H
#define SYSTEM_H
#include "system.h"

//无效参数宏函数
#define UNUSED_PARAM(a)  (void)(a)

//Modbus缓存打印宏函数
#define DEBUG_MACRO_NAME 1
#ifdef DEBUG_MACRO_NAME
#define DebugModbusBuf(string, ptr_buf, len) do { \
    printf("[%s:%d]  "string, __FUNCTION__, __LINE__); \
    for (int j = 0; j < len; j++) \
    { \
         printf("%02x ", ptr_buf[j]); \
    } \
    printf("\n"); \
} while(0)
#define DebugPrint(fmt, ...)  printf("Debug INFO [%s:%d]  "fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DebugModbusBuf(string, ptr_buf, len) (void)NULL
#define DebugPrint(fmt, ...)  (void)NULL
#endif

//文件
//#define CONFIG_PATH         "/home/moxa/set.ini"                   //系统配置文件
#define CONFIG_PATH         "./lc_data_set.ini"               //系统配置文件
#define NVME_MOUNT_POINT   "/mnt/nvme"
#define NVME_LOG_DIR       "/mnt/nvme/zlog"
#define FLASH_LOG_DIR      "/home/zlgmcu/zlog"
#define LOG_LINK_DIR       "/home/zlgmcu/zlog_link"
#define LOG_CONFIG_FILE    "/home/zlg/zlog.conf"
#define NET_PATH            "/etc/network/interfaces"              //系统网络接口定义文件
#define NET_PATH_BAK        "/etc/network/interfaces.bak"          //系统网络接口定义文件的备份文件

//命令值
#define CMD_START                 (0xF3)  //启动命令
#define CMD_STOP                  (0xF0)  //关机命令
#define CMD_RESET                 (0xF2)  //复位命令
#define CMD_STANDBY               (0xF5)  //待机命令
//子系统切入切出值
#define SUB_SYSTEM_IN             (0x00)  //子系统切入值
#define SUB_SYSTEM_OUT            (0x01)  //子系统切出值

/* 
#define Run_State_Reset 0x00       //复位
#define STOP 0xF0                  //已停机-----------
#define STARTING 0xF2              //启动中oooooooooo
#define RUNING 0xF3                //运行中---------

#define STANDBY 0xF5               //已待机----------
#define STOP_WITH_FAULT 0xF6       //故障停机------------
#define RUN_REDUCE_POWR 0xF7       //降额运行ooooooo
#define RUN_WITH_WARN 0xF8         //告警运行---------
#define RUN_WITH_Branch_Fault 0xF9 //部分支路故障运行
#define RUN_MODE_ADDR 221          //运行模式 */



//子/系统状态值
#define SYS_STOP                  (0xF0)  //停机
#define SYS_STARTING              (0xF2)  //启动中  
#define SYS_RUNING                (0xF3)  //运行
#define SYS_STANDBY               (0xF5)  //待机
#define SYS_STOP_WITH_FAULT       (0xF6)  //故障停机
#define SYS_RUN_WITH_WARN         (0xF8)  //告警运行
#define SYS_RUN_WITH_FAULT        (0xF9)  //支路故障运行
//???
#define STOPING 0xF4               //关机中ooooooooooo




//当EMS检测到系统有故障时，不再分配功率给储能单元
#define SYS_NORMAL_STATE          (0x00)  //系统正常
#define SYS_ABNORMAL_STATE        (0xEE)  //系统有告警/故障

//系统状态地址
#define SYS_PROTOCOL_VER_ADDR     (1)     //协议版本
#define SYS_SOFTWARE_VER_ADDR     (2)     //LC软件主版本
#define SYS_VERSION_YEAR_ADDR     (3)     //LC软件从版本H
#define SYS_VERSION_DATE_ADDR     (4)     //LC软件从版本L
#define SYS_PCS_NUM_ADDR          (53)    //系统PCS总台数
#define SYS_BMS_NUM_ADDR          (54)    //系统BMS总台数
#define SYS_CHARGE_STATE_ADDR     (55)    //系统充放电状态 0/1/-1
#define SYS_WARN_STATE_ADDR       (56)    //系统告警状态
#define SYS_FAULT_STATE_ADDR      (57)    //系统故障状态
#define SYS_HEART_BEAT_ADDR       (58)    //系统心跳
#define SYS_RUN_TIME_ADDR         (59)    //系统运行时间
#define SYS_STOP_CODE_ADDR        (60)    //系统停机码
#define SYS_IN_PCS_NUM_ADDR       (89)    //当前PCS在线数量
#define SYS_IN_BMS_NUM_ADDR       (90)    //当前BMS在线数量
#define SYS_RUN_MODE_ADDR         (221)   //运行模式
#define SYS_SYNC_TIEM_ADDR        (1500)   //系统对时
#define SYS_PERIOD1_ADDR          (301)   //系统时段1
#define SYS_PERIOD2_ADDR          (421)   //系统时段2

//系统充放电状态
#define SYS_PCS_CHARGE_STATE      (-1)    //充电状态
#define SYS_PCS_DISCHARGE_STATE   (1)     //放电状态
#define SYS_PCS_STANDBY_STATE     (0)     //待机状态

//系统控制地址 
#define SYSTEM_CTRL_ADDR          (200)    //总系统控制
#define SUB_SYSTEM_CTRL_ADDR      (201)   //子系统1控制
#define SUB_SYSTEM2_CTRL_ADDR     (202)   //子系统2控制

//DO控制地址
#define FAULT_LED1_CTRL_ADDR      (53302)   //故障灯1控制
#define FAULT_LED2_CTRL_ADDR      (53332)   //故障灯2控制



//系统状态地址
#define SYSTEM_STATUS_ADDR        (100)    //总系统状态
#define SUB_SYSTEM_STATUS_ADDR    (101)   //子系统1状态
#define SUB_SYSTEM2_STATUS_ADDR   (102)   //子系统2状态

#define SYSTEM_CTRL_ADDR          (200)   //系统控制
#define SUB_SYSTEM_CTRL_ADDR      (201)   //子系统控制
//子系统切入切出地址
#define SUB_SYSTEM1_SWITCH_ADDR   (211)   //子系统1切入切出
#define SUB_SYSTEM2_SWITCH_ADDR   (212)   //子系统2切入切出
 
#define SUB_SYSTEM_IN             (0x00)  //子系统切入
#define SUB_SYSTEM_OUT            (0x01)  //子系统切出


//系统功率
#define SYSTEM_ACTIVE_ADDRL       (228)   //系统有功低
#define SYSTEM_ACTIVE_ADDRH       (229)   //系统有功高
#define SUB_SYSTEM1_ACTIVE_ADDRL  (232)   //子系统1有功低
#define SUB_SYSTEM1_ACTIVE_ADDRH  (233)   //子系统1有功高
#define SUB_SYSTEM2_ACTIVE_ADDRL  (236)   //子系统2有功低
#define SUB_SYSTEM2_ACTIVE_ADDRH  (237)   //子系统2有功高

#define LOG_PRINTF_CNT            (5)     //打印次数限制  
#define CONNECT_DELAY_TIME        (5)     //socket再次连接延时时间  

//通用类型
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;
typedef signed   char  INT8S;
typedef unsigned short INT16U;
typedef signed   short INT16S;
typedef unsigned int   INT32U;
typedef signed   int   INT32S;
typedef float          FP32;
typedef double         FP64;
typedef long long int  INT64U;

//16位2进制表示方式
typedef union
{
    struct
    {
        INT16U b0:1;
        INT16U b1:1;
        INT16U b2:1;
        INT16U b3:1;
        INT16U b4:1;
        INT16U b5:1;
        INT16U b6:1;
        INT16U b7:1;
        INT16U b8:1;
        INT16U b9:1;
        INT16U b10:1;
        INT16U b11:1;
        INT16U b12:1;
        INT16U b13:1;
        INT16U b14:1;
        INT16U b15:1;
    }bit;
    INT16U data;
} u16_bit_conv;

//16位高低位表示方式
typedef union
{
     INT8U D8[2];
     INT16U D16;
     INT16S DS16;
} u16_conv;

//32位ABCD或高低位表示方式
typedef union
{
    INT8U D8[4];
    INT16U D16[2];
    INT16S DS16[2];
    INT32U D32;
    INT32S DS32;
}u32_conv;

//烟感高低报警值
enum enumRow
{
    FireFailure       = 55002,
    Fire1Failure,
    Fire2Failure,
    Flooding1Failure,
    Flooding2Failure,
    H2_COLowFailure   = 55012,
    H2_COHighFailure,
};

//////////////软件版本号，分别为年、月、日、当日第几版////////////////
#define software_year           (0x22)
#define software_month          (0x01)
#define software_day            (0x23)
#define software_num            (0x01)

//////////////软件版本号，分别为年、月、日、当日第几版  出厂版本V1.00////////////////
#define software1               (0x56)
#define software2               (0x31)
#define software3               (0x2e)
#define software4               (0x30)
#define software5               (0x30)

//////////////协议版本号 ，分别为年、月、日、当日第几版////////////////
#define protocol_year           (0x21)
#define protocol_month          (0x06)
#define protocol_day            (0x30)
#define protocol_num            (0x01)

#endif //SYSTEM_H

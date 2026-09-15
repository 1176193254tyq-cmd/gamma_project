/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : timer.h
 * Project :
 * Description : 定时器，产生定时信号
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 添加cpp适配宏，新增超时判定宏和超时时间上限宏
 */

#ifndef TIMER_H
#define TIMER_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 获取tick
 * @return 系统TICK
 */
uint64_t Timer_GetTick();

/**
 * @brief 复位指定的定时器
 * @param[in] num 定时器序号
 */
void Reset_Timer(unsigned short num);

/**
 * @brief 复位所有定时器
 */
void Reset_All_Timer();

/**
 * @brief  信号回调函数
 * @note 会遇到while(1)中usleep被SIGALRM打断，导致Timer_Callback被调用，此时运行在Timer_MainTask线程
 * @param[in] signo : 信号
 */
void Timer_Callback(int signo);

/**
 * @brief 初始化定时器，定时CHECK_TICK_TIME ms, CHECK_TICK_TIME ms产生一次信号
 */
void Timer_Init();


/***************************************定时器定义************************************/

//通讯状态设置函数声明类型
typedef void (*COMM_SET)(unsigned char dev_num, unsigned char status,unsigned char isreset);
//通讯状态获取函数声明类型
typedef unsigned char (*COMM_GET)(unsigned char dev_num);

//!定时器单位
#define TICK_TIME               (10)      //每个tick的时间，单位ms
#define TIMER_CNT               (300)     //定义定时器数量

//!超时时间
#define WARN_FAULT_TIMEOUT_CNT  (5)       //故障告警检测周期（WARN_FAULT_TIMEOUT_CNT * 10ms）
#define PCS_TIMEOUT_CNT         (3000)    //PCS通信超时时间(PCS 20s判定离线，2000*10ms)
#define BMS_TIMEOUT_CNT         (18000)   //BMS通信超时时间(BMS 20s判定离线，2000*10ms)
#define RACK_TIMEOUT_CNT        (10000)    //RACK通信超时时间(100s判定离线，10000*10ms)
#define DI_TIMEOUT_CNT          (1000)    //DI转换器(E1210)通信超时时间(10s判定离线，1000*10ms)
#define DIDO_TIMEOUT_CNT        (1000)    //DIDO转换器(E1214)通信超时时间(10s判定离线，1000*10ms)
#define GW_TIMEOUT_CNT          (1000)    //GW通信超时时间(10s判定离线，1000*10ms)
#define ETH485_TIMEOUT_CNT      (9000)    //以太网转485透传设备(Nport5430)通信超时时间(METER 90s判定离线，9000*10ms)
#define METER_TIMEOUT_CNT       (3000)    //电表通信超时时间(30s判定离线，3000*10ms)
#define AIR_TIMEOUT_CNT         (9000)    //空调通信超时时间(90s判定离线，9000*10ms)
#define UPS_TIMEOUT_CNT         (3000)    //ups通信超时时间(30s判定离线，3000*10ms)
#define TEMP_HUMI_TIMEOUT_CNT   (3000)    //温湿度传感器通信超时时间(METER 30s判定离线，3000*10ms)
#define MEASU_TIMEOUT_CNT       (12000)    //测控装置通信超时时间(METER 30s判定离线，3000*10ms)
#define ZERO_TIMEOUT_CNT        (6000)    //零功率超时时间(60s，6000*10ms)
#define GAS_TIMEOUT_CNT         (3000)    //气体传感器通信超时时间(30s判定离线，3000*10ms)
#define EMS_TIMEOUT_CNT         (30000)    //EMS通信超时时间(180s判定离线，18000*10ms)
#define EMSP1_TIMEOUT_CNT       (60000)    //EMSP1通信超时时间(600s判定离线，60000*10ms)
#define SYS_CNT                 (100)     //系统定时器(1s一次，100*10ms)
#define SYSTEM_CNT              SYS_CNT 
#define AUTO_SCAN_CNT           (300)     //定时充放电时段扫描周期(3s一次，300*10ms)
#define SYN_CNT                 (3600 * 100)  //系统定时器(1h一次，3600*100*10ms)

#define UPS_30s_CNT             (30 * 100)  //系统定时器(30s一次，30 * 100ms)
#define General_power_CNT       (25 * 100)  //系统定时器(30s一次，30 * 100ms)

//!定时器设备定义，从0到CHECK_TIMER_CNT(300)的范围内容设定
//0-10，预留给单个设备；11之后为连续的设备定时器
#define FALUT_CHECK_TIMER       (0)      // 0 故障/告警检测定时器
#define SYSTEM_TIMER            (1)      // 1 系统时钟定时器
#define SCHEDULE_TIMER          (2)      // 2 定时充放电检测定时器
//3-10保留
#define TIME_SYN_TIMER          (11)     // 11-20   对时任务同步设备，11-1号设备
#define PCS_TIMER_BASE          (21)     // 21-30   10个PCS超时定时器，21-PCS1、22-PCS2
#define BMS_TIMER_BASE          (31)     // 31-40   10个BMS超时定时器，31-BMS1、32-BMS2
#define DI_TIMER_BASE           (41)     // 41-45   5个DI转换器(E1210)通信超时定时器
#define DIDO_TIMER_BASE         (46)     // 46-50   5个DIDO转换器(E1214)通信超时定时器
#define GW_TIMER_BASE           (51)     // 51-55   5个GW超时定时器
//56-60保留
#define DC_METER_TIMER_BASE     (61)     // 61-80   20个DC电表超时定时器
#define AC_METER_TIMER_BASE     (81)     // 81-100  20个AC电表超时定时器
#define ETH485_TIMER_BASE       (101)    // 101-120 20个ETH485链路超时定时器
#define UPS_TIMER_BASE          (121)    // 121-130 10个UPS超时定时器，121-UPS1、122-UPS2
#define AIRCTR_TIMER_BASE       (131)    // 131-180 50个空调超时定时器，131-空调1、132-空调2
#define TEMP_HUMI_TIMER_BASE    (181)    // 181-230 50个温湿度1超时定时器，181-温湿度1、182-温湿度2
#define ZERO_POWER_TIMER_BASE   (231)    // 231-240 10个子系统0功率超时定时器，211-1号子系统、212-2号子系统
#define MEASU_TIMER_BASE        (241)    // 241-250 10个测控通讯超时定时器，221-1号测控
#define GAS_TIMER_BASE          (251)    // 251-260 10个气体传感器定时器，231-1号气体传感器
#define EMS_TIMER_BASE          (261)    // 261-270 10个EMS定时器，241-1号支路EMS

#define UPS_30_TIMER_BASE        (271)    // 261-270 10个EMS定时器，241-1号支路EMS
#define General_power_TIMER_BASE (272)    // 261-270 10个EMS定时器，241-1号支路EMS

//!超时时间
#define TIMEOUT_50ms_CNT        (5)         //50毫秒检测周期, 5 * 10ms
#define TIMEOUT_1sec_CNT        (100)       //1秒检测周期, 100 * 10ms
#define TIMEOUT_2sec_CNT        (200)       //2秒检测周期, 200 * 10ms
#define TIMEOUT_3sec_CNT        (300)       //3秒检测周期, 300 * 10ms
#define TIMEOUT_10sec_CNT       (1000)      //10秒检测周期, 1000 * 10ms
#define TIMEOUT_15sec_CNT       (1500)      //15秒检测周期, 1500 * 10ms
#define TIMEOUT_25sec_CNT       (2500)      //25秒检测周期, 2500 * 10ms
#define TIMEOUT_30sec_CNT       (3000)      //30秒检测周期, 3000 * 10ms
#define TIMEOUT_60sec_CNT       (6000)      //60秒检测周期, 6000 * 10ms
#define TIMEOUT_90sec_CNT       (9000)      //90秒检测周期, 9000 * 10ms
#define TIMEOUT_2min_CNT        (12000)     //2分钟检测周期, 6000 * 2 * 10ms
#define TIMEOUT_1h_CNT          (360000)    //1小时检测周期, 60 * 60 * 100 * 10ms




//!定时器变量

extern volatile uint64_t TIMEOUT_TIMER[TIMER_CNT];               //定时器定义


#ifdef __cplusplus
}
#endif

#endif // TIMER_H

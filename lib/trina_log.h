/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : trina_log.h
 * Project : 本地控制器基础版代码
 * Description : 日志输出
 * File Created : 2022/11/11
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 添加cpp适配宏
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 新规
 */

#ifndef TRINA_LOG_H
#define TRINA_LOG_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "log.h"
#include "system.h"


/********************************************第三版：日志编号线程型日志输出************************************************/

//log标识结构体
typedef struct {
    char    log_flg[64];    //log标识，格式为filename-line_no-dev_no组成，因此限定文件名超过32个字符
} StTrinaLog;

//日志输出宏定义，根据日志标识进行判定，如果日志没有输出则输出，若果已输出则等60s再输出
#define TRINA_LOG(dev_no, format, ...)  do {            \
    if (AddLogFlag(dev_no, __LINE__, __FILE__) > 0) {   \
        LOG_INFO(format, ##__VA_ARGS__);                \
    }                                                   \
}while(0)
//日志缓存大小
#define TRINA_LOG_LIST_SIZE     (256)
//定时器缓存
extern volatile INT32U LOG_FLAG_CLEAR_TIMER;
//超时时间
#define TIMEOUT_60sec_CNT   (6000)  //日志清理周期(60s一次)


/**
 * @brief 更新指定的定时器
 */
void Update_Log_Timer();

/**
 * @brief 初始化日志数组
 */
void  InitLogList();

/**
 * @brief 新增日志
 * @param[in] dev_no 设备序号
 * @param[in] line_no 日志行号
 * @param[in] file_name 文件名称
 * @return 大于0是新增日志，小于0是已打印日志
 */
int AddLogFlag(INT8U dev_no, INT16U line_no, const char* file_name);

/**
 * @brief 天合日志输出线程
 */
void TrinaLog_Output_Task();

void format_hex_prefix(const INT8U* buf, size_t len, size_t n,
                                     char* out, size_t out_sz);
#if 0
/********************************************第二版：定时器限定型日志输出************************************************/

/**
 * @brief 更新指定的定时器
 * @param[in] num 定时器序号
 */
void Update_Debug_Timer(INT16U num);

/**
 * @brief 复位指定的定时器
 * @param[in] num 定时器序号
 */
void Reset_Debug_Timer(INT16U num);

/**
 * @brief 复位所有定时器
 */
void Reset_All_Debug_Timer();

//调试信息定时器定义
#define DEBUG_TIMER_CNT         (1024)   //调试信息定时器数量
extern volatile INT32U DEBUG_TIMER[DEBUG_TIMER_CNT];

//日志信息超时时间
#define LOG_PRINT_TIMEOUT_CNT       (50)        //通用日志信息打印周期(50ms一次, 50*10ms)
#define LOG_INFO_TIMEOUT_CNT        (1000)      //通用日志信息打印周期(10s一次, 500*10ms)
#define LOG_DEBUG_TIMEOUT_CNT       (5000)      //调试日志信息打印周期(50s一次, 1000*10ms)
#define RECONNECT_TIMEOUT_CNT       (6000)      //网络重连日志信息打印周期(60s一次, 6000*10ms)

//日志信息定时器设备定义，从0到DEBUG_TIMER_CNT的范围内容设定
//0-10, 预留给临时调试
#define LOG_INFO_TIMER              (0)     // 通用日志信息定时器
//每个设备对应的通讯重连日志信息
#define PCS_RECONNECT_TIMER         (11)    // 11-20   10个PCS网络重连日志信息定时器
#define BMS_RECONNECT_TIMER         (21)    // 21-30   10个BMS网络重连日志信息定时器
#define DI_RECONNECT_TIMER          (31)    // 31-35   5个DI网络重连日志信息定时器
#define DIDO_RECONNECT_TIMER        (36)    // 36-40   5个DIDO网络重连日志信息定时器
#define GW_RECONNECT_TIMER          (41)    // 41-45   5个GW网络重连日志信息定时器
#define ETH485_RECONNECT_TIMER      (46)    // 46-50   5个ETH485链路网络重连日志信息定时器
#define PCS_MSG_TIMER               (51)    // 51-70   20个PCS日志信息定时器
#define SYSTEM_MSG_TIMER            (71)    // 71-90   20个子系统日志定时器

//带定时器判断的宏，定时器值为0时和超时时间判定成功时，会继续打印日志信息
#define TRINA_TIMER_LOG(timer, timeout, format, ...)     do { \
    if (((Timer_GetTick() - DEBUG_TIMER[timer]) >= timeout) || (DEBUG_TIMER[timer] == 0)) \
    {   \
        LOG_INFO(format, ##__VA_ARGS__);    \
        Update_Debug_Timer(timer);  \
    }   \
}while(0)
//通用日志信息定时器，log_num编号预留了10位，不要和项目中其他地方的值相同，避免定时器时间冲突
#define TRINA_PRINT_TIMER_LOG(log_num, format, ...)     TRINA_TIMER_LOG((LOG_INFO_TIMER + log_num), LOG_PRINT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define TRINA_INFO_TIMER_LOG(log_num, format, ...)      TRINA_TIMER_LOG((LOG_INFO_TIMER + log_num), LOG_INFO_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define TRINA_DBG_TIMER_LOG(log_num, format, ...)       TRINA_TIMER_LOG((LOG_INFO_TIMER + log_num), LOG_DEBUG_TIMEOUT_CNT, format, ##__VA_ARGS__)
//网络日志重连定时器
#define PCS_CONNECT_TIMER_LOG(pcs_num, format, ...)     TRINA_TIMER_LOG((PCS_RECONNECT_TIMER + pcs_num), RECONNECT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define BMS_CONNECT_TIMER_LOG(bms_num, format, ...)     TRINA_TIMER_LOG((BMS_RECONNECT_TIMER + bms_num), RECONNECT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define DI_CONNECT_TIMER_LOG(di_num, format, ...)       TRINA_TIMER_LOG((DI_RECONNECT_TIMER + di_num), RECONNECT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define DIDO_CONNECT_TIMER_LOG(dido_num, format, ...)   TRINA_TIMER_LOG((DIDO_RECONNECT_TIMER + dido_num), RECONNECT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define GW_CONNECT_TIMER_LOG(gw_num, format, ...)       TRINA_TIMER_LOG((GW_RECONNECT_TIMER + gw_num), RECONNECT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define ETH485_CONNECT_TIMER_LOG(eth_num, format, ...)  TRINA_TIMER_LOG((ETH485_RECONNECT_TIMER + eth_num), RECONNECT_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define PCS_MSG_LOG(msg_num, pcs_num, format, ...)      TRINA_TIMER_LOG((PCS_MSG_TIMER + msg_num + pcs_num*10), LOG_INFO_TIMEOUT_CNT, format, ##__VA_ARGS__)
#define SYS_MSG_LOG(msg_num, sub_num, format, ...)      TRINA_TIMER_LOG((PCS_MSG_TIMER + msg_num + sub_num*10), LOG_INFO_TIMEOUT_CNT, format, ##__VA_ARGS__)


/********************************************第一版：数量统计型日志输出************************************************/
//线程锁，强制同步全局变量数据
static pthread_mutex_t MS_TRINA_LOG_MUTEX = PTHREAD_MUTEX_INITIALIZER;
//日志的默认值
#define TRINA_LOG_MSG_LIST     (5)         //LOG列表的容量
#define TRINA_LOG_MSG_LEN      (1024)      //单条LOG的长度
#define TRINA_LOG_PRINT_COUNT  (100)       //同种log禁止输出的最大极限
//日志的缓存区，使用static可以避免多个文件间缓存的冲突
static char MS_SRC_BUFF[TRINA_LOG_MSG_LEN] = {0};                             //源标字符串缓存区
static char MS_DST_BUFF[TRINA_LOG_MSG_LIST][TRINA_LOG_MSG_LEN] = {{0}};       //目标字符串缓存区
static volatile INT32U MS_TRINA_LOG_COUNT[TRINA_LOG_MSG_LIST] = {0};    //同种log内容统计值

//通知日志新编宏
#define TRINA_LOG_INFO(format, ...)    do { \
    /* 将日志内容存入源地址缓存中 */ \
    pthread_mutex_lock(&MS_TRINA_LOG_MUTEX); \
    memset(MS_SRC_BUFF, 0, TRINA_LOG_MSG_LEN); \
    sprintf(MS_SRC_BUFF, format, ##__VA_ARGS__); \
    int MS_LOG_STRCMP_COUNT = 0; \
    int MS_LOG_SUM_COUNT = 0; \
    /* 校验日志是否重复输出，并校验重复输出的日志是否超限 */  \
    for (int i = 0; i < TRINA_LOG_MSG_LIST; i++) { \
        if (strcmp(MS_DST_BUFF[i], MS_SRC_BUFF) == 0) { \
            MS_LOG_STRCMP_COUNT++; \
            if (MS_TRINA_LOG_COUNT[i]++ > TRINA_LOG_PRINT_COUNT) { \
                MS_TRINA_LOG_COUNT[i] = 0; \
                MS_LOG_SUM_COUNT++; \
            } \
            break; \
        } \
    } \
    /* 将新增日志追加到缓存队列中 */ \
    if (MS_LOG_STRCMP_COUNT == 0) { \
        for (int i = TRINA_LOG_MSG_LIST; (i - 1) >= 0; i--) { \
            memset(MS_DST_BUFF[i], 0, TRINA_LOG_MSG_LEN); \
            strcpy(MS_DST_BUFF[i], MS_DST_BUFF[i - 1]); \
        } \
        memset(MS_DST_BUFF[0], 0, TRINA_LOG_MSG_LEN); \
        strcpy(MS_DST_BUFF[0], MS_SRC_BUFF); \
        MS_TRINA_LOG_COUNT[0] = 0; \
    } \
    pthread_mutex_unlock(&MS_TRINA_LOG_MUTEX); \
    /* 输出新增日志和超限日志 */ \
    if (MS_LOG_STRCMP_COUNT == 0 || MS_LOG_SUM_COUNT != 0) { \
        LOG_INFO("%s", MS_SRC_BUFF); \
    } \
}while(0)

#endif

#ifdef __cplusplus
}
#endif

#endif // TRINA_LOG_H

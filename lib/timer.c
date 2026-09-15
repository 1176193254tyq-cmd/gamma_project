/*
 * Copyright (C) 2022
 * File Name: timed.c
 * Project:
 * Description: 定时器、超时判定
 * File Created:
 * Author: wujinjin
 * -----
 * Last Modified:
 * Modified By:
 * -----
 * HISTORY:
 */

#include "main.h"
#include "timer.h"
#include "trina_log.h"
#include <sys/sysinfo.h>
//!定时器变量
volatile INT32U Timer_Tick = 0;                              //系统TICK
volatile uint64_t TIMEOUT_TIMER[TIMER_CNT] = {0};              //超时检测定时器定义
volatile uint64_t sys_10ms_cnt=0;                           //软件定时器计数值
// static struct timeval system_start_time={0};        
#define TIMER_U32_DIFF(a, b)    (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0xFFFFFFFF) + 1)))
#define TIMER_U16_DIFF(a, b)    (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0xFFFF) + 1)))
#define TIMER_DIFF_1(a,b)       (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0x7FFF) + 1)))   





#define   GDB_DEBUG           //只能在调试模式下使用，因为同步时间的时候，相关时间变量会受影响





uint64_t Timer_GetTick()
{
    uint64_t time_tick=0;
    struct timeval sys_time;
    gettimeofday(&sys_time,NULL);
    sysPara* sys_cfg = SysConf_GetInfo();
    // if(sys_cfg->debugFlag)
    // {
    //     // time_tick=(sys_time.tv_sec-system_start_time.tv_sec)*100+(sys_time.tv_usec-system_start_time.tv_usec)/10000;
    //     time_tick=sys_time.tv_sec*100+sys_time.tv_usec/10000;
    // }
    // else
    {
        time_tick=sys_10ms_cnt;
    }
    return time_tick;    //10ms精度   
}

void Reset_Timer(unsigned short num)
{
    TIMEOUT_TIMER[num] = Timer_GetTick();
}

void Reset_All_Timer()
{
    INT32U i = 0;
    for(i = 0; i < TIMER_CNT; i++)
    {
        TIMEOUT_TIMER[i] = Timer_GetTick();
    }
}

void Timer_Callback(int signo)
{
    if(signo == SIGALRM)
    {
        // Timer_Tick++;
        sys_10ms_cnt++;             //软件定时器的计数值    
        // if(Timer_Tick > 0xFFFF0000) //避免tick溢出，定时器失效
        // {
        //     Timer_Tick = 0;
        //     Reset_All_Timer();
        //     // Reset_All_Debug_Timer();
        //     LOG_INFO("system time tick reset");
        // }
    }
}

void Timer_Init()
{
    uint64_t time_tick=0;
    sysPara* sys_cfg = SysConf_GetInfo();
    if(sys_cfg->debugFlag)
    {

    }
    else
    {
        signal(SIGALRM, Timer_Callback);
        struct itimerval value, ovalue;
        value.it_value.tv_sec = 0;
        value.it_value.tv_usec = TICK_TIME * 1000;
        value.it_interval = value.it_value;
        setitimer(ITIMER_REAL, &value, &ovalue);

    }
}


//*****
//*****返回1时间未到，返回0时间到了
INT8S Soft_Timer(uint64_t last_time,uint64_t check_time_ms)
{
    uint64_t now_time = Timer_GetTick();
    if(now_time<last_time)
    {
        return 0;
    }
    else if((now_time-last_time)<(check_time_ms/10))
    {
        return 1;
    }
    return 0;
}

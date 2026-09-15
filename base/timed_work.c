/* ------------------------------------------------------------
* Copyright (C) 2022
* File Name : timed.c
* Project :
* Description : 定时器、超时判定
* File Created :
* Author : wujinjin
* ------------------------------------------------------------
* Last Modified :
* Modified By :
* ------------------------------------------------------------
* HISTORY :
*/

#include "main.h"
#include "timed_work.h"

static char IsCheckTime = 0;
char* const pIsCheckTime = &IsCheckTime;
/**
 * @brief  time_section_test() -  检测当前时间所在时段
 * @param[in] h  - 当前时间小时
 * @param[in] m  - 当前时间分钟
 * @param[in] p  - 时段设置
 * @return 当前时间在开始时间和结束时间之间时返回1，否则返回0
 */
static unsigned char time_section_test(unsigned short h, unsigned short m, period* p)
{
//当前小时和分钟,根据实际给出
unsigned short strDateH = h;
unsigned short strDateM = m;

//开始时间
unsigned short strDateBeginH = p->bh;      //开始小时
unsigned short strDateBeginM = p->bm;      //开始分钟

//结束时间
unsigned short strDateEndH = p->eh;        //结束小时
unsigned short strDateEndM = p->em;        //结束分钟

//跨天
if(strDateEndH < strDateBeginH)
{
if((strDateH > strDateEndH) && (strDateH < strDateBeginH))
{
return 0;
}
else if((strDateH == strDateEndH) && (strDateH < strDateBeginH) && (strDateM > strDateEndM))
{
return 0;
}
else if((strDateH > strDateEndH) && (strDateH == strDateBeginH)    && (strDateM < strDateBeginM))
{
return 0;
}
else
{
return 1;
}
}
else
{
if((strDateH > strDateBeginH) && (strDateH < strDateEndH))
{
return 1;
}
else if((strDateH == strDateBeginH) && (strDateH < strDateEndH) && (strDateM >= strDateBeginM))
{
return 1;
}
else if((strDateH > strDateBeginH) && (strDateH == strDateEndH) && (strDateM <= strDateEndM))
{
return 1;
}
else if((strDateH == strDateBeginH) && (strDateH == strDateEndH) && (strDateM >= strDateBeginM) && (strDateM <= strDateEndM))
{
return 1;
}
else
{
return 0;
}
}

return 0;
}
static long MSecs = 0;
/**
 * @brief 获取时间戳
 */
long GetMSecsSinceEpoch()
{
return MSecs;
}
/**
 * @brief 计算时间戳
 */
long GetMSecsSinceloop()
{
struct timeb timestamp = {};

if (0 == ftime(&timestamp))
return timestamp.time * 1000 + timestamp.millitm;
else
return 0;
}
/**
 * @brief 定时任务集合
 */
void Task_Timed_Work(void)
{
int i = 0, ret = 0;
char longBuf[64] = {0}, shortBuf[16] = {0};
time_t timer = 0;
static unsigned short second_cnt = 0; 
struct tm* tblock = NULL;
static unsigned char current_period = 0, current_period_pre = 0;
u32_conv Datatemp1;
sysPara* sys_cfg = SysConf_GetInfo();
LOG_INFO("system timed task start");
IsCheckTime = 0;
while(1)
{
//系统心跳，1s周期
if((Timer_GetTick() - TIMEOUT_TIMER[SYSTEM_TIMER]) >= SYSTEM_CNT) //100*10ms
{
SystemTotalData[SYS_HEART_BEAT_ADDR].D16++;
if(second_cnt++>=3600)
{
second_cnt=0;
SystemTotalData[SYS_RUN_TIME_ADDR].D16++;  //7年后回头    
}
TIMEOUT_TIMER[SYSTEM_TIMER] = Timer_GetTick();
timer = time(NULL);  //直接就是时间戳 time_t timer
tblock = localtime(&timer); //struct tm *tblock; 变成2022-07-25 17:23:12
SystemTotalData[SYS_SYNC_TIEM_ADDR].D16 = tblock->tm_year + 1900;
SystemTotalData[SYS_SYNC_TIEM_ADDR+1].D16 = tblock->tm_mon + 1;
SystemTotalData[SYS_SYNC_TIEM_ADDR+2].D16 = tblock->tm_mday;
SystemTotalData[SYS_SYNC_TIEM_ADDR+3].D16 = tblock->tm_hour;
SystemTotalData[SYS_SYNC_TIEM_ADDR+4].D16 = tblock->tm_min;
SystemTotalData[SYS_SYNC_TIEM_ADDR+5].D16 = tblock->tm_sec;
}

//对时任务
if((IsCheckTime == 1)||((Timer_GetTick() - TIMEOUT_TIMER[TIME_SYN_TIMER]) >= SYSTEM_CNT)) //1h
{
(*pIsCheckTime) = 0;
timer = time(NULL);  //直接就是时间戳 time_t timer
tblock = localtime(&timer); //struct tm *tblock; 变成2022-07-25 17:23:12
for(i = 0; (i < sys_cfg->bmsNum) && (i < MAX_BMS_NUM) && (tblock != NULL); i++)
{
SystemTotalData[10182 + i * 1000].D16 = tblock->tm_year + 1900;
SystemTotalData[10183 + i * 1000].D16 = tblock->tm_mon + 1;
SystemTotalData[10184 + i * 1000].D16 = tblock->tm_mday;
SystemTotalData[10185 + i * 1000].D16 = tblock->tm_hour;
SystemTotalData[10186 + i * 1000].D16 = tblock->tm_min;
SystemTotalData[10187 + i * 1000].D16 = tblock->tm_sec;
}
TIMEOUT_TIMER[TIME_SYN_TIMER] = Timer_GetTick();
}
MSecs = GetMSecsSinceloop();

usleep(50 * 1000); //10ms
}
}
/**
 * @brief 获取此时系统时间
 */
const char* get_current_time() {
static char buffer[26]; 
time_t t;
struct tm *tm_info;

t = time(NULL);            // 获取当前时间
tm_info = localtime(&t);   // 将时间转为本地时间

// 格式化时间
strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

return buffer; // 返回静态数组的地址
}
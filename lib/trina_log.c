/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : trina_log.h
 * Project : 本地控制器基础版代码
 * Description : 日志输出
 * File Created : 2023/01/08
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/01/08
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/01/08 新规
 */

#include "main.h"
#include "timer.h"
#include "trina_log.h"


/********************************************第三版：日志编号线程型日志输出************************************************/

static StTrinaLog TRINA_LOG_LIST[TRINA_LOG_LIST_SIZE];                  //日志数组缓存
static volatile INT32U log_list_pos = 0;                              //日志数组定位
static pthread_mutex_t TRINA_LOG_MUTEX = PTHREAD_MUTEX_INITIALIZER;     //日志数组变更锁
volatile INT32U LOG_FLAG_CLEAR_TIMER = 0;                             //设备定时器定义
#define FileName(name) (strrchr(name,'/') ? strrchr(name,'/')+1 : name) //根据__FILE__只输出文件名


void Update_Log_Timer()
{
    LOG_FLAG_CLEAR_TIMER = Timer_GetTick();
}

void  InitLogList()
{
    pthread_mutex_lock(&TRINA_LOG_MUTEX);
    memset(&TRINA_LOG_LIST, 0, TRINA_LOG_LIST_SIZE * sizeof(StTrinaLog));
    log_list_pos = 0;
    pthread_mutex_unlock(&TRINA_LOG_MUTEX);
}

int AddLogFlag(INT8U dev_no, INT16U line_no, const char* file_name)
{
    volatile int retval = 0;
    pthread_mutex_lock(&TRINA_LOG_MUTEX);

    //初始化
    StTrinaLog new_dat;
    memset(&new_dat, 0, sizeof(StTrinaLog));

    //文件名、行号、设备号，组合成log标识
    sprintf(new_dat.log_flg, "%s-%d-%d", FileName(file_name), line_no, dev_no);

    if (log_list_pos == 0) {
        //新增日志
        TRINA_LOG_LIST[0] = new_dat;
        log_list_pos++;
        retval = 1;
    }
    else {
        //查询日志是否存在
        INT32U i = 0;
        for (; i < log_list_pos; i++) {
            //比较两个log的标识是否相同，相同则计数累加
            if (strcmp(TRINA_LOG_LIST[i].log_flg, new_dat.log_flg) == 0) {
                break;
            }
        }
        //新增日志
        if (i >= log_list_pos) {
            //缓存空余，新增日志
            if ((log_list_pos+1) < TRINA_LOG_LIST_SIZE) {
                TRINA_LOG_LIST[log_list_pos++] = new_dat;
            }
            //特殊处理，清空缓存再新增
            else {
                InitLogList();
                TRINA_LOG_LIST[log_list_pos++] = new_dat;
            }
            retval = 1;
        }
        else {
            retval = -1;
        }
    }

    pthread_mutex_unlock(&TRINA_LOG_MUTEX);
    return retval;
}

void TrinaLog_Output_Task()
{
    LOG_INFO("trina log output task start");
    ThreadConfirm(__func__);

    while(1)
    {
        //日志标识清理周期(60s一次)
        if ((Timer_GetTick() - LOG_FLAG_CLEAR_TIMER) >= TIMEOUT_60sec_CNT) {
            InitLogList();
            Update_Log_Timer();
        }

        //轮询周期
        usleep(1000);
    }
}
void format_hex_prefix(const INT8U* buf, size_t len, size_t n,
                                     char* out, size_t out_sz)
{
    size_t want = (len < n) ? len : n;
    size_t off = 0;
    for (size_t i = 0; i < want && off + 3 < out_sz; ++i) {
        off += snprintf(out + off, out_sz - off, "%02X ", buf[i]);
    }
    if (len > want && off + 4 < out_sz) {
        off += snprintf(out + off, out_sz - off, "...");
    }
    if (off < out_sz) out[off ? off - 1 : 0] = '\0'; // 去掉最后一个空格（可选）
}


#if 0
/********************************************第二版：定时器限定型日志输出************************************************/

volatile INT32U DEBUG_TIMER[DEBUG_TIMER_CNT] = {0};                 //调试信息定时器定义

void Update_Debug_Timer(INT16U num)
{
    DEBUG_TIMER[num] = Timer_GetTick();
}

void Reset_Debug_Timer(INT16U num)
{
    DEBUG_TIMER[num] = 0;
}

void Reset_All_Debug_Timer()
{
    INT32U i = 0;
    for (i = 0; i < DEBUG_TIMER_CNT; i++)
    {
        DEBUG_TIMER[i] = Timer_GetTick();
    }
}
#endif

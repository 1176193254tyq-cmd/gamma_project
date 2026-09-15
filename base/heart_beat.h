// hb.h
#pragma once
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include "system.h"

typedef struct HBHandle {
    // 固定参数
    INT16U hb_reg_addr;
    int      period_ms;

  
    int      socket_Bank;   
    int      Bank_id;

    // 线程控制
    pthread_t th;
    pthread_mutex_t mtx;
    pthread_cond_t  cv;
    bool running;
    bool paused;
    bool reset_deadline;

 
    INT8U hb;

 
    int bms_num; 
    int pcs_num;
} HBHandle;

HBHandle* hb_create(int bms_num, int period_ms, INT16U hb_reg_addr);
void       hb_destroy(HBHandle* h);
void* hb_thread(void* arg);
// 生命周期与控制
typedef void* (*HBThreadFn)(void*);

int hb_start(HBHandle* h, int socket_Bank, int Bank_id, HBThreadFn thread_fn);
void hb_stop(HBHandle* h);
void hb_pause(HBHandle* h);
void hb_resume(HBHandle* h);
void add_ms(struct timespec* t, int ms);
// 运行中更新socket/Bank（例如重连后切换fd）
void hb_update_channel(HBHandle* h, int new_socket_Bank, int new_Bank_id);

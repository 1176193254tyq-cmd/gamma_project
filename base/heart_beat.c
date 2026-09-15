/* ------------------------------------------------------------
* Copyright (C) 2026
* File Name : heart_beat.c
* Project :
* Description : 心跳线程控制
* File Created :tyq

* ------------------------------------------------------------

* ------------------------------------------------------------
* HISTORY : 
*/
#define _POSIX_C_SOURCE 200809L
#include "time.h"
#include "heart_beat.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "main.h"


void add_ms(struct timespec* t, int ms){
    t->tv_sec  += ms/1000;
    long ns = t->tv_nsec + (long)(ms%1000)*1000000L;
    if (ns >= 1000000000L){ t->tv_sec += 1; ns -= 1000000000L; }
    t->tv_nsec = ns;
}

void* hb_thread(void* arg){
    HBHandle* h = (HBHandle*)arg;

    struct timespec next;
    clock_gettime(CLOCK_MONOTONIC, &next);//获取当前时间
    add_ms(&next, h->period_ms);//增加指定时间

    for(;;){
        // —— 暂停/退出判定 ——
        pthread_mutex_lock(&h->mtx);//互斥加锁，保护共享资源
        while (h->paused && h->running){//当心跳被停止并且线程还在运行中时
            pthread_cond_wait(&h->cv, &h->mtx);//线程休眠
        }
        bool running = h->running;
        bool need_reset = h->reset_deadline;//如果意外断开的话，重联之后继续从当前时间进行
        h->reset_deadline = false;
        // 读取当前通道
        int socket_Bank = h->socket_Bank;
        int Bank_id     = h->Bank_id;
        pthread_mutex_unlock(&h->mtx);

        if (!running) break;

        if (need_reset){
            clock_gettime(CLOCK_MONOTONIC, &next);
            add_ms(&next, h->period_ms);
        }

        // —— 发送心跳（允许socket_Bank为无效时跳过） ——
        if (socket_Bank > 0){
            Modbus_TCP_Write_Heart(socket_Bank, Bank_id,
                                           0x06, h->hb_reg_addr, (INT16U)h->hb,
                                            200);
            h->hb++; // 0..255
        }

        // —— 周期对齐休眠 ——
        int e;
        do { e = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL); }//线程睡到1s钟，绝对时间
        while (e == EINTR);
        add_ms(&next, h->period_ms);
    }
    return NULL;
}
/*心跳任务创建 
设备编号
定时周期
寄存器地址
*/
HBHandle* hb_create(int num, int period_ms, INT16U hb_reg_addr){
    HBHandle* h = (HBHandle*)calloc(1, sizeof(HBHandle));
    if (!h) return NULL;
    h->period_ms   = (period_ms > 0 ? period_ms : 1000);
    h->hb_reg_addr = hb_reg_addr;
    h->bms_num     = num;
    h->pcs_num     = num; 
    h->socket_Bank = -1;
    h->Bank_id     = 1;
    pthread_mutex_init(&h->mtx, NULL);//创建线程互斥变量，保护共享变量资源，防止被其他线程更改
    pthread_cond_init(&h->cv, NULL);//创建线程条件变量
    return h;
}

void hb_destroy(HBHandle* h){
    if (!h) return;
    pthread_mutex_destroy(&h->mtx);
    pthread_cond_destroy(&h->cv);
    free(h);
}

int hb_start(HBHandle* h, int socket_Bank, int Bank_id, HBThreadFn thread_fn){
    if (!h) return -1;
    if (!thread_fn) thread_fn = hb_thread; // 允许传 NULL 使用默认 hb_thread

    pthread_mutex_lock(&h->mtx);
    if (h->running){ pthread_mutex_unlock(&h->mtx); return 0; }
    h->socket_Bank    = socket_Bank;
    h->Bank_id        = Bank_id;
    h->hb             = 0;
    h->paused         = false;
    h->reset_deadline = true;
    h->running        = true;
    pthread_mutex_unlock(&h->mtx);

    int rc = pthread_create(&h->th, NULL, thread_fn, h);
    if (rc != 0){
        pthread_mutex_lock(&h->mtx);
        h->running = false;
        pthread_mutex_unlock(&h->mtx);
        return -1;
    }
    return 0;
}

void hb_stop(HBHandle* h){
    if (!h) return;
    pthread_mutex_lock(&h->mtx);
    if (!h->running){ pthread_mutex_unlock(&h->mtx); return; }
    h->running = false;
    h->paused  = false;
    pthread_cond_broadcast(&h->cv);
    pthread_mutex_unlock(&h->mtx);
    pthread_join(h->th, NULL);
}

void hb_pause(HBHandle* h){
    if (!h) return;
    pthread_mutex_lock(&h->mtx);
    h->paused = true;
    pthread_mutex_unlock(&h->mtx);
}

void hb_resume(HBHandle* h){
    if (!h) return;
    pthread_mutex_lock(&h->mtx);
    h->paused = false;
    h->reset_deadline = true;
    pthread_cond_broadcast(&h->cv);
    pthread_mutex_unlock(&h->mtx);
}

void hb_update_channel(HBHandle* h, int new_socket_Bank, int new_Bank_id){
    if (!h) return;
    pthread_mutex_lock(&h->mtx);
    h->socket_Bank = new_socket_Bank;
    h->Bank_id     = new_Bank_id;
    h->reset_deadline = true; 
    pthread_mutex_unlock(&h->mtx);
}

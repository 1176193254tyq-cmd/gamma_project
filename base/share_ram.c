/*
 * Copyright (C) 2023
 * File Name: share_ram.c
 * Project:
 * Description:
 * File Created:
 * Author: lj
 * -----
 * HISTORY:
 */

 #include "main.h"
 #include "assist_func.h"
 #include "trina_log.h"
 #include "timed_check.h"
 #include "share_ram.h"
 #include "address_map.h"
 
 #include <sys/mman.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 // 共享内存文件路径
 #define LOGGER_SHARE_RAM_PATH "/my_shared_memory_logger"
 #define BBMS_SHARE_RAM_PATH "/my_shared_memory_bms"
 // 全局共享内存指针
 static RegistersModel *Ram_Logger = NULL;
 static RegistersModel *Ram_Bms = NULL;


 // 互斥锁用于多线程同步
 pthread_mutex_t ram_mutex = PTHREAD_MUTEX_INITIALIZER;
 // 共享内存初始化 - 电芯数据
void share_Bms_ram_init()
{
    int fd;
    const char *file_path = BBMS_SHARE_RAM_PATH;  // 使用与Python相同的路径

    // 打开共享内存文件（如果文件不存在则创建）
    fd = shm_open(file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("打开共享内存文件失败");
        exit(EXIT_FAILURE);
    }

    // 设置共享内存的大小
    int size = sizeof(RankModel);
    if (ftruncate(fd, size) == -1) {
        perror("设置共享内存大小失败");
        exit(EXIT_FAILURE);
    }

    // 将共享内存映射到当前进程的地址空间
    void *addrbms = (INT16U *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addrbms == MAP_FAILED) {
        perror("共享内存映射失败");
        close(fd);
        exit(EXIT_FAILURE);
    }

    Ram_Bms = (RankModel *)addrbms;
    printf("电芯共享内存映射成功\n");
}

 // 初始化共享内存（日志数据）
 void share_Logger_ram_init(void)
 {
     const char *file_path = LOGGER_SHARE_RAM_PATH;
     int fd = shm_open(file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
     if (fd == -1) {
         perror("无法打开共享内存文件");
         exit(EXIT_FAILURE);
     }
 
     // 设置共享内存大小
     int size = sizeof(RegistersModel);
     if (ftruncate(fd, size) == -1) {
         perror("无法设置共享内存大小");
         close(fd);
         exit(EXIT_FAILURE);
     }
 
     // 映射共享内存到进程地址空间
     void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
     if (addr == MAP_FAILED) {
         perror("共享内存映射失败");
         close(fd);
         exit(EXIT_FAILURE);
     }
 
     Ram_Logger = (RegistersModel *)addr;
     printf("日志共享内存映射成功\n");
 }
 
 // 共享内存任务循环
 void Task_share_ram(void)
 {
     // 等待系统初始化完成
     sleep(25);
 
     // 初始化共享内存
     share_Logger_ram_init();
     share_Bms_ram_init();
     // 获取系统配置和寄存器映射指针
     sysPara *sys_cfg = SysConf_GetInfo();
     INT16U *pRamReg = (INT16U *)pRegM;
     INT16U *pRamRank = (INT16U *)pRankM;
     int logger_total_lenth = sizeof(RegistersModel) / sizeof(INT16U);
     int bms_total_lenth=sizeof(RankModel) / sizeof(INT16U);
     while (1) {
         sleep(1);  // 周期性写入
 
         pthread_mutex_lock(&ram_mutex);
         for (int i = 0; i < logger_total_lenth; ++i) {
             ((INT16U *)Ram_Logger)[i] = pRamReg[i];//将除了rack的数据写入到共享内存中
         }
         pthread_mutex_unlock(&ram_mutex);


         pthread_mutex_lock(&ram_mutex);
         for (int i = 0; i < bms_total_lenth; ++i) {
             ((INT16U *)Ram_Bms)[i] = pRamRank[i];//将rack数据写入到共享内存中
         }
         pthread_mutex_unlock(&ram_mutex);
  
        //  for(int i=0 ;i<logger_total_lenth;i++)
        //  {
        //  LOG_INFO("ban数据[%d]=%d",i,Ram_Logger->Hold[i]);
        //  }
 

        // for(int i=0 ;i<bms_total_lenth;i++)
        // {
        // LOG_INFO("rack数据[%d]=%d",i,Ram_Bms->Input[i]);
        // }


     }
 }
 RegistersModel* get_logger_shared_ram()
 {
     return Ram_Logger;
 }

 RankModel* get_rack_shared_ram()
 {
     return Ram_Bms;
 }
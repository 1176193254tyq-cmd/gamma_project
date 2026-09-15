#ifndef CJSON_APP_H
#define CJSON_APP_H


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

#include "cJSON.h"
#include "main.h"
#define JOSN_FILE          "/home/moxa/sys_rom_data.json"                      //系统日志文件
//#define JOSN_FILE           "/home/normal/uc-8410a/test/hello/sys_rom_data.json"

typedef struct 
{
    int uint8_data[2];     //
    int uint16_data[3];   //     
    int uint32_data[4];   //
    double float_data[5];    //
    char *string_data;          // 
}sys_rom_data_str;


extern sys_rom_data_str g_sys_rom_data;

void Sys_Rom_Data_Init();
void Sys_Rom_Data_Check();
void Sys_Rom_Data_Task();
#endif
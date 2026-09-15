/*
 *  log.h
 *
 *  Created on: 2018-6-12
 *      Author: root
 */

#ifndef LOG_H_
#define LOG_H_

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "zlog.h"
#include <pthread.h>
extern pthread_mutex_t g_log_mutex ;
#define LOG_JUST_ERR(format,...) do\
{\
	char *d = strrchr((const char *)__FILE__, '/');\
	char time_str[27];	\
	time2str(time_str);\
	fprintf(stderr, "\r[\033[31mERR\033[0m] [%s] [%s:%d] " format "\r\n",           \
					             time_str, d ? d + 1 : __FILE__ , __LINE__, ##__VA_ARGS__);                    \
}while(0)

#define LOG_TO_FILE		1
#define LOG_TO_SHELL	2
#define LOG_TYPE_DEFINE        	LOG_TO_FILE

#if (LOG_TYPE_DEFINE == LOG_TO_SHELL)
#define LOG_INFO(format,...) do \
{\
	char *d = strrchr((const char *)__FILE__, '/');\
	char time_str[27];	\
	time2str(time_str);\
	fprintf(stdout, "\r[\033[32mINFO\033[0m] [%s] [%s:%d] " format "\r\n",                    \
					              time_str, d ? d + 1 : __FILE__ , __LINE__, ##__VA_ARGS__);                            \
	/*dzlog_info(format,##__VA_ARGS__);*/\
}while(0)

#define LOG_DBG(format,...) do\
{\
	char *d = strrchr((const char *)__FILE__, '/');\
	char time_str[27];	\
	time2str(time_str);\
    fprintf(stdout, "\r[DEBUG] [%s] [%s:%d] " format "\r\n",                    \
					              time_str, d ? d + 1 : __FILE__ , __LINE__, ##__VA_ARGS__);                            \
	/*dzlog_debug(format,##__VA_ARGS__);*/\
}while(0)
#define LOG_WARN(format, ...) do\
{\
	char *d = strrchr((const char *)__FILE__, '/');\
	char time_str[27];	\
	time2str(time_str);\
	fprintf(stdout, "\r[\033[33mWARN\033[0m] [%s] [%s:%d] " format "\r\n",                    \
					             time_str, d ? d + 1 : __FILE__  , __LINE__, ##__VA_ARGS__);                            \
	/*dzlog_warn(format,##__VA_ARGS__);*/\
}while(0)
#define LOG_ERR(format,...) do\
{\
	char *d = strrchr((const char *)__FILE__, '/');\
	char time_str[27];	\
	time2str(time_str);\
	fprintf(stderr, "\r[\033[31mERR\033[0m] [%s] [%s:%d] " format "\r\n",           \
					             time_str, d ? d + 1 : __FILE__ , __LINE__, ##__VA_ARGS__);                    \
	dzlog_error(format,##__VA_ARGS__);\
}while(0)

#else

#define LOG_INFO(format,...) do \
{ \
    pthread_mutex_lock(&g_log_mutex); \
    dzlog_info(format, ##__VA_ARGS__); \
    pthread_mutex_unlock(&g_log_mutex); \
} while(0)

#define LOG_DBG(format,...) do\
{\
	dzlog_debug(format,##__VA_ARGS__);\
}while(0)

#define LOG_WARN(format, ...) do\
{\
	dzlog_warn(format,##__VA_ARGS__);\
}while(0)

#define LOG_ERR(format,...) do\
{\
	dzlog_error(format,##__VA_ARGS__);\
}while(0)

#endif

#define IR_LOG_INFO(format,...) do \
{\
	zlog_info(ir_log,format,##__VA_ARGS__);\
}while(0)

#ifdef __cplusplus
}
#endif

#endif /* LOG_H_ */


#pragma once
#include <mysql.h>
#include <stdbool.h>
#include <stdint.h>
#include <netinet/in.h>
#include "system.h"

typedef struct {
    int  role_id;
    bool allow_plain;
} AclResult;

int  acl_db_global_init(void);                     // 进程启动时调用（一次）
void acl_db_global_cleanup(void);                  // 进程退出时调用（一次）

int  acl_db_connect(const char* host, unsigned port,
                    const char* user, const char* pass,
                    const char* dbname);           // 配置阶段调用（连上DB）
void acl_db_disconnect(void);

bool acl_check_sockaddr(const struct sockaddr* sa, socklen_t salen,
                        AclResult* out);           // 白名单判断

// 把 IPv4/IPv6 sockaddr 转 16字节（IPv4→::ffff:a.b.c.d）
void sockaddr_to_ipv6_16(const struct sockaddr* sa, INT8U out16[16]);

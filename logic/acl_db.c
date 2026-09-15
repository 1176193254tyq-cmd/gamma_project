// acl_db.c
#include "acl_db.h"
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

static MYSQL       g_mysql;
static MYSQL_STMT* g_stmt = NULL;
static pthread_mutex_t g_mtx = PTHREAD_MUTEX_INITIALIZER;
static bool g_inited = false;
static bool g_connected = false;

int acl_db_global_init(void){
    if (g_inited) return 0;
    if (mysql_library_init(0, NULL, NULL) != 0) return -1;
    g_inited = true;
    return 0;
}
void acl_db_global_cleanup(void){
    if (g_stmt) { mysql_stmt_close(g_stmt); g_stmt = NULL; }
    if (g_connected){ mysql_close(&g_mysql); g_connected=false; }
    if (g_inited){ mysql_library_end(); g_inited=false; }
}

int acl_db_connect(const char* host, unsigned port,
                   const char* user, const char* pass,
                   const char* dbname)
{
    if (!g_inited && acl_db_global_init()!=0) return -1;

    mysql_init(&g_mysql);

    unsigned long reconnect = 1;
    mysql_options(&g_mysql, MYSQL_OPT_RECONNECT, &reconnect);
    unsigned int  to = 3;
    mysql_options(&g_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &to);
    mysql_options(&g_mysql, MYSQL_OPT_READ_TIMEOUT,  &to);
    mysql_options(&g_mysql, MYSQL_OPT_WRITE_TIMEOUT, &to);

    /* 选一：如果是本机连接，禁用 SSL（避免握手失败） */

    if (host && (!strcmp(host,"127.0.0.1") || !strcmp(host,"::1") || !strcmp(host,"localhost"))) {
        enum mysql_ssl_mode mode = SSL_MODE_DISABLED;
        mysql_options(&g_mysql, MYSQL_OPT_SSL_MODE, &mode);
    }


    /* 或者选二：强制走 Unix Socket（更稳）
    mysql_options(&g_mysql, MYSQL_OPT_UNIX_SOCKET, "/var/run/mysqld/mysqld.sock");
    host = "localhost"; port = 0;
    */

    if (!mysql_real_connect(&g_mysql, host, user, pass, dbname, port, NULL, 0)){
        return -2;
    }
    g_connected = true;

    const char* SQL =
        "SELECT role_id, allow_plain "
        "FROM ip_whitelist "
        "WHERE enabled=1 AND ip=? "
        "LIMIT 1";

    g_stmt = mysql_stmt_init(&g_mysql);
    if (!g_stmt) return -3;
    if (mysql_stmt_prepare(g_stmt, SQL, (unsigned long)strlen(SQL)) != 0){
        mysql_stmt_close(g_stmt); g_stmt=NULL; return -4;
    }
    return 0;
}

void acl_db_disconnect(void){
    if (g_stmt){ mysql_stmt_close(g_stmt); g_stmt=NULL; }
    if (g_connected){ mysql_close(&g_mysql); g_connected=false; }
}

void sockaddr_to_ipv6_16(const struct sockaddr* sa, INT8U out16[16]){
    memset(out16, 0, 16);
    if (sa->sa_family == AF_INET){
        const struct sockaddr_in* v4 = (const struct sockaddr_in*)sa;
        // ::ffff:a.b.c.d
        out16[10]=0xff; out16[11]=0xff;
        memcpy(out16+12, &v4->sin_addr, 4);
    } else if (sa->sa_family == AF_INET6){
        const struct sockaddr_in6* v6 = (const struct sockaddr_in6*)sa;
        memcpy(out16, &v6->sin6_addr, 16);
    }
}

bool acl_check_sockaddr(const struct sockaddr* sa, socklen_t salen, AclResult* out){
    if (!g_connected || !g_stmt) return false;

    INT8U ip16[16];
    sockaddr_to_ipv6_16(sa, ip16);//获取IP地址

    MYSQL_BIND in[1];  memset(in, 0, sizeof(in));
    unsigned long len = 16; my_bool is_null = 0;
    in[0].buffer_type   = MYSQL_TYPE_BLOB;
    in[0].buffer        = (void*)ip16;
    in[0].buffer_length = 16;
    in[0].length        = &len;
    in[0].is_null       = &is_null;

    MYSQL_BIND outb[2]; memset(outb, 0, sizeof(outb));
    int role_id=0; my_bool isnull0=0;
    int allow_plain=0; my_bool isnull1=0;

    outb[0].buffer_type= MYSQL_TYPE_TINY;
    outb[0].buffer     = &role_id;
    outb[0].is_null    = &isnull0;

    outb[1].buffer_type= MYSQL_TYPE_TINY;
    outb[1].buffer     = &allow_plain;
    outb[1].is_null    = &isnull1;

    bool allowed = false;
    pthread_mutex_lock(&g_mtx);             // 单语句简单用全局互斥即可
    do{
        if (mysql_stmt_bind_param(g_stmt, in) != 0) break;//所有参数对应第一个❓号，这里面的❓作为占位符
        if (mysql_stmt_execute(g_stmt) != 0) break;//查询
        if (mysql_stmt_bind_result(g_stmt, outb) != 0) break;//绑定到接收缓冲区

        int stat = mysql_stmt_fetch(g_stmt);//查询到了一条结果，和SQL语句中的一致，返回0
        if (stat == 0){                      // 命中一条
            allowed = true;
            if (out){ out->role_id = role_id; out->allow_plain = (allow_plain!=0); }
        }
        // 清游标，准备下次复用
        mysql_stmt_free_result(g_stmt);
        mysql_stmt_reset(g_stmt);
    }while(0);//；为了配合break语句
    pthread_mutex_unlock(&g_mtx);
    return allowed;
}

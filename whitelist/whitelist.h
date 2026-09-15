

 #ifndef WHITELIST_H
 #define WHITELIST_H
#include "system.h"
 
 /**
  * @brief EMS服务器通信线程
  */
 void Task_Whitelist_Server(void);
typedef enum {
    HS_IDLE = 0,          // 等待客户端发起认证请求
    HS_WAIT_RESP,         // 已下发 token，等待密文回传
    HS_AUTHED             // 已认证（马上跳入业务处理）
} hs_state_t;

typedef struct {
    INT8U plain[512];
    INT8U cipher[512];
    INT8U tx[600];
    INT32U xxkey[4];
    INT8U token[16];
} conn_ctx_t;

#define FUNC_AUTH_REQ 0x41
#define FUNC_AUTH_RESP 0x42 
 /***************************************EMS服务器通信定义************************************/
 
 //读取
 #define TCP_SERVER_RECV_LEN (300)  //接收的缓存长度
 
 #endif // MODBUS_TCP_SERVER_H
 
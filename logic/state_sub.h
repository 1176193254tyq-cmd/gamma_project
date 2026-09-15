#ifndef STATE_SUB_H
#define STATE_SUB_H
#include "system.h"
/**
 * @brief 状态机类型
 * State machine type
 */
typedef enum
{
    SUBInit = 0,  //初始化  initialization
    SUBReady = 1,//就绪 Ready
    SUBStarting = 3,  //启动中 starting
    SUBRun = 4,       //运行 run 
    SUBStopping = 5,   //停机中 Stopping
    SUBReseting = 6,   //复位中 Reseting
    SUBSetStart = 7,   //设置开始 Setup begins
    SUBPowering = 8,   //功率设置 Power setting
    SUBFault = 9,      //故障 Fault
    SUBOutTime = 10,   //超时 OutTime
    SUBLim = 11,        //限制幅值设置 limit setting
}SUB_State_ENUM;

typedef enum {
    COND_NE0,//需要告警码的
    COND_EQ1 //不需要告警码
} cond_t;

typedef struct {
    INT32U addr;
    cond_t   cond;
    const char *fmt;   // 需要参数的用 fmt；不需要参数的用 msg
    const char *msg;
    INT8U  need_sys_sub; // 1: 日志需要 sys_num/sub_num/value
} alarm_item_t;

typedef void (*sub_s_state_fun)(INT16U *event_id,INT8U sys_num,INT8U sub_num,SUB_State_ENUM real_state);
/**
 * 事件类型分为4大类：
 * 第1类是无事件，分配为0
 * 第2类是事件启动，分配在1到100
 * 第3大类是事件已经被执行，执行后处于等待状态，101到200
 * 第4大类是事件已经得到成功的结果，执行后处于等待状态，201到300
 * 以下为第1类和第2类，3类和4类以第2类内容一致，顺序增加100，故不再类举,如此分类的目的是为了完成闭环控制
 * Event types are divided into 4 major categories:
 * Category 1 is no event and is assigned 0
 * Category 2 is event startup, assigned between 1 and 100
 * Category 3 is that the event has been executed and is in a waiting state after execution, 101 to 200
 * The following are Category 1 and Category 2. Category 3 and Category 4 have the sube content as Category 2, 
 * and the order is increased by 100, so they are no longer classified. 
 * The purpose of such classification is to complete closed-loop control.
 */
#define SUB_EVENT_NULL              (0)             //无事件 No events
#define SUB_EVENT_INIT              (1)             //初始化 initialization
#define SUB_EVENT_FAULT             (2)             //发生故障 Fault
#define SUB_EVENT_CMD_START         (3)             //发送启动命令 Send start command
#define SUB_EVENT_CMD_STOP          (4)             //发送停机命令 Send shutdown command
#define SUB_EVENT_CMD_STANDBY       (5)             //发送待机命令 Send standby command
#define SUB_EVENT_CMD_RESET         (6)             //发送复位命令 Send reset command 
#define SUB_EVENT_SUB_RUN           (7)             //SUB进入运行 SUB enters running
#define SUB_EVENT_CTR_TIMEOUT       (8)             //控制超时  Control timeout     
#define SUB_EVENT_FAULT_RECOVERY    (10)            //故障恢复 Recovery  
#define SUB_EVENT_SET_POWER         (11)            //下发功率 Delivery power
#define SUB_EVENT_STOP_PASS         (13)            //停机到检测电压 Shut down to detection voltage
#define SUB_EVENT_RESET_PASS        (14)            //停机到复位 shutdown to reset
#define SUB_EVENT_SET_START         (15)            //开始进行设置 Start setting up
#define SUB_EVENT_CMD_Power         (17)            //发送功率设置命令 Send Power setup command
#define SUB_EVENT_CHECK_Power       (18)            //核对功率设置命令 Check the sag setting command
#define SUB_EVENT_CMD_LIM           (20)            //发送幅值设置命令 Send limit setup command
#define SUB_EVENT_CHECK_LIM         (21)            //核对幅值设置命令 Check limit setting command

#define COUNT_BITS_16(x)  ( {            \
    INT16U __v = (INT16U)(x);        \
    INT8U  __c = 0;                    \
    while (__v) { __v &= (INT16U)(__v - 1); ++__c; } \
    __c;                                 \
} )
typedef struct {
SUB_State_ENUM now_state;          //当前状态 Current status
INT8U  eventId;                   //事件ID  Event ID  
SUB_State_ENUM next_state;          //下个状态  next state
sub_s_state_fun fun;                //执行函数  Execute function
char *string;                       //打印的日志内容 Printed log content
}StateSUBform;

/**
 * @brief 状态机执行函数；
 * State machine execution function
 * @param[in] state - Current status
 * @param[in] bms_num-BMS serial number, 0 represents the first unit
 * @param[in] sub_num-sub serial number, 0 represents the first unit
 * @retval null
 */
void SUB_State_Run(volatile SUB_State_ENUM *state,INT8U sys_num,INT8U sub_num);

SUB_State_ENUM Get_State_Sub(INT8U sys_num,INT8U sub_num);
INT8U set_out_sub(INT8U sys_num,INT8U sub_num,INT16U event_id);
INT8U Get_Out_Sub(INT8U sys_num,INT8U sub_num);
uint64_t get_out_time_sub(INT8U sys_num,INT8U sub_num);
void Clear_Out_Sub(INT8U sys_num,INT8U sub_num);
void Wait_Out_Sub(INT8U sys_num,INT8U sub_num);
INT8U Set_In_Sub(INT8U sys_num,INT8U sub_num,INT16U event_id);
INT8U get_in_sub(INT8U sys_num,INT8U sub_num);
void Clear_In_Sub(INT8U sys_num,INT8U sub_num);
void wait_in_sub(INT8U sys_num,INT8U sub_num);
uint64_t get_in_time_sub(INT8U sys_num,INT8U sub_num);

void Success_Out_Sub(INT8U sys_num,INT8U sub_num);

void success_in_sub(INT8U sys_num,INT8U sub_num);
void system_value1();
void system_value();
#endif
#include "main.h"
#include "log.h"
#include "timed_check.h"
#include "state_sub.h"
#include "state_task.h"
#include <stdarg.h>
#include <math.h>
extern INT8U BusType;
/**
 * @brief 事件类型，每个SUB都有这个标识
 * Event type, each SUB has this flag
 */
static volatile INT16U Event_Id[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

/**
 * @brief 状态机持续时间，每个SUB都有计时器器
 * State machine duration, each SUB has a sofer timer
 */
static volatile uint64_t Last_Time[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

/**
 * @brief 该SUB状态机模块的事件输入接口，由输入事件触发状态机的轮转
 * The event input interface of the SUB state machine module triggers the rotation of the state machine by input events.
 */
static volatile INT16U Event_In[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

/**
 * @brief 该SUB状态机模块的事件输出接口，由输出事件触发外部模块的动作
 * The event output interface of the SUB state machine module triggers the action of the external module by the output event.
 */
static volatile INT16U Event_Out[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

/**
 * @brief 输入事件持续时间，每个SUB都有计时器，事件开始或结束该计时器会被清0
 * Input event duration times. Each SUB has a timer. The timer will be cleared to 0 when the event begins or ends.
 */
static volatile uint64_t Event_In_Time[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

/**
 * @brief 输出事件持续时间，每个SUB都有计时器，事件开始或结束该计时器会被清0
 * Output event duration times. Each SUB has a timer. The timer will be cleared to 0 when the event begins or ends.
 */
static volatile uint64_t Event_Out_Time[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

/* -------------------- 内部函数声明 -------------------- */

static void Event_Init_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Fault_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_CMD_Start_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_CMD_Stop_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_CMD_Reset_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_SUB_Run_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Timeout_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Fault_Recovery_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_CMD_Power_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Power_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Stop_Pass_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Reset_Pass_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_CMD_Lim_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);
static void Event_Lim_Check(INT16U *Event_Id, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state);

/* -------------------- 状态表 -------------------- */

static const StateSUBform state_list[] = {
    { SUBInit,     SUB_EVENT_INIT,            SUBReady,    Event_Init_Check,           "初始化事件：初始化->就绪" },

    { SUBFault,    SUB_EVENT_CMD_STOP,        SUBStopping, Event_CMD_Stop_Check,       "关闭命令：故障停机->关闭中" },
    { SUBFault,    SUB_EVENT_FAULT_RECOVERY,  SUBReady,    Event_Fault_Recovery_Check, "故障恢复：故障停机->就绪" },
    { SUBFault,    SUB_EVENT_CMD_RESET,       SUBReseting, Event_CMD_Reset_Check,      "复位命令：故障停机->复位" },

    { SUBOutTime,  SUB_EVENT_FAULT,           SUBFault,    Event_Fault_Check,          "故障事件：超时->故障" },
    { SUBOutTime,  SUB_EVENT_CMD_STOP,        SUBStopping, Event_CMD_Stop_Check,       "关闭命令：超时->关闭中" },

    { SUBStopping, SUB_EVENT_FAULT,           SUBFault,    Event_Fault_Check,          "故障事件：停机->故障" },
    { SUBStopping, SUB_EVENT_STOP_PASS,       SUBReady,    Event_Stop_Pass_Check,      "关闭事件：停机->就绪" },
    { SUBStopping, SUB_EVENT_CMD_RESET,       SUBReseting, Event_CMD_Reset_Check,      "复位命令：停机->复位" },

    { SUBReady,    SUB_EVENT_FAULT,           SUBFault,    Event_Fault_Check,          "故障事件：就绪->故障" },
    { SUBReady,    SUB_EVENT_CMD_STOP,        SUBStopping, Event_CMD_Stop_Check,       "关闭命令：就绪->关闭中" },
    { SUBReady,    SUB_EVENT_CMD_START,       SUBStarting, Event_CMD_Start_Check,      "启动命令：就绪->子系统启动中" },

    { SUBStarting, SUB_EVENT_FAULT,           SUBFault,    Event_Fault_Check,          "故障事件：启动中->故障" },
    { SUBStarting, SUB_EVENT_CMD_STOP,        SUBStopping, Event_CMD_Stop_Check,       "关闭命令：启动中->关闭中" },
    { SUBStarting, SUB_EVENT_SUB_RUN,         SUBRun,      Event_SUB_Run_Check,        "SUB运行：启动中->运行" },

    { SUBRun,      SUB_EVENT_FAULT,           SUBFault,    Event_Fault_Check,          "故障事件：运行->故障" },
    { SUBRun,      SUB_EVENT_CMD_STOP,        SUBStopping, Event_CMD_Stop_Check,       "关闭命令：运行->关闭中" },

    // { SUBRun,      SUB_EVENT_CMD_LIM,         SUBLim,      Event_CMD_Lim_Check,      "设置幅值命令：运行->设置幅值命令" },
    // { SUBRun,      SUB_EVENT_CMD_Power,       SUBPowering, Event_CMD_Power_Check,    "设置功率命令：运行->设置功率命令" },
     //{ SUBLim,      SUB_EVENT_CHECK_LIM,       SUBRun,      Event_Lim_Check,          "核对幅值：设置幅值命令->运行" },
    // { SUBPowering, SUB_EVENT_CHECK_Power,     SUBRun,      Event_Power_Check,        "核对功率：设置功率命令->运行" },
    // { SUBReseting, SUB_EVENT_RESET_PASS,      SUBReady,    Event_Reset_Pass_Check,   "复位事件：复位->就绪" },
};

#define LIST_SIZE (sizeof(state_list) / sizeof(StateSUBform))

INT8U Debug_State_Sub(INT8U sys_num, INT8U sub_num, INT8U sta)
{
    (void)sys_num; (void)sub_num;
    INT8U debug_sta = 0;
    return (debug_sta == sta) ? 1 : 0;
}

SUB_State_ENUM Get_State_Sub(INT8U sys_num, INT8U sub_num)
{
    return SUB_state[sys_num * MAX_SUB_NUM + sub_num];
}

uint64_t Get_State_Time_Sub(INT8U sys_num, INT8U sub_num)
{
    uint64_t t = 0;
    if (Timer_GetTick() > Last_Time[sys_num * MAX_SUB_NUM + sub_num]) {
        t = Timer_GetTick() - Last_Time[sys_num * MAX_SUB_NUM + sub_num];
    }
    return t;
}

INT8U Set_Out_Sub(INT8U sys_num, INT8U sub_num, INT16U id)
{
    Event_Out[sys_num * MAX_SUB_NUM + sub_num] = id;
    Event_Out_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
    return Event_Out[sys_num * MAX_SUB_NUM + sub_num];
}

INT8U Get_Out_Sub(INT8U sys_num, INT8U sub_num)
{
    return Event_Out[sys_num * MAX_SUB_NUM + sub_num];
}

uint64_t Get_Out_Time_Sub(INT8U sys_num, INT8U sub_num)
{
    uint64_t t = 0;
    if (Timer_GetTick() > Event_Out_Time[sys_num * MAX_SUB_NUM + sub_num]) {
        t = Timer_GetTick() - Event_Out_Time[sys_num * MAX_SUB_NUM + sub_num];
    }
    return t;
}

void Clear_Out_Sub(INT8U sys_num, INT8U sub_num)
{
    Event_Out_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
    Event_Out[sys_num * MAX_SUB_NUM + sub_num] = 0;
}

void Wait_Out_Sub(INT8U sys_num, INT8U sub_num)
{
    INT16U *p = &Event_Out[sys_num * MAX_SUB_NUM + sub_num];
    if ((*p > 0) && (*p <= 100)) *p += 100;
}

void Success_Out_Sub(INT8U sys_num, INT8U sub_num)
{
    INT16U *p = &Event_Out[sys_num * MAX_SUB_NUM + sub_num];
    if ((*p > 100) && (*p <= 200)) *p += 100;
}

INT8U Set_In_Sub(INT8U sys_num, INT8U sub_num, INT16U id)
{
    Event_In[sys_num * MAX_SUB_NUM + sub_num] = id;
    Event_In_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
    return Event_In[sys_num * MAX_SUB_NUM + sub_num];
}

INT8U Get_In_Sub(INT8U sys_num, INT8U sub_num)
{
    return Event_In[sys_num * MAX_SUB_NUM + sub_num];
}

void Clear_In_Sub(INT8U sys_num, INT8U sub_num)
{
    Event_In_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
    Event_In[sys_num * MAX_SUB_NUM + sub_num] = 0;
}

void Wait_In_Sub(INT8U sys_num, INT8U sub_num)
{
    INT16U *p = &Event_In[sys_num * MAX_SUB_NUM + sub_num];
    if ((*p > 0) && (*p <= 100)) *p += 100;
}


uint64_t Get_In_Time_Sub(INT8U sys_num, INT8U sub_num)
{
    uint64_t t = 0;
    if (Timer_GetTick() > Event_In_Time[sys_num * MAX_SUB_NUM + sub_num]) {
        t = Timer_GetTick() - Event_In_Time[sys_num * MAX_SUB_NUM + sub_num];
    }
    return t;
}

void Success_In_Sub(INT8U sys_num, INT8U sub_num)
{
    INT16U *p = &Event_In[sys_num * MAX_SUB_NUM + sub_num];
    if ((*p > 100) && (*p <= 200)) *p += 100;
}

/**
 * @brief 初始化事件检查函数，用于处理子系统的初始化事件
 * 
 * 该函数负责检查并处理子系统的初始化状态，包括：
 * 1. 清空所有事件输入/输出缓冲区
 * 2. 检查子系统是否已初始化
 * 3. 如果未初始化，记录初始化时间
 * 4. 如果已初始化，检查是否需要触发初始化事件
 * 
 * @param eid 事件ID数组指针，用于存储和更新子系统事件ID
 * @param sys_num 系统编号，用于标识当前处理的系统
 * @param sub_num 子系统编号，用于标识当前处理的子系统
 * @param real_state 子系统实际状态枚举值（本函数中未使用）
 * 
 * @note 函数内部使用静态变量init_flag记录初始化状态
 * @note 初始化事件触发间隔为5秒（5 * 1000毫秒）
 * @note 特殊字符处理：\t（制表符）、\r（回车符）、\n（换行符）
 */
static void Event_Init_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    static INT8U init_flag[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

    for (int i = 0; i < (MAX_SYS_NUM * MAX_SUB_NUM); i++) {
        Event_In[i] = 0;
        Event_Out[i] = 0;
    }

    if (init_flag[sys_num * MAX_SUB_NUM + sub_num] == 0) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        init_flag[sys_num * MAX_SUB_NUM + sub_num] = 1;
    } else {
        if (!Soft_Timer(Last_Time[sys_num * MAX_SUB_NUM + sub_num], 5 * 1000)) {
            Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
            eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_INIT;
        }
    }
}

/**
 * @brief 检查系统故障状态并处理相应的告警事件
 * 
 * @param eid 输出参数，用于存储故障事件ID
 * @param sys_num 系统编号
 * @param sub_num 子模块编号
 * @param real_state 子模块的实际状态
 * 
 * @note 该函数会检查多种故障条件，包括消防告警、变压器轻瓦斯报警、
 *       烟雾报警和测控故障等。当检测到故障时，会执行相应的处理逻辑，
 *       包括清零目标功率、设置故障标志和记录日志等。
 * 
 * @warning 函数内部使用了GET_INPUT、GET_HOLD、SET_HOLD等宏，这些宏
 *          的具体实现未在代码片段中展示，需要确保这些宏定义正确。
 * 
 * @attention 

 * - 基址计算部分有注释掉的旧代码和当前使用的代码
 * - 使用了sys_cfg->subNum替代了原来的MAX_SUB_NUM常量
 */
static void Event_Fault_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U is_Fault = 0;
sysPara* sys_cfg = SysConf_GetInfo();

    if ( (INT16U)GET_INPUT(27312)==1    //变压器轻瓦斯报警信号     
    || (INT16U)GET_INPUT(27316)==1    //烟雾报警信号   
    || GET_INPUT(27300) == 1                        //测控故障总   

    ) 
        
    {
        is_Fault = 1;
        if(GET_HOLD(399)==0)//目标功率清0使能
        {
        SET_HOLD(1010, 0);
        SET_HOLD(1011, 0);
        LOG_INFO("MV目标功率清0!");
        }
        
       SET_INPUT(SYSTEM_FAULT_SUMMARY, 1);

    }



    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_FAULT);
    if ((is_Fault == 1) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_FAULT;
        LOG_INFO("SUB-%d is Fault!", sub_num);
    }
}


static void Event_CMD_Start_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CMD_START);

    if ((SUB_EVENT_CMD_START == Get_In_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CMD_START;
        Clear_In_Sub(sys_num, sub_num);
        LOG_INFO("SUB-%d state is %d!", sub_num, eid[sys_num * MAX_SUB_NUM + sub_num]);
        LOG_INFO("sys_num:%d, sub_num:%d", sys_num, sub_num);
        // Set_Out_Sub(sys_num, sub_num, SUB_EVENT_CMD_START);
    }
}

static void Event_CMD_Stop_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CMD_STOP);

    if ((SUB_EVENT_CMD_STOP == Get_In_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CMD_STOP;
        Clear_In_Sub(sys_num, sub_num);
        LOG_INFO("SUB-%d state is %d!", sub_num, eid[sys_num * MAX_SUB_NUM + sub_num]);
        // Set_Out_Sub(sys_num, sub_num, SUB_EVENT_CMD_STOP);
    }
}

static void Event_CMD_Reset_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CMD_RESET);

    if ((SUB_EVENT_CMD_RESET == Get_In_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CMD_RESET;
        Clear_In_Sub(sys_num, sub_num);
        LOG_INFO("SUB-%d state is %d!", sub_num, eid[sys_num * MAX_SUB_NUM + sub_num]);
        Set_Out_Sub(sys_num, sub_num, SUB_EVENT_CMD_RESET);
    }
}

static void Event_SUB_Run_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_SUB_RUN);
    INT16U regval = GET_INPUT(28071 + (sys_num * BMS_PER_SYS + sub_num) * 200);
    INT8U BMS_connection_state = GET_INPUT(35001 + sub_num * 300);
    (void)BMS_connection_state;

    // if ((((SUB_EVENT_CMD_START + 200) == Get_Out_Sub(sys_num, sub_num)) && (regval == 0 || regval == 1 || regval == 2)) ||
    //     (1 == resD)) 
        
    if (((GET_INPUT(28062 + (sys_num * BMS_PER_SYS + sub_num) * 200)==2) && (regval == 0 || regval == 1 || regval == 2)) ||
    (1 == resD))   
        
    {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_SUB_RUN;
        LOG_INFO("SUB-%d is already started!", sub_num);
    }
}

static void Event_Timeout_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)eid; (void)sys_num; (void)sub_num; (void)real_state;

}

static void Event_Fault_Recovery_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;

    INT8U is_finish = 1;
  
    INT8U bms_comm_finsih = 1;
    bool all_salf = true;

    sysPara *sys_cfg = SysConf_GetInfo();

    if ( (INT16U)GET_INPUT(27312)==1    //变压器轻瓦斯报警信号     
    || (INT16U)GET_INPUT(27316)==1    //烟雾报警信号   
    || (INT16U)GET_INPUT(27316)==1    //烟雾报警信号  
    || GET_INPUT(27300) == 1                        //测控故障总   

    )                                                                                                                                                    
    {
        is_finish = 0;

    } else {
        is_finish = 1;

    }
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_FAULT_RECOVERY);
    if ((1 == is_finish) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_FAULT_RECOVERY;
        LOG_INFO("SUB-%d fault recovery!", sub_num);
    }
}

#define STABLE_TICKS_1S   (50u)   /* 10ms tick  */


static INT8U debounce_bool(INT8U raw_now,
                             INT8U *raw_prev,
                             uint64_t *last_change_tick)
{
    uint64_t now = Timer_GetTick();

    if (raw_now != *raw_prev) {
        *raw_prev = raw_now;
        *last_change_tick = now;
    }


    if ((uint64_t)(now - *last_change_tick) >= (uint64_t)STABLE_TICKS_1S) {
        return raw_now;
    }


    return raw_now;
}
static uint16_t PCS_Status_To_RegValue(INT32S status)
{
    if (status == 0)
    {
        return 0;
    }
    else if (status > 0)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}
static void check_limit(INT8U sys_num, INT8U sub_num)
{
    sysPara *sys_cfg = SysConf_GetInfo();

    /* ---------------- 索引 ---------------- */
    INT16U idx = (INT16U)(sys_num * MAX_SUB_NUM + sub_num);
    INT32U sub_index = (INT32U)(sys_num * sys_cfg->subNum + sub_num);
    INT32U fire_base = sub_index * 30U;

    /* ---------------- 每个 sys/sub 一套防抖状态 ---------------- */
    static INT8U   s_alarm_raw_prev[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static uint64_t s_alarm_last_change[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static INT8U   s_alarm_stable[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static INT8U   s_alarm_stable_prev[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

    static INT8U   s_trip_raw_prev[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static uint64_t s_trip_last_change[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static INT8U   s_trip_stable[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static INT8U   s_trip_stable_prev[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

    static INT8U   s_summary_raw_prev[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static uint64_t s_summary_last_change[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static INT8U   s_summary_stable[MAX_SYS_NUM * MAX_SUB_NUM] = {0};
    static INT8U   s_summary_stable_prev[MAX_SYS_NUM * MAX_SUB_NUM] = {0};

    uint64_t now = Timer_GetTick();

    /* ---------------- 先判断系统运行状态 ---------------- */
    SYS_State_ENUM state0 = Get_State_Sys(0);
    SYS_State_ENUM state1 = Get_State_Sys(1);

    bool sys0_run = (state0 == SYSRun || state0 == SYSWarnRun);
    bool sys1_run = (state1 == SYSRun || state1 == SYSWarnRun);

    if ((!sys0_run) && (!sys1_run))
    {
        if (GET_HOLD(399) == 0)   /* 目标功率清0使能 */
        {
            SET_HOLD(1010, 0);
            SET_HOLD(1011, 0);
        }
    }

    /* =========================================================
     * 1) 原始告警 raw_alarm_now
     * ========================================================= */
    INT8U raw_alarm_now = 0;

    /* 子系统消防 + 公共告警 */
// GET_INPUT(64905 + fire_base) != 0 ||     /* 消防一级 */
//         GET_INPUT(64906 + fire_base) != 0 ||     /* 消防二级 */
    if ( GET_INPUT(27312) == 1 ||                 /* 轻瓦斯报警 */
        GET_INPUT(27316) == 1 ||                 /* 烟雾报警 */
        GET_INPUT(27300) == 1)                   /* 测控故障总 */
    {
        raw_alarm_now = 1;
    }

    /* PCS 故障字2：烟感 bit9 */
    if ((((GET_INPUT(17000 + 300 * 0 + 67) >> 9) & 1u) == 1u) ||
        (((GET_INPUT(17000 + 300 * 1 + 67) >> 9) & 1u) == 1u) ||
        (((GET_INPUT(2600 + 300 * 1 + 201) >> 9) & 1u) == 1u)||
        (((GET_INPUT(2600 + 300 * 3 + 201) >> 9) & 1u) == 1u)
    )
    {
        raw_alarm_now = 1;
    }

    /* =========================================================
     * 2) 原始 Trip raw_trip_now
     * ========================================================= */
    INT8U raw_trip_now =
        (GET_INPUT(27309) != 0) ||   /* 油位超低跳闸 */
        (GET_INPUT(27311) != 0) ||   /* 超温跳闸 */
        (GET_INPUT(27313) != 0) ||   /* 重瓦斯跳闸 */
        (GET_INPUT(27314) != 0) ||   /* 压力释放阀动作 */
        (GET_INPUT(27317) != 0) ||   /* 绕组超温跳闸 */
        (GET_INPUT(27325) != 0) ||   /* 应急跳闸 */
        (GET_INPUT(27308) != 0);     /* 辅助变超高温报警 */

    if (raw_trip_now)
    {
        raw_alarm_now = 1;
    }

    /* =========================================================
     * 3) 告警防抖
     * ========================================================= */
    if (raw_alarm_now != s_alarm_raw_prev[idx])
    {
        s_alarm_raw_prev[idx] = raw_alarm_now;
        s_alarm_last_change[idx] = now;
    }

    if ((uint64_t)(now - s_alarm_last_change[idx]) >= (uint64_t)STABLE_TICKS_1S)
    {
        s_alarm_stable[idx] = raw_alarm_now;
    }

    /* =========================================================
     * 4) Trip 防抖
     * ========================================================= */
    if (raw_trip_now != s_trip_raw_prev[idx])
    {
        s_trip_raw_prev[idx] = raw_trip_now;
        s_trip_last_change[idx] = now;
    }

    if ((uint64_t)(now - s_trip_last_change[idx]) >= (uint64_t)STABLE_TICKS_1S)
    {
        s_trip_stable[idx] = raw_trip_now;
    }

    /* Trip 稳定上升沿，只触发一次 */
    if (s_trip_stable[idx] && !s_trip_stable_prev[idx])
    {
        LOG_INFO("sys:%d sub:%d RMU Trip stable rising!", sys_num, sub_num);

        if (GET_HOLD(399) == 0)
        {
            SET_HOLD(1010, 0);
            SET_HOLD(1011, 0);
        }

        measureflag = 1;
    }
    s_trip_stable_prev[idx] = s_trip_stable[idx];

    /* =========================================================
     * 5) 通信状态
     * ========================================================= */
    INT8U measu_comm_fault = (Get_MEASU_Comm(0) == 1) ? 1 : 0;
    INT8U ems_comm_fault   = (Get_EMS_Comm(0)   == 1) ? 1 : 0;
    INT8U ups_comm_fault   = (Get_UPS_Comm(0)   == 1) ? 1 : 0;

    /* 测控通信断联：清测控相关点位 */
    if (measu_comm_fault)
    {
        for (int i = 27300; i < 27500; i++) SET_INPUT(i, 0);
        for (int i = 25300; i < 25400; i++) SET_INPUT(i, 0);
        for (int i = 25000; i < 25020; i++) SET_INPUT(i, 0);
        for (int i = 25800; i < 25822; i++) SET_INPUT(i, 0);

        INT16U measureval = (INT16U)GET_INPUT(STATUS_WORD2);
        measureval &= (INT16U)~(1u << 8);
        SET_INPUT(STATUS_WORD2, measureval);
    }
    else
    {
        INT16U measureval = (INT16U)GET_INPUT(STATUS_WORD2);
        measureval |= (INT16U)(1u << 8);
        SET_INPUT(STATUS_WORD2, measureval);
    }

    /* UPS 通信断联 */
    if (ups_comm_fault)
    {
        for (int i = 27100; i < 27140; i++) SET_INPUT(i, 0);

        INT16U upsval = (INT16U)GET_INPUT(STATUS_WORD2);
        upsval &= (INT16U)~(1u << 9);
        SET_INPUT(STATUS_WORD2, upsval);
    }
    else
    {
        INT16U upsval = (INT16U)GET_INPUT(STATUS_WORD2);
        upsval |= (INT16U)(1u << 9);
        SET_INPUT(STATUS_WORD2, upsval);
    }


        /* EMS 通信断联 */
    if (ems_comm_fault)
    {
      
        INT16U emsval = (INT16U)GET_INPUT(STATUS_WORD2);
        emsval &= (INT16U)~(1u << 10);
        SET_INPUT(STATUS_WORD2, emsval);
    }
    else
    {
        INT16U emsval = (INT16U)GET_INPUT(STATUS_WORD2);
        emsval |= (INT16U)(1u << 10);
        SET_INPUT(STATUS_WORD2, emsval);
    }

    /* =========================================================
     * 6) BMS 通信断联清点位
     * ========================================================= */
    if (Get_BMS_Comm(sub_index) == 1)
    {
        INT16U BMScommstatus = (INT16U)GET_INPUT(STATUS_WORD3);

        if (sub_index < 16U)
        {
            BMScommstatus &= (INT16U)~(1u << sub_index);
            SET_INPUT(STATUS_WORD3, BMScommstatus);
        }

        for (int i = 28000; i < 28200; i++)
        {
            SET_INPUT(i + sub_index * 200, 0);
        }

        for (int i = 31000; i < 31199; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                SET_INPUT(i + sub_index * 3000 + j * 200, 0);
            }
        }

        for (int i = 0; i < 1300; i++)
        {
            for (int j = 0; j < 12; j++)
            {
                SET_C_INPUT(sub_index + 1, i + j * 2000, 0);
            }
        }

        for (int i = 35001; i < 35050; i++)
        {
            SET_HOLD(sub_index * 300 + i, 0);
        }
    }
    else
    {
        INT16U BMScommstatus = (INT16U)GET_INPUT(STATUS_WORD3);

        if (sub_index < 16U)
        {
            BMScommstatus |= (INT16U)(1u << sub_index);
            SET_INPUT(STATUS_WORD3, BMScommstatus);
        }
    }

    /* =========================================================
     * 7) 统一汇总 summary_raw
     * 
     * ========================================================= */
    INT8U summary_raw = 0;

    if (s_alarm_stable[idx]) summary_raw = 1;
    if (s_trip_stable[idx])  summary_raw = 1;
    if (measu_comm_fault)    summary_raw = 1;
    if (ems_comm_fault)      summary_raw = 1;
  

    /* summary 防抖 */
    if (summary_raw != s_summary_raw_prev[idx])
    {
        s_summary_raw_prev[idx] = summary_raw;
        s_summary_last_change[idx] = now;
    }

    if ((uint64_t)(now - s_summary_last_change[idx]) >= (uint64_t)STABLE_TICKS_1S)
    {
        s_summary_stable[idx] = summary_raw;
    }

    /* =========================================================
     * 8) summary 上升沿 / 下降沿动作
     * ========================================================= */
    if (s_summary_stable[idx] && !s_summary_stable_prev[idx])
    {
        LOG_INFO("SYSTEM_FAULT_SUMMARY 0->1, sys:%d sub:%d", sys_num, sub_num);

        
        if (GET_INPUT(64905 + fire_base) != 0)
            LOG_INFO("reason: fire level1, sys:%d sub:%d val:%d",
                     sys_num, sub_num, GET_INPUT(64905 + fire_base));

        if (GET_INPUT(64906 + fire_base) != 0)
            LOG_INFO("reason: fire level2, sys:%d sub:%d val:%d",
                     sys_num, sub_num, GET_INPUT(64906 + fire_base));

        if (GET_INPUT(27312) == 1) LOG_INFO("reason: transformer gas alarm");
        if (GET_INPUT(27316) == 1) LOG_INFO("reason: smoke alarm");
        if (GET_INPUT(27300) == 1) LOG_INFO("reason: measure fault summary");

        if ((((GET_INPUT(17000 + 300 * 0 + 67) >> 9) & 1u) == 1u) ||
            (((GET_INPUT(17000 + 300 * 1 + 67) >> 9) & 1u) == 1u)||
            (((GET_INPUT(2600 + 300 * 1 + 201) >> 9) & 1u) == 1u)||
            (((GET_INPUT(2600 + 300 * 3 + 201) >> 9) & 1u) == 1u)
        )
        {
            LOG_INFO("reason: PCS fault word2 smoke bit");
        }

        if (raw_trip_now)
        {
            LOG_INFO("reason: RMU Trip active");
        }

        if (measu_comm_fault) LOG_INFO("reason: MEASU comm lost");
        if (ems_comm_fault)   LOG_INFO("reason: EMS comm lost");
     

        if (GET_HOLD(399) == 0)
        {
            SET_HOLD(1010, 0);
            SET_HOLD(1011, 0);
            LOG_INFO("MV target power cleared to 0");
        }

        SET_INPUT(SYSTEM_FAULT_SUMMARY, 1);

        if (Get_State_Sub(sys_num, sub_num) == SUBRun)
        {
            Set_In_Sub(sys_num, sub_num, SUB_EVENT_FAULT);
        }
    }
    else if ((!s_summary_stable[idx]) && s_summary_stable_prev[idx])
    {
        LOG_INFO("SYSTEM_FAULT_SUMMARY 1->0, sys:%d sub:%d", sys_num, sub_num);
        SET_INPUT(SYSTEM_FAULT_SUMMARY, 0);
    }
    else
    {
        /* 稳态保持 */
        SET_INPUT(SYSTEM_FAULT_SUMMARY, s_summary_stable[idx] ? 1 : 0);
    }

    s_summary_stable_prev[idx] = s_summary_stable[idx];
    s_alarm_stable_prev[idx]   = s_alarm_stable[idx];

    /* =========================================================
     * 9) 若 summary 已置位，则保持目标功率清0
     * ========================================================= */
    if (GET_INPUT(SYSTEM_FAULT_SUMMARY) == 1)
    {
        if (GET_HOLD(399) == 0)
        {
            SET_HOLD(1010, 0);
            SET_HOLD(1011, 0);
        }
    }
}

 void system_value()
{
    sysPara *sys_cfg = SysConf_GetInfo();
    INT16U year, month, day, hour, minute, second;
//当设备进行切入切出时。通信拓扑会发生变化，然后根据不同设备进行系统赋值
    if (g_enabled_mask_pcs == 0x0C || g_enabled_mask_bms == 0x0C) {

        INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
        INT16U SOC = (GET_INPUT(28000 + 2* 200 + 3) +
                        GET_INPUT(28000 + 3 * 200 + 3) 
                                                        ) / BMS_num;
        SET_INPUT(SYSTEM_SOC, SOC);

        INT16U SOH = (GET_INPUT(28000 + 2 * 200 + 4) +
                        GET_INPUT(28000 + 3 * 200 + 4) 
                                                        ) / BMS_num;
        SET_INPUT(SYSTEM_SOH, SOH);


        INT16U ac_freq = GET_INPUT(17000 + 1 * 300 + 43);
        SET_INPUT(AC_FREQUENCY, ac_freq);

        INT16S Power_Factor = (INT16S)GET_INPUT(17000 + 1 * 300 + 42);
        SET_INPUT(POWER_FACTOR, Power_Factor);

        INT32U DC_average_voltage = (GET_INPUT(28000 + 2 * 200 + 0) +
                                       GET_INPUT(28000 + 3* 200 + 0) )/ BMS_num;
        SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
        SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);

        if (GET_INPUT(17063 + 1 * 300) == 2) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
        }
        SET_INPUT(NUMBER_OF_WARNING_PCSs, (((GET_INPUT(17060 + 1 * 300) >> 6) & 1) ? 2 : 0));
        SET_INPUT(NUMBER_OF_FAULT_PCSs,   (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) ? 2 : 0));
        SET_INPUT(PCS_GROUP_STATUS,   (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) ? 2 : 0));
        int faulting_BMS_Num = 0;   
        
        for(int fault_BMS=0;fault_BMS<2;fault_BMS++)
        {
            if((GET_INPUT(28071 + (fault_BMS+2) * 200) == 4))
            {
                faulting_BMS_Num++;
            }
            else
            {
                faulting_BMS_Num=faulting_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );
        int Warning_BMS_Num=0;
        for(int Warn_BMS_Num=0;Warn_BMS_Num<2;Warn_BMS_Num++)
        {
            if((GET_INPUT(28040 + (Warn_BMS_Num+2) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num+2) * 200) !=0))
            {
                Warning_BMS_Num++;
            }
            else
            {
                Warning_BMS_Num=Warning_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );
        int running_bms_num =0;
        for(int Run_BMS_Num=0;Run_BMS_Num<2;Run_BMS_Num++)
        {
            if((GET_INPUT(28072 + (Run_BMS_Num+2) * 200) !=0))
            {
                running_bms_num ++;
            }
            else
            {
                running_bms_num =running_bms_num ;
            }
        }
        SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num  );

        if ((GET_INPUT(28072 + 2* 200) == 2) ||
            (GET_INPUT(28072 + 3 * 200) == 2) ) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
        } else if ((GET_INPUT(28072 + 2 * 200) == 1) ||
                   (GET_INPUT(28072 + 3 * 200) == 1) ) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 5);
        }

    } else if (g_enabled_mask_pcs == 0x03 || g_enabled_mask_bms == 0x03) {
       INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
        INT16U SOC = (GET_INPUT(28000 + 0 * 200 + 3) +
                        GET_INPUT(28000 + 1 * 200 + 3) ) / BMS_num;
        SET_INPUT(SYSTEM_SOC, SOC);

        INT16U SOH = (GET_INPUT(28000 + 0 * 200 + 4) +
                        GET_INPUT(28000 + 1 * 200 + 4) ) / BMS_num;
        SET_INPUT(SYSTEM_SOH, SOH);

        INT16U ac_freq = GET_INPUT(17000 + 0 * 300 + 43);
        SET_INPUT(AC_FREQUENCY, ac_freq);

        INT16S Power_Factor = (INT16S)GET_INPUT(17000 + 0 * 300 + 42);
        SET_INPUT(POWER_FACTOR, Power_Factor);

        INT32U DC_average_voltage = (GET_INPUT(28000 + 0 * 200 + 0) +
                                       GET_INPUT(28000 + 1 * 200 + 0) ) /BMS_num;
        SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
        SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);

        if (GET_INPUT(17063 + 0 * 300) == 2) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
        }
        SET_INPUT(NUMBER_OF_WARNING_PCSs, (((GET_INPUT(17060 + 0 * 300) >> 6) & 1) ? 2 : 0));
        SET_INPUT(NUMBER_OF_FAULT_PCSs,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 2 : 0));
        SET_INPUT(PCS_GROUP_STATUS,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 1 : 0));
        if ((GET_INPUT(28072 + 0 * 200) == 2) ||
            (GET_INPUT(28072 + 1 * 200) == 2) ) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
        } else if ((GET_INPUT(28072 + 0 * 200) == 1) ||
                   (GET_INPUT(28072 + 1 * 200) == 1) ) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 5);
        }
        int faulting_BMS_Num = 0;  
        for(int fault_BMS=0;fault_BMS<2;fault_BMS++)
        {
            if((GET_INPUT(28071 + (fault_BMS) * 200) == 4))
            {
                faulting_BMS_Num++;
            }
            else
            {
                faulting_BMS_Num=faulting_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );
        int Warning_BMS_Num=0;
        for(int Warn_BMS_Num=0;Warn_BMS_Num<2;Warn_BMS_Num++)
        {
            if((GET_INPUT(28040 + (Warn_BMS_Num) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num) * 200) !=0))
            {
                Warning_BMS_Num++;
            }
            else
            {
                Warning_BMS_Num=Warning_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );
        int running_bms_num =0;
        for(int Run_BMS_Num=0;Run_BMS_Num<2;Run_BMS_Num++)
        {
            if((GET_INPUT(28072 + (Run_BMS_Num) * 200) !=0))
            {
                running_bms_num ++;
            }
            else
            {
                running_bms_num =running_bms_num ;
            }
        }
        SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num );

    } else {
        if ((GET_INPUT(NUMBER_OF_PCSs) == 2)&&(GET_INPUT(NUMBER_OF_BMSs) == 2)) {

            INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
            INT16U SOC = (GET_INPUT(28000 + 0 * 200 + 3) +
                            GET_INPUT(28000 + 1 * 200 + 3) ) / BMS_num;
            SET_INPUT(SYSTEM_SOC, SOC);

            INT16U SOH = (GET_INPUT(28000 + 0 * 200 + 4) +
                            GET_INPUT(28000 + 1 * 200 + 4) ) / BMS_num;
            SET_INPUT(SYSTEM_SOH, SOH);

            INT16U ac_freq = GET_INPUT(17000 + 0 * 300 + 43);
            SET_INPUT(AC_FREQUENCY, ac_freq);

            INT16S Power_Factor = (INT16S)GET_INPUT(17000 + 0 * 300 + 42);
            SET_INPUT(POWER_FACTOR, Power_Factor);
     
            INT32U DC_average_voltage = (GET_INPUT(28000 + 0 * 200 + 0) +
                                           GET_INPUT(28000 + 1 * 200 + 0) ) / BMS_num;
            SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
            SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);

            if ((GET_INPUT(17063 + 0 * 300) == 2)&&((GET_INPUT(17062 + 0 * 300) == 1))) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
            }
            else if ((GET_INPUT(17063 + 0 * 300) == 16)&&((GET_INPUT(17062 + 0 * 300) == 2))) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 1);
            }            
            else if ((GET_INPUT(17063 + 0 * 300) == 8) ||
                (GET_INPUT(17062 + 0 * 300) == 4) ) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 3);
            } else if ((GET_INPUT(17063 + 0 * 300) == 16) ||
                    (GET_INPUT(17062 + 0 * 300) == 2) ) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
            }
            SET_INPUT(NUMBER_OF_WARNING_PCSs, (((GET_INPUT(17060 + 0 * 300) >> 6) & 1) ? 2 : 0));
            SET_INPUT(NUMBER_OF_FAULT_PCSs,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 2 : 0));

             SET_INPUT(PCS_GROUP_STATUS,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 1 : 0));
            if (((GET_INPUT(17063 + 0 * 300) >> 3) & 1) == 1U) {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 2);
            } else {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 0);
            }
           int faulting_BMS_Num = 0;   // 一定要先清零
            for(int fault_BMS=0;fault_BMS<2;fault_BMS++)
            {
                if((GET_INPUT(28071 + (fault_BMS) * 200) == 4))
                {
                    faulting_BMS_Num++;
                }
                else
                {
                    faulting_BMS_Num=faulting_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );


            int Warning_BMS_Num=0;
            for(int Warn_BMS_Num=0;Warn_BMS_Num<2;Warn_BMS_Num++)
            {
                if((GET_INPUT(28040 + (Warn_BMS_Num) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num) * 200) !=0))
                {
                    Warning_BMS_Num++;
                }
                else
                {
                    Warning_BMS_Num=Warning_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );
            int running_bms_num =0;
            for(int Run_BMS_Num=0;Run_BMS_Num<2;Run_BMS_Num++)
            {
                if((GET_INPUT(28072 + (Run_BMS_Num) * 200) !=0))
                {
                    running_bms_num ++;
                }
                else
                {
                    running_bms_num =running_bms_num ;
                }
            }
            SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num  );
             if (((GET_INPUT(17064 + 0 * 300) >> 5) & 1) == 1U) {
                SET_INPUT(Blackstart_mode, 1);
            } else {
                SET_INPUT(Blackstart_mode, 0);
            }

        } else if((GET_INPUT(NUMBER_OF_PCSs) == 4)&&(GET_INPUT(NUMBER_OF_BMSs) == 4))
        {
             INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
            INT16U SOC = (GET_INPUT(28000 + 0 * 200 + 3) + GET_INPUT(28000 + 1 * 200 + 3) +
                            GET_INPUT(28000 + 2 * 200 + 3) + GET_INPUT(28000 + 3 * 200 + 3) ) / BMS_num;
            SET_INPUT(SYSTEM_SOC, SOC);

            INT16U SOH = (GET_INPUT(28000 + 0 * 200 + 4) + GET_INPUT(28000 + 1 * 200 + 4) +
                            GET_INPUT(28000 + 2 * 200 + 4) + GET_INPUT(28000 + 3 * 200 + 4)) / BMS_num;
            SET_INPUT(SYSTEM_SOH, SOH);



            SYS_State_ENUM state[2]; 

            state[0]= Get_State_Sys(0); //获取系统1的状态
            state[1]= Get_State_Sys(1); //获取系统2的状态

            bool sys1_run = (state[0] == SYSRun || state[0] == SYSWarnRun);
            bool sys2_run = (state[1] == SYSRun || state[1] == SYSWarnRun);
            
            if(sys1_run==1)
            {
                INT16S Power_Factor = ((INT16S)GET_INPUT(17000 + 0 * 300 + 42)); 
                SET_INPUT(POWER_FACTOR, Power_Factor);
            }
            else if(sys2_run==1)
            {
                INT16S Power_Factor = ((INT16S)GET_INPUT(17000 + 1 * 300 + 42)); 
                SET_INPUT(POWER_FACTOR, Power_Factor);
            }
            else
            {
                 SET_INPUT(POWER_FACTOR, 0);
            }
            if(GET_INPUT(17000 + 0 * 300 + 43)>=GET_INPUT(17000 + 1 * 300 + 43))
            {
                INT16U ac_freq = (GET_INPUT(17000 + 0 * 300 + 43));
                 SET_INPUT(AC_FREQUENCY, ac_freq);
            }
            else 
            {
                INT16U ac_freq = (GET_INPUT(17000 + 1 * 300 + 43));
                 SET_INPUT(AC_FREQUENCY, ac_freq);
            }
            

            INT32U DC_average_voltage = (GET_INPUT(28000 + 0 * 200 + 0) +
                                           GET_INPUT(28000 + 1 * 200 + 0) +
                                           GET_INPUT(28000 + 2 * 200 + 0) +
                                           GET_INPUT(28000 + 3 * 200 + 0) ) / BMS_num;
            SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
            SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);


            if (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) == 1) 
            {          
               
                SET_INPUT(PCS_GROUP_STATUS, 1);
          
            } 
            

            if (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) == 1) 
            {          
               
                SET_INPUT(PCS_GROUP_STATUS, 2);
            } 
            if ((((GET_INPUT(17060 + 0 * 300) >> 6) & 1) ^ ((GET_INPUT(17060 + 1 * 300) >> 6) & 1)) == 1) {
                SET_INPUT(NUMBER_OF_WARNING_PCSs, 2);
            } else if ((((GET_INPUT(17060 + 0 * 300) >> 6) & 1) == 1) &&
                       (((GET_INPUT(17060 + 1 * 300) >> 6) & 1) == 1)) {
                SET_INPUT(NUMBER_OF_WARNING_PCSs, 4);
            } else {
                SET_INPUT(NUMBER_OF_WARNING_PCSs, 0);
            }
 

            if ((((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ^ ((GET_INPUT(17060 + 1 * 300) >> 4) & 1)) == 1) {
                SET_INPUT(NUMBER_OF_FAULT_PCSs, 2);

                // SET_INPUT(PCS_GROUP_STATUS, 1);
            } else if ((((GET_INPUT(17060 + 0 * 300) >> 4) & 1) == 1) &&
                       (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) == 1)) {
                SET_INPUT(NUMBER_OF_FAULT_PCSs, 4);
                SET_INPUT(PCS_GROUP_STATUS, 3);
            } 

            else {
                SET_INPUT(NUMBER_OF_FAULT_PCSs, 0);
                SET_INPUT(PCS_GROUP_STATUS, 0);
            }
        
            if ((((GET_INPUT(17063 + 0 * 300) >> 3) & 1) ^ ((GET_INPUT(17063 + 1 * 300) >> 3) & 1)) == 1) {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 2);
            } else if ((((GET_INPUT(17063 + 0 * 300) >> 3) & 1) == 1) &&
                       (((GET_INPUT(17063 + 1 * 300) >> 3) & 1) == 1)) {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 4);
            } else {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 0);
            }
             uint16_t master1_state;
            uint16_t master2_state;
            uint16_t slave1_state;
            uint16_t slave2_state;

            uint8_t all_fault;
            uint8_t any_fault;
            uint8_t any_running;
            uint8_t all_standby;

            /*
            * 主机状态：
            * 2  = 待机
            * 8  = 运行
            * 16 = 故障
            */
            master1_state = GET_INPUT(17063 + 0 * 300);
            master2_state = GET_INPUT(17063 + 1 * 300);

            /*
            * 从机状态：
            * 0 = 离线，按照待机处理
            * 1 = 待机
            * 2 = 故障
            * 4 = 运行
            */
            slave1_state = GET_INPUT(17062 + 0 * 300);
            slave2_state = GET_INPUT(17062 + 1 * 300);

            /*
            * 状态1：所有PCS全部故障。
            */
            all_fault =
                (master1_state == 16) &&
                (master2_state == 16) &&
                (slave1_state == 2) &&
                (slave2_state == 2);

            /*
            * 状态4：任意PCS故障。
            */
            any_fault =
                (master1_state == 16) ||
                (master2_state == 16) ||
                (slave1_state == 2) ||
                (slave2_state == 2);

            /*
            * 状态3：任意PCS运行。
            */
            any_running =
                (master1_state == 8) ||
                (master2_state == 8) ||
                (slave1_state == 4) ||
                (slave2_state == 4);

            /*
            * 状态2：所有PCS待机。
            *
            * 主机必须为待机状态2。
            * 从机为待机状态1或离线状态0，都按照待机处理。
            */
            all_standby =
                (master1_state == 2) &&
                (master2_state == 2) &&
                ((slave1_state == 0) || (slave1_state == 1)) &&
                ((slave2_state == 0) || (slave2_state == 1));

            /*
            * 判断优先级：
            * 全部故障 > 任意故障 > 任意运行 > 全部待机。
            */
            if (all_fault)
            {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 1);
            }
            else if (any_fault)
            {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
            }
            else if (any_running)
            {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 3);
            }
            else if (all_standby)
            {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
            }
            else
            {
                /*
                * 未定义状态，避免保留上一次状态。
                */
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 0);
            }
            
            int faulting_BMS_Num = 0;   // 一定要先清零
            for(int fault_BMS=0;fault_BMS<sys_cfg->bmsNum;fault_BMS++)
            {
                if((GET_INPUT(28071 + (fault_BMS) * 200) == 4) )
                {
                    faulting_BMS_Num++;
                }
                else
                {
                    faulting_BMS_Num=faulting_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );
            int Warning_BMS_Num=0;
            for(int Warn_BMS_Num=0;Warn_BMS_Num<sys_cfg->bmsNum;Warn_BMS_Num++)
            {
                if((GET_INPUT(28040 + (Warn_BMS_Num) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num) * 200) !=0))
                {
                    Warning_BMS_Num++;
                }
                else
                {
                    Warning_BMS_Num=Warning_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );

            int running_bms_num =0;
            for(int Run_BMS_Num=0;Run_BMS_Num<sys_cfg->bmsNum;Run_BMS_Num++)
            {
                if((GET_INPUT(28072 + (Run_BMS_Num) * 200) !=0))
                {
                    running_bms_num ++;             
                }
                else
                {
                    running_bms_num =running_bms_num ;
                }
            }
            SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num );

           if ((((GET_INPUT(17064 + 0 * 300) >> 5) & 1) == 1U)&&(((GET_INPUT(17064 + 1 * 300) >> 5) & 1) == 1U)) {
                SET_INPUT(Blackstart_mode, 1);
            } else {
                SET_INPUT(Blackstart_mode, 0);
            }
        }
    }
//该部分数据无论拓扑如何变化都可以这样计算
    /* 系统级数据量 */
    INT32U chg_remain_capacity =
        GET_INPUT(28000 + 0 * 200 + 6) + GET_INPUT(28000 + 1 * 200 + 6) +
        GET_INPUT(28000 + 2 * 200 + 6) + GET_INPUT(28000 + 3 * 200 + 6) ;
    chg_remain_capacity *= 10;
    SET_INPUT(AVAILABLE_CHARGE_ENERGYH, (INT16U)(chg_remain_capacity >> 16));
    SET_INPUT(AVAILABLE_CHARGE_ENERGYL, (INT16U)(chg_remain_capacity & 0xFFFF));

    INT32U dchg_remain_capacity =
        GET_INPUT(28000 + 0 * 200 + 7) + GET_INPUT(28000 + 1 * 200 + 7) +
        GET_INPUT(28000 + 2 * 200 + 7) + GET_INPUT(28000 + 3 * 200 + 7) ;
    dchg_remain_capacity *= 10;
    SET_INPUT(AVAILABLE_DISCHARGE_ENERGYH, (INT16U)(dchg_remain_capacity >> 16));
    SET_INPUT(AVAILABLE_DISCHARGE_ENERGYL, (INT16U)(dchg_remain_capacity & 0xFFFF));

    INT32U chg_power =
        GET_INPUT(28000 + 0 * 200 + 38) + GET_INPUT(28000 + 1 * 200 + 38) +
        GET_INPUT(28000 + 2 * 200 + 38) + GET_INPUT(28000 + 3 * 200 + 38) ;
    chg_power *= 10;
    SET_INPUT(AVAILABLE_CHARGE_POWERH, (INT16U)(chg_power >> 16));
    SET_INPUT(AVAILABLE_CHARGE_POWERL, (INT16U)(chg_power & 0xFFFF));

    INT32U dchg_power =
        GET_INPUT(28000 + 0 * 200 + 37) + GET_INPUT(28000 + 1 * 200 + 37) +
        GET_INPUT(28000 + 2 * 200 + 37) + GET_INPUT(28000 + 3 * 200 + 37) ;
    dchg_power *= 10;
    SET_INPUT(AVAILABLE_DISCHARGE_POWERH, (INT16U)(dchg_power >> 16));
    SET_INPUT(AVAILABLE_DISCHARGE_POWERL, (INT16U)(dchg_power & 0xFFFF));

    INT32S Atvice_power = ((INT16S)GET_INPUT(17000 + 0 * 300 + 40) +  (INT16S)GET_INPUT(17000 + 1 * 300 + 40))*10;
    SET_INPUT(ATVICE_POWERH, (INT16U)(Atvice_power >> 16));
    SET_INPUT(ATVICE_POWERL, (INT16U)(Atvice_power & 0xFFFF));

    INT32S Reactive_power = ((INT16S)GET_INPUT(17000 + 0 * 300 + 41) + (INT16S)GET_INPUT(17000 + 1 * 300 + 41))*10;

    SET_INPUT(REACTIVE_POWERH, (INT16U)(Reactive_power >> 16));
    SET_INPUT(REACTIVE_POWERL, (INT16U)(Reactive_power & 0xFFFF));

    int64_t p164 = (INT16S)GET_INPUT(2600 + 0 * 200 + 18);
    int64_t q164 = (INT16S)GET_INPUT(2600 + 0 * 200 + 19);
    int64_t p264 = (INT16S)GET_INPUT(2600 + 1 * 200 + 18);
    int64_t q264 = (INT16S)GET_INPUT(2600 + 1 * 200 + 19);

    int64_t p364 = (INT16S)GET_INPUT(2600 + 2 * 200 + 18);
    int64_t q364 = (INT16S)GET_INPUT(2600 + 2 * 200 + 19);
    int64_t p464 = (INT16S)GET_INPUT(2600 + 3 * 200 + 18);
    int64_t q464 = (INT16S)GET_INPUT(2600 + 3 * 200 + 19);

    double  s1   = sqrt((double)p164 * (double)p164 + (double)q164 * (double)q164);
    double  s2   = sqrt((double)p264 * (double)p264 + (double)q264 * (double)q264);
    double  s3   = sqrt((double)p364 * (double)p364 + (double)q364 * (double)q364);
    double  s4   = sqrt((double)p464 * (double)p464 + (double)q464 * (double)q464);

    int64_t p64 = Atvice_power;
    int64_t q64 = Reactive_power;
    double  s   = sqrt((double)p64 * (double)p64 + (double)q64 * (double)q64);
    INT32S S = (INT32S)(s+0.5);

    // 同样拆成高低字输出
    SET_INPUT(APPARENT_POWERH, (INT16U)((INT32U)S >> 16));
    SET_INPUT(APPARENT_POWERL, (INT16U)((INT32U)S & 0xFFFF));

// if(sys_cfg->pcsNum==2)
// {
//    INT32U Available_inductive_reactive_powertotal=sqrt((double)5000 * (double)5000 - (double)p64 * (double)p64);


//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF));

//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF));
// }
// else if(sys_cfg->pcsNum==4)
// {
//       INT32U Available_inductive_reactive_powertotal=sqrt((double)10000 * (double)10000 - (double)p64 * (double)p64);


//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF));

//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF)); 
// }
   INT8U PCS_num =COUNT_BITS_16(GET_INPUT(218));
    INT32U Nominal_capctiy = 25000*PCS_num;

    SET_INPUT(NOMINAL_CAPCTIY_H, (INT16U)(Nominal_capctiy >> 16));
    SET_INPUT(NOMINAL_CAPCTIY_L, (INT16U)(Nominal_capctiy & 0xFFFF));
    INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
    INT32U Nominal_energy = 50000*BMS_num;

    SET_INPUT(NOMINAL_ENERGY_H, (INT16U)(Nominal_energy >> 16));
    SET_INPUT(NOMINAL_ENERGY_L, (INT16U)(Nominal_energy & 0xFFFF));

    INT32S DC_power = ((INT16S)GET_INPUT(17000 + 0 * 300 + 54) +  (INT16S)GET_INPUT(17000 + 1 * 300 + 54))*10;
    SET_INPUT(DC_POWERH, (INT16U)(DC_power >> 16));
    SET_INPUT(DC_POWERL, (INT16U)(DC_power & 0xFFFF));

    INT32S DC_current =  ((INT16S)GET_INPUT(17000 + 0 * 300 + 55) +  (INT16S)GET_INPUT(17000 + 1 * 300 + 55))*10;
    SET_INPUT(DC_CURRENTH, (INT16U)(DC_current >> 16));
    SET_INPUT(DC_CURRENTL, (INT16U)(DC_current & 0xFFFF));


    const char *time_str = get_current_time();
int ret = sscanf(time_str,
                 "%u-%u-%u %u:%u:%u",
                 &year,
                 &month,
                 &day,
                 &hour,
                 &minute,
                 &second);

if (ret == 6)
{
    SET_INPUT(Year,   (INT16U)year);
    SET_INPUT(Month,  (INT16U)month);
    SET_INPUT(Day,    (INT16U)day);
    SET_INPUT(Hour,   (INT16U)hour);
    SET_INPUT(Minute, (INT16U)minute);
    SET_INPUT(Second, (INT16U)second);
}

    INT32S PCS1_status = ((INT16S)GET_INPUT(2600 + 0 * 300 + 18));
    INT32S PCS2_status = ((INT16S)GET_INPUT(2600 + 1 * 300 + 18));
    INT32S PCS3_status = ((INT16S)GET_INPUT(2600 + 2 * 300 + 18));
    INT32S PCS4_status = ((INT16S)GET_INPUT(2600 + 3 * 300 + 18));

    SET_INPUT(17000 + 300 * 0 + 32, PCS_Status_To_RegValue(PCS1_status));
    SET_INPUT(17000 + 300 * 0 + 33, PCS_Status_To_RegValue(PCS2_status));
    SET_INPUT(17000 + 300 * 1 + 32, PCS_Status_To_RegValue(PCS3_status));
    SET_INPUT(17000 + 300 * 1 + 33, PCS_Status_To_RegValue(PCS4_status));




    if((GET_INPUT(27327) == 1)&& (GET_INPUT(27326) == 0))//高压侧开关分闸
    {
        SET_INPUT(High_voltage_switch_status,0xEE);
    }
    else if(GET_INPUT(27327) == 0 && GET_INPUT(27326) == 1)//高压侧开关合闸
    {
        SET_INPUT(High_voltage_switch_status,0xAA);
    }

}

static void Event_CMD_Lim_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CMD_LIM);

        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CMD_LIM;
        Clear_In_Sub(sys_num, sub_num);
  

  
}

static void Event_Lim_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CHECK_LIM);

    if (((SUB_EVENT_CMD_LIM + 200) == Get_Out_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CHECK_LIM;
        LOG_INFO("SUB-%d state is %d!", sub_num, eid[sys_num * MAX_SUB_NUM + sub_num]);
    }
}

static void Event_CMD_Power_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CMD_Power);

    if ((SUB_EVENT_CMD_Power == Get_In_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CMD_Power;
        Clear_In_Sub(sys_num, sub_num);
        LOG_INFO("SUB-%d state is %d!", sub_num, eid[sys_num * MAX_SUB_NUM + sub_num]);
        Set_Out_Sub(sys_num, 0, SUB_EVENT_CMD_Power);
        LOG_INFO("sub 里面的sys_num is %d", sys_num);
    } else {
        uint64_t t = Get_Out_Time_Sub(sys_num, sub_num);
        if (t > 100000) {
            Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
            Set_Out_Sub(sys_num, sub_num, SUB_EVENT_CTR_TIMEOUT);
        }
    }
}

static void Event_Power_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_CHECK_Power);

    if (((SUB_EVENT_CMD_Power + 200) == Get_Out_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_CHECK_Power;
        LOG_INFO("SUB-%d state is %d!", sub_num, eid[sys_num * MAX_SUB_NUM + sub_num]);
    }
}

static void Event_Stop_Pass_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_STOP_PASS);
     INT8U bms_data=0;
    if ((bms_data==0) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_STOP_PASS;
        LOG_INFO("SUB-%d is already stopped!", sub_num);
    }
}

static void Event_Reset_Pass_Check(INT16U *eid, INT8U sys_num, INT8U sub_num, SUB_State_ENUM real_state)
{
    (void)real_state;
    INT8U resD = Debug_State_Sub(sys_num, sub_num, SUB_EVENT_RESET_PASS);

    if (((SUB_EVENT_CMD_RESET + 200) == Get_Out_Sub(sys_num, sub_num)) || (1 == resD)) {
        Last_Time[sys_num * MAX_SUB_NUM + sub_num] = Timer_GetTick();
        eid[sys_num * MAX_SUB_NUM + sub_num] = SUB_EVENT_RESET_PASS;
        LOG_INFO("SUB-%d is already reseted!", sub_num);
    }
}

 void system_value1()
{


    sysPara *sys_cfg = SysConf_GetInfo();
    INT16U year, month, day, hour, minute, second;
    uint16_t master1_state;
    uint16_t master2_state;
    uint16_t slave1_state;
    uint16_t slave2_state;

    uint8_t all_fault;
    uint8_t any_fault;
    uint8_t any_running;
    uint8_t all_standby;

//当设备进行切入切出时。通信拓扑会发生变化，然后根据不同设备进行系统赋值
    if (g_enabled_mask_pcs == 0x0C || g_enabled_mask_bms == 0xF0) {
       INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
        INT16U SOC = (GET_INPUT(28000 + 4 * 200 + 3) +
                        GET_INPUT(28000 + 5 * 200 + 3) +
                        GET_INPUT(28000 + 6 * 200 + 3) +
                        GET_INPUT(28000 + 7 * 200 + 3)) / BMS_num;
        SET_INPUT(SYSTEM_SOC, SOC);

        INT16U SOH = (GET_INPUT(28000 + 4 * 200 + 4) +
                        GET_INPUT(28000 + 5 * 200 + 4) +
                        GET_INPUT(28000 + 6 * 200 + 4) +
                        GET_INPUT(28000 + 7 * 200 + 4)) / BMS_num;
        SET_INPUT(SYSTEM_SOH, SOH);


        INT16U ac_freq = GET_INPUT(17000 + 1 * 300 + 43);
        SET_INPUT(AC_FREQUENCY, ac_freq);

        INT16S Power_Factor = (INT16S)GET_INPUT(17000 + 1 * 300 + 42);
        SET_INPUT(POWER_FACTOR, Power_Factor);

        INT32U DC_average_voltage = (GET_INPUT(28000 + 4 * 200 + 0) +
                                       GET_INPUT(28000 + 5 * 200 + 0) +
                                       GET_INPUT(28000 + 6 * 200 + 0) +
                                       GET_INPUT(28000 + 7 * 200 + 0)) / BMS_num;
        SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
        SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);

        if (GET_INPUT(17063 + 1 * 300) == 2) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
        }
        SET_INPUT(NUMBER_OF_WARNING_PCSs, (((GET_INPUT(17060 + 1 * 300) >> 6) & 1) ? 2 : 0));
        SET_INPUT(NUMBER_OF_FAULT_PCSs,   (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) ? 2 : 0));
        SET_INPUT(PCS_GROUP_STATUS,   (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) ? 2 : 0));
        int faulting_BMS_Num = 0;   
        
        for(int fault_BMS=0;fault_BMS<4;fault_BMS++)
        {
            if((GET_INPUT(28071 + (fault_BMS+4) * 200) == 4))
            {
                faulting_BMS_Num++;
            }
            else
            {
                faulting_BMS_Num=faulting_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );
        int Warning_BMS_Num=0;
        for(int Warn_BMS_Num=0;Warn_BMS_Num<4;Warn_BMS_Num++)
        {
            if((GET_INPUT(28040 + (Warn_BMS_Num+4) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num+4) * 200) !=0))
            {
                Warning_BMS_Num++;
            }
            else
            {
                Warning_BMS_Num=Warning_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );
        int running_bms_num =0;
        for(int Run_BMS_Num=0;Run_BMS_Num<4;Run_BMS_Num++)
        {
            if((GET_INPUT(28072 + (Run_BMS_Num+4) * 200) !=0))
            {
                running_bms_num ++;
            }
            else
            {
                running_bms_num =running_bms_num ;
            }
        }
        SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num  );

        if ((GET_INPUT(28072 + 4 * 200) == 2) ||
            (GET_INPUT(28072 + 5 * 200) == 2) ||
            (GET_INPUT(28072 + 6 * 200) == 2) ||
            (GET_INPUT(28072 + 7 * 200) == 2)) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
        } else if ((GET_INPUT(28072 + 4 * 200) == 1) ||
                   (GET_INPUT(28072 + 5 * 200) == 1) ||
                   (GET_INPUT(28072 + 6 * 200) == 1) ||
                   (GET_INPUT(28072 + 7 * 200) == 1)) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 5);
        }

    } else if (g_enabled_mask_pcs == 0x03 || g_enabled_mask_bms == 0x0F) {
      INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
        INT16U SOC = (GET_INPUT(28000 + 0 * 200 + 3) +
                        GET_INPUT(28000 + 1 * 200 + 3) +
                        GET_INPUT(28000 + 2 * 200 + 3) +
                        GET_INPUT(28000 + 3 * 200 + 3)) / BMS_num;
        SET_INPUT(SYSTEM_SOC, SOC);

        INT16U SOH = (GET_INPUT(28000 + 0 * 200 + 4) +
                        GET_INPUT(28000 + 1 * 200 + 4) +
                        GET_INPUT(28000 + 2 * 200 + 4) +
                        GET_INPUT(28000 + 3 * 200 + 4)) / BMS_num;
        SET_INPUT(SYSTEM_SOH, SOH);

        INT16U ac_freq = GET_INPUT(17000 + 0 * 300 + 43);
        SET_INPUT(AC_FREQUENCY, ac_freq);

        INT16S Power_Factor = (INT16S)GET_INPUT(17000 + 0 * 300 + 42);
        SET_INPUT(POWER_FACTOR, Power_Factor);

        INT32U DC_average_voltage = (GET_INPUT(28000 + 0 * 200 + 0) +
                                       GET_INPUT(28000 + 1 * 200 + 0) +
                                       GET_INPUT(28000 + 2 * 200 + 0) +
                                       GET_INPUT(28000 + 3 * 200 + 0)) / BMS_num;
        SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
        SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);

        if (GET_INPUT(17063 + 0 * 300) == 2) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
        }
        SET_INPUT(NUMBER_OF_WARNING_PCSs, (((GET_INPUT(17060 + 0 * 300) >> 6) & 1) ? 2 : 0));
        SET_INPUT(NUMBER_OF_FAULT_PCSs,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 2 : 0));
        SET_INPUT(PCS_GROUP_STATUS,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 1 : 0));
        if ((GET_INPUT(28072 + 0 * 200) == 2) ||
            (GET_INPUT(28072 + 1 * 200) == 2) ||
            (GET_INPUT(28072 + 2 * 200) == 2) ||
            (GET_INPUT(28072 + 3 * 200) == 2)) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
        } else if ((GET_INPUT(28072 + 0 * 200) == 1) ||
                   (GET_INPUT(28072 + 1 * 200) == 1) ||
                   (GET_INPUT(28072 + 2 * 200) == 1) ||
                   (GET_INPUT(28072 + 3 * 200) == 1)) {
            SET_INPUT(STATE_OF_THE_ENERGY_STATION, 5);
        }
        int faulting_BMS_Num = 0;  
        for(int fault_BMS=0;fault_BMS<4;fault_BMS++)
        {
            if((GET_INPUT(28071 + (fault_BMS) * 200) == 4))
            {
                faulting_BMS_Num++;
            }
            else
            {
                faulting_BMS_Num=faulting_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );
        int Warning_BMS_Num=0;
        for(int Warn_BMS_Num=0;Warn_BMS_Num<4;Warn_BMS_Num++)
        {
            if((GET_INPUT(28040 + (Warn_BMS_Num) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num) * 200) !=0))
            {
                Warning_BMS_Num++;
            }
            else
            {
                Warning_BMS_Num=Warning_BMS_Num;
            }
        }
        SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );
        int running_bms_num =0;
        for(int Run_BMS_Num=0;Run_BMS_Num<4;Run_BMS_Num++)
        {
            if((GET_INPUT(28072 + (Run_BMS_Num) * 200) !=0))
            {
                running_bms_num ++;
            }
            else
            {
                running_bms_num =running_bms_num ;
            }
        }
        SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num );

    } else {
        if ((GET_INPUT(NUMBER_OF_PCSs) == 2)&&(GET_INPUT(NUMBER_OF_BMSs) == 4)) {
            INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
            INT16U SOC = (GET_INPUT(28000 + 0 * 200 + 3) +
                            GET_INPUT(28000 + 1 * 200 + 3) +
                            GET_INPUT(28000 + 2 * 200 + 3) +
                            GET_INPUT(28000 + 3 * 200 + 3)) / BMS_num;
            SET_INPUT(SYSTEM_SOC, SOC);

            INT16U SOH = (GET_INPUT(28000 + 0 * 200 + 4) +
                            GET_INPUT(28000 + 1 * 200 + 4) +
                            GET_INPUT(28000 + 2 * 200 + 4) +
                            GET_INPUT(28000 + 3 * 200 + 4)) / BMS_num;
            SET_INPUT(SYSTEM_SOH, SOH);



            INT32U DC_average_voltage = (GET_INPUT(28000 + 0 * 200 + 0) +
                                           GET_INPUT(28000 + 1 * 200 + 0) +
                                           GET_INPUT(28000 + 2 * 200 + 0) +
                                           GET_INPUT(28000 + 3 * 200 + 0)) / BMS_num;
            SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
            SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);
            
            INT16U ac_freq = GET_INPUT(17000 + 0 * 300 + 43);
            SET_INPUT(AC_FREQUENCY, ac_freq);

            INT16S Power_Factor = (INT16S)GET_INPUT(17000 + 0 * 300 + 42);
            SET_INPUT(POWER_FACTOR, Power_Factor);
            if ((GET_INPUT(17063 + 0 * 300) == 2)&&(((GET_INPUT(17062 + 0 * 300) == 1))||((GET_INPUT(17062 + 0 * 300) == 0)))) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
            }
            else if ((GET_INPUT(17063 + 0 * 300) == 16)&&((GET_INPUT(17062 + 0 * 300) == 2))) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 1);
            }            
            else if ((GET_INPUT(17063 + 0 * 300) == 8) ||
                (GET_INPUT(17062 + 0 * 300) == 4) ) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 3);
            } else if ((GET_INPUT(17063 + 0 * 300) == 16) ||
                    (GET_INPUT(17062 + 0 * 300) == 2) ) {
                SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
            }
            SET_INPUT(NUMBER_OF_WARNING_PCSs, (((GET_INPUT(17060 + 0 * 300) >> 6) & 1) ? 2 : 0));
            SET_INPUT(NUMBER_OF_FAULT_PCSs,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 2 : 0));

             SET_INPUT(PCS_GROUP_STATUS,   (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ? 1 : 0));
            if (((GET_INPUT(17063 + 0 * 300) >> 3) & 1) == 1U) {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 2);
            } else {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 0);
            }
           int faulting_BMS_Num = 0;   // 一定要先清零
            for(int fault_BMS=0;fault_BMS<4;fault_BMS++)
            {
                if((GET_INPUT(28071 + (fault_BMS) * 200) == 4))
                {
                    faulting_BMS_Num++;
                }
                else
                {
                    faulting_BMS_Num=faulting_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );


            int Warning_BMS_Num=0;
            for(int Warn_BMS_Num=0;Warn_BMS_Num<4;Warn_BMS_Num++)
            {
                if((GET_INPUT(28040 + (Warn_BMS_Num) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num) * 200) !=0))
                {
                    Warning_BMS_Num++;
                }
                else
                {
                    Warning_BMS_Num=Warning_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );
            int running_bms_num =0;
            for(int Run_BMS_Num=0;Run_BMS_Num<4;Run_BMS_Num++)
            {
                if((GET_INPUT(28072 + (Run_BMS_Num) * 200) !=0))
                {
                    running_bms_num ++;
                }
                else
                {
                    running_bms_num =running_bms_num ;
                }
            }
            SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num  );
             if ((((GET_INPUT(17064 + 0 * 300) >> 5) & 1) == 1U)) {
                SET_INPUT(Blackstart_mode, 1);
            } else {
                SET_INPUT(Blackstart_mode, 0);
            }

        } else if((GET_INPUT(NUMBER_OF_PCSs) == 4)&&(GET_INPUT(NUMBER_OF_BMSs) == 8))
        {
             INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
            INT16U SOC = (GET_INPUT(28000 + 0 * 200 + 3) + GET_INPUT(28000 + 1 * 200 + 3) +
                            GET_INPUT(28000 + 2 * 200 + 3) + GET_INPUT(28000 + 3 * 200 + 3) +
                            GET_INPUT(28000 + 4 * 200 + 3) + GET_INPUT(28000 + 5 * 200 + 3) +
                            GET_INPUT(28000 + 6 * 200 + 3) + GET_INPUT(28000 + 7 * 200 + 3)) / BMS_num;
            SET_INPUT(SYSTEM_SOC, SOC);

            INT16U SOH = (GET_INPUT(28000 + 0 * 200 + 4) + GET_INPUT(28000 + 1 * 200 + 4) +
                            GET_INPUT(28000 + 2 * 200 + 4) + GET_INPUT(28000 + 3 * 200 + 4) +
                            GET_INPUT(28000 + 4 * 200 + 4) + GET_INPUT(28000 + 5 * 200 + 4) +
                            GET_INPUT(28000 + 6 * 200 + 4) + GET_INPUT(28000 + 7 * 200 + 4)) / BMS_num;
            SET_INPUT(SYSTEM_SOH, SOH);

            SYS_State_ENUM state[2]; 

            state[0]= Get_State_Sys(0); //获取系统1的状态
            state[1]= Get_State_Sys(1); //获取系统2的状态

            bool sys1_run = (state[0] == SYSRun || state[0] == SYSWarnRun);
            bool sys2_run = (state[1] == SYSRun || state[1] == SYSWarnRun);
            
            if(sys1_run==1)
            {
                INT16S Power_Factor = ((INT16S)GET_INPUT(17000 + 0 * 300 + 42)); 
                SET_INPUT(POWER_FACTOR, Power_Factor);
            }
            else if(sys2_run==1)
            {
                INT16S Power_Factor = ((INT16S)GET_INPUT(17000 + 1 * 300 + 42)); 
                SET_INPUT(POWER_FACTOR, Power_Factor);
            }
            else
            {
                 SET_INPUT(POWER_FACTOR, 0);
            }

            if(GET_INPUT(17000 + 0 * 300 + 43)>=GET_INPUT(17000 + 1 * 300 + 43))
            {
                INT16U ac_freq = (GET_INPUT(17000 + 0 * 300 + 43));
                 SET_INPUT(AC_FREQUENCY, ac_freq);
            }
            else 
            {
                INT16U ac_freq = (GET_INPUT(17000 + 1 * 300 + 43));
                 SET_INPUT(AC_FREQUENCY, ac_freq);
            }
            

            INT32U DC_average_voltage = (GET_INPUT(28000 + 0 * 200 + 0) +
                                           GET_INPUT(28000 + 1 * 200 + 0) +
                                           GET_INPUT(28000 + 2 * 200 + 0) +
                                           GET_INPUT(28000 + 3 * 200 + 0) +
                                           GET_INPUT(28000 + 4 * 200 + 0) +
                                           GET_INPUT(28000 + 5 * 200 + 0) +
                                           GET_INPUT(28000 + 6 * 200 + 0) +
                                           GET_INPUT(28000 + 7 * 200 + 0)) / BMS_num;
            SET_INPUT(DC_AVERAGE_VOLTAGEH, DC_average_voltage >> 16);
            SET_INPUT(DC_AVERAGE_VOLTAGEL, DC_average_voltage & 0xFFFF);


            if (((GET_INPUT(17060 + 0 * 300) >> 4) & 1) == 1) 
            {          
               
                SET_INPUT(PCS_GROUP_STATUS, 1);
          
            } 
            

            if (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) == 1) 
            {          
               
                SET_INPUT(PCS_GROUP_STATUS, 2);
            } 
            if ((((GET_INPUT(17060 + 0 * 300) >> 6) & 1) ^ ((GET_INPUT(17060 + 1 * 300) >> 6) & 1)) == 1) {
                SET_INPUT(NUMBER_OF_WARNING_PCSs, 2);
            } else if ((((GET_INPUT(17060 + 0 * 300) >> 6) & 1) == 1) &&
                       (((GET_INPUT(17060 + 1 * 300) >> 6) & 1) == 1)) {
                SET_INPUT(NUMBER_OF_WARNING_PCSs, 4);
            } else {
                SET_INPUT(NUMBER_OF_WARNING_PCSs, 0);
            }
 

            if ((((GET_INPUT(17060 + 0 * 300) >> 4) & 1) ^ ((GET_INPUT(17060 + 1 * 300) >> 4) & 1)) == 1) {
                SET_INPUT(NUMBER_OF_FAULT_PCSs, 2);

                // SET_INPUT(PCS_GROUP_STATUS, 1);
            } else if ((((GET_INPUT(17060 + 0 * 300) >> 4) & 1) == 1) &&
                       (((GET_INPUT(17060 + 1 * 300) >> 4) & 1) == 1)) {
                SET_INPUT(NUMBER_OF_FAULT_PCSs, 4);
                SET_INPUT(PCS_GROUP_STATUS, 3);
            } 

            else {
                SET_INPUT(NUMBER_OF_FAULT_PCSs, 0);
                SET_INPUT(PCS_GROUP_STATUS, 0);
            }
        
            if ((((GET_INPUT(17063 + 0 * 300) >> 3) & 1) ^ ((GET_INPUT(17063 + 1 * 300) >> 3) & 1)) == 1) {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 2);
            } else if ((((GET_INPUT(17063 + 0 * 300) >> 3) & 1) == 1) &&
                       (((GET_INPUT(17063 + 1 * 300) >> 3) & 1) == 1)) {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 4);
            } else {
                SET_INPUT(NUMBER_OF_RUNNING_PCSs, 0);
            }

    /*
     * 主机状态：
     * 2  = 待机
     * 8  = 运行
     * 16 = 故障
     */
    master1_state = GET_INPUT(17063 + 0 * 300);
    master2_state = GET_INPUT(17063 + 1 * 300);

    /*
     * 从机状态：
     * 0 = 离线，按照待机处理
     * 1 = 待机
     * 2 = 故障
     * 4 = 运行
     */
    slave1_state = GET_INPUT(17062 + 0 * 300);
    slave2_state = GET_INPUT(17062 + 1 * 300);

    /*
     * 状态1：所有PCS全部故障。
     */
    all_fault =
        (master1_state == 16) &&
        (master2_state == 16) &&
        (((slave1_state)>> 1)&0x01 == 1) &&
        (((slave2_state)>> 1)&0x01 == 1);

    /*
     * 状态4：任意PCS故障。
     */
    any_fault =
        (master1_state == 16) ||
        (master2_state == 16) ||
        (((slave1_state)>> 1)&0x01 == 1) ||
        (((slave2_state)>> 1)&0x01 == 1);

    /*
     * 状态3：任意PCS运行。
     */
    any_running =
        (master1_state == 8) ||
        (master2_state == 8) ||
        (((slave1_state)>> 2)&0x01 == 1) ||
        (((slave2_state)>> 2)&0x01 == 1);

    /*
     * 状态2：所有PCS待机。
     *
     * 主机必须为待机状态2。
     * 从机为待机状态1或离线状态0，都按照待机处理。
     */
    all_standby =
        (master1_state == 2) &&
        (master2_state == 2) &&
        ((slave1_state == 0) || (((slave1_state)>> 0)&0x01 == 1)) &&
        ((slave2_state == 0) || (((slave2_state)>> 0)&0x01 == 1));

    /*
     * 判断优先级：
     * 全部故障 > 任意故障 > 任意运行 > 全部待机。
     */
    if (all_fault)
    {
        SET_INPUT(STATE_OF_THE_ENERGY_STATION, 1);
    }
    else if (any_fault)
    {
        SET_INPUT(STATE_OF_THE_ENERGY_STATION, 4);
    }
    else if (any_running)
    {
        SET_INPUT(STATE_OF_THE_ENERGY_STATION, 3);
    }
    else if (all_standby)
    {
        SET_INPUT(STATE_OF_THE_ENERGY_STATION, 2);
    }
    else
    {
        /*
         * 未定义状态，避免保留上一次状态。
         */
        SET_INPUT(STATE_OF_THE_ENERGY_STATION, 0);
    }



            int faulting_BMS_Num = 0;   // 一定要先清零
            for(int fault_BMS=0;fault_BMS<sys_cfg->bmsNum;fault_BMS++)
            {
                if((GET_INPUT(28071 + (fault_BMS) * 200) == 4) )
                {
                    faulting_BMS_Num++;
                }
                else
                {
                    faulting_BMS_Num=faulting_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_FAULT_BMSs, faulting_BMS_Num );
            int Warning_BMS_Num=0;
            for(int Warn_BMS_Num=0;Warn_BMS_Num<sys_cfg->bmsNum;Warn_BMS_Num++)
            {
                if((GET_INPUT(28040 + (Warn_BMS_Num) * 200) !=0)||(GET_INPUT(28041 + (Warn_BMS_Num) * 200) !=0))
                {
                    Warning_BMS_Num++;
                }
                else
                {
                    Warning_BMS_Num=Warning_BMS_Num;
                }
            }
            SET_INPUT(NUMBER_OF_WARNING_BMSs, Warning_BMS_Num );

            int running_bms_num =0;
            for(int Run_BMS_Num=0;Run_BMS_Num<sys_cfg->bmsNum;Run_BMS_Num++)
            {
                if((GET_INPUT(28072 + (Run_BMS_Num) * 200) !=0))
                {
                    running_bms_num ++;
     
             
                }
                else
                {
                    running_bms_num =running_bms_num ;
                }
            }
            SET_INPUT(NUMBER_OF_RUNNING_BMSs, running_bms_num );
           if ((((GET_INPUT(17064 + 0 * 300) >> 5) & 1) == 1U)&&(((GET_INPUT(17064 + 1 * 300) >> 5) & 1) == 1U)) {
                SET_INPUT(Blackstart_mode, 1);
            } else {
                SET_INPUT(Blackstart_mode, 0);
            }
        }
    }
//该部分数据无论拓扑如何变化都可以这样计算
    /* 系统级数据量 */
    INT32U chg_remain_capacity =
        (GET_INPUT(28000 + 0 * 200 + 6) + GET_INPUT(28000 + 1 * 200 + 6) +
        GET_INPUT(28000 + 2 * 200 + 6) + GET_INPUT(28000 + 3 * 200 + 6) +
        GET_INPUT(28000 + 4 * 200 + 6) + GET_INPUT(28000 + 5 * 200 + 6) +
        GET_INPUT(28000 + 6 * 200 + 6) + GET_INPUT(28000 + 7 * 200 + 6))*10;
    SET_INPUT(AVAILABLE_CHARGE_ENERGYH, (INT16U)(chg_remain_capacity >> 16));
    SET_INPUT(AVAILABLE_CHARGE_ENERGYL, (INT16U)(chg_remain_capacity & 0xFFFF));

    INT32U dchg_remain_capacity =
        (GET_INPUT(28000 + 0 * 200 + 7) + GET_INPUT(28000 + 1 * 200 + 7) +
        GET_INPUT(28000 + 2 * 200 + 7) + GET_INPUT(28000 + 3 * 200 + 7) +
        GET_INPUT(28000 + 4 * 200 + 7) + GET_INPUT(28000 + 5 * 200 + 7) +
        GET_INPUT(28000 + 6 * 200 + 7) + GET_INPUT(28000 + 7 * 200 + 7))*10;
    SET_INPUT(AVAILABLE_DISCHARGE_ENERGYH, (INT16U)(dchg_remain_capacity >> 16));
    SET_INPUT(AVAILABLE_DISCHARGE_ENERGYL, (INT16U)(dchg_remain_capacity & 0xFFFF));

    INT32U chg_power =
        (GET_INPUT(28000 + 0 * 200 + 38) + GET_INPUT(28000 + 1 * 200 + 38) +
        GET_INPUT(28000 + 2 * 200 + 38) + GET_INPUT(28000 + 3 * 200 + 38) +
        GET_INPUT(28000 + 4 * 200 + 38) + GET_INPUT(28000 + 5 * 200 + 38) +
        GET_INPUT(28000 + 6 * 200 + 38) + GET_INPUT(28000 + 7 * 200 + 38))*10;
    SET_INPUT(AVAILABLE_CHARGE_POWERH, (INT16U)(chg_power >> 16));
    SET_INPUT(AVAILABLE_CHARGE_POWERL, (INT16U)(chg_power & 0xFFFF));

    INT32U dchg_power =
        (GET_INPUT(28000 + 0 * 200 + 37) + GET_INPUT(28000 + 1 * 200 + 37) +
        GET_INPUT(28000 + 2 * 200 + 37) + GET_INPUT(28000 + 3 * 200 + 37) +
        GET_INPUT(28000 + 4 * 200 + 37) + GET_INPUT(28000 + 5 * 200 + 37) +
        GET_INPUT(28000 + 6 * 200 + 37) + GET_INPUT(28000 + 7 * 200 + 37))*10;

    SET_INPUT(AVAILABLE_DISCHARGE_POWERH, (INT16U)(dchg_power >> 16));
    SET_INPUT(AVAILABLE_DISCHARGE_POWERL, (INT16U)(dchg_power & 0xFFFF));

    INT32S Atvice_power = ((INT16S)GET_INPUT(17000 + 0 * 300 + 40) +  (INT16S)GET_INPUT(17000 + 1 * 300 + 40))*10;
    SET_INPUT(ATVICE_POWERH, (INT16U)(Atvice_power >> 16));
    SET_INPUT(ATVICE_POWERL, (INT16U)(Atvice_power & 0xFFFF));

    INT32S Reactive_power = ((INT16S)GET_INPUT(17000 + 0 * 300 + 41) + (INT16S)GET_INPUT(17000 + 1 * 300 + 41))*10;

    SET_INPUT(REACTIVE_POWERH, (INT16U)(Reactive_power >> 16));
    SET_INPUT(REACTIVE_POWERL, (INT16U)(Reactive_power & 0xFFFF));

    int64_t p64 = (int64_t)Atvice_power;
    int64_t q64 = (int64_t)Reactive_power;
    double  s   = sqrt((double)p64 * (double)p64 + (double)q64 * (double)q64);

    INT32S S = (INT32S)(s+0.5);

    // 同样拆成高低字输出
    SET_INPUT(APPARENT_POWERH, (INT16U)((INT32U)S >> 16));
    SET_INPUT(APPARENT_POWERL, (INT16U)((INT32U)S & 0xFFFF));

// if(sys_cfg->pcsNum==2)
// {
//    INT32U Available_inductive_reactive_powertotal=sqrt((double)5000 * (double)5000 - (double)p64 * (double)p64);


//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF));

//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF));
// }
// else if(sys_cfg->pcsNum==4)
// {
//       INT32U Available_inductive_reactive_powertotal=sqrt((double)10000 * (double)10000 - (double)p64 * (double)p64);


//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_INDUCTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF));

//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERH, (INT16U)(Available_inductive_reactive_powertotal >> 16));
//     SET_INPUT(AVAILABLE_CAPACTIVE_REACTIVE_POWERL, (INT16U)(Available_inductive_reactive_powertotal & 0xFFFF)); 
// }
   INT8U PCS_num =COUNT_BITS_16(GET_INPUT(218));
    INT32U Nominal_capctiy = 25000*PCS_num;

    SET_INPUT(NOMINAL_CAPCTIY_H, (INT16U)(Nominal_capctiy >> 16));
    SET_INPUT(NOMINAL_CAPCTIY_L, (INT16U)(Nominal_capctiy & 0xFFFF));
 INT8U BMS_num =COUNT_BITS_16(GET_INPUT(220));
    INT32U Nominal_energy = 50000*BMS_num;

    SET_INPUT(NOMINAL_ENERGY_H, (INT16U)(Nominal_energy >> 16));
    SET_INPUT(NOMINAL_ENERGY_L, (INT16U)(Nominal_energy & 0xFFFF));

    INT32S DC_power = ((INT16S)GET_INPUT(17000 + 0 * 300 + 54) +  (INT16S)GET_INPUT(17000 + 1 * 300 + 54))*10;
    SET_INPUT(DC_POWERH, (INT16U)(DC_power >> 16));
    SET_INPUT(DC_POWERL, (INT16U)(DC_power & 0xFFFF));

    INT32S DC_current = ((INT16S)GET_INPUT(17000 + 0 * 300 + 55) + (INT16S)GET_INPUT(17000 + 1 * 300 + 55))*10;
    SET_INPUT(DC_CURRENTH, (INT16U)(DC_current >> 16));
    SET_INPUT(DC_CURRENTL, (INT16U)(DC_current & 0xFFFF));

 INT32U Total_Daily_discharged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 26)<<16) + GET_INPUT(2600 + 0 * 300 + 27))+((GET_INPUT(2600 + 0 * 300 + 66)<<16) + GET_INPUT(2600 + 0 * 300 + 67))+
 ((GET_INPUT(2600 + 1 * 300 + 26)<<16) + GET_INPUT(2600 + 1 * 300 + 27))+((GET_INPUT(2600 + 1 * 300 + 66)<<16) + GET_INPUT(2600 + 1 * 300 + 67))+
 ((GET_INPUT(2600 + 2 * 300 + 26)<<16) + GET_INPUT(2600 + 2 * 300 + 27))+((GET_INPUT(2600 + 2 * 300 + 66)<<16) + GET_INPUT(2600 + 2 * 300 + 67))+
 ((GET_INPUT(2600 + 3 * 300 + 26)<<16) + GET_INPUT(2600 + 3 * 300 + 27))+((GET_INPUT(2600 + 3 * 300 + 66)<<16) + GET_INPUT(2600 + 3 * 300 + 67))
 )*10;
     SET_INPUT(Daily_discharged_EnergyH, (INT16U)(Total_Daily_discharged_Energy >> 16));
     SET_INPUT(Daily_discharged_EnergyL, (INT16U)(Total_Daily_discharged_Energy & 0xFFFF));

 INT32U Total_Daily_charged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 28)<<16) + GET_INPUT(2600 + 0 * 300 + 29))+((GET_INPUT(2600 + 0 * 300 + 68)<<16) + GET_INPUT(2600 + 0 * 300 + 69))+
 ((GET_INPUT(2600 + 1 * 300 + 28)<<16) + GET_INPUT(2600 + 1 * 300 + 29))+((GET_INPUT(2600 + 1 * 300 + 68)<<16) + GET_INPUT(2600 + 1 * 300 + 69))+
 ((GET_INPUT(2600 + 2 * 300 + 28)<<16) + GET_INPUT(2600 + 2 * 300 + 29))+((GET_INPUT(2600 + 2 * 300 + 68)<<16) + GET_INPUT(2600 + 2 * 300 + 69))+
 ((GET_INPUT(2600 + 3 * 300 + 28)<<16) + GET_INPUT(2600 + 3 * 300 + 29))+((GET_INPUT(2600 + 3 * 300 + 68)<<16) + GET_INPUT(2600 + 3 * 300 + 69))
 )*10;
     SET_INPUT(Daily_charged_EnergyH, (INT16U)(Total_Daily_charged_Energy >> 16));
     SET_INPUT(Daily_charged_EnergyL, (INT16U)(Total_Daily_charged_Energy & 0xFFFF));


 INT32U Total_Total_discharged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 42)<<16) + GET_INPUT(2600 + 0 * 300 + 43))+((GET_INPUT(2600 + 0 * 300 + 74)<<16) + GET_INPUT(2600 + 0 * 300 + 75))+
 ((GET_INPUT(2600 + 1 * 300 + 42)<<16) + GET_INPUT(2600 + 1 * 300 + 43))+((GET_INPUT(2600 + 1 * 300 + 74)<<16) + GET_INPUT(2600 + 1 * 300 + 75))+
 ((GET_INPUT(2600 + 2 * 300 + 42)<<16) + GET_INPUT(2600 + 2 * 300 + 43))+((GET_INPUT(2600 + 2 * 300 + 74)<<16) + GET_INPUT(2600 + 2 * 300 + 75))+
 ((GET_INPUT(2600 + 3 * 300 + 42)<<16) + GET_INPUT(2600 + 3 * 300 + 43))+((GET_INPUT(2600 + 3 * 300 + 74)<<16) + GET_INPUT(2600 + 3 * 300 + 75))
 )*10;
     SET_INPUT(Total_discharged_EnergyH, (INT16U)(Total_Total_discharged_Energy >> 16));
     SET_INPUT(Total_discharged_EnergyL, (INT16U)(Total_Total_discharged_Energy & 0xFFFF));

 INT32U Total_Total_charged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 44)<<16) + GET_INPUT(2600 + 0 * 300 + 45))+((GET_INPUT(2600 + 0 * 300 + 76)<<16) + GET_INPUT(2600 + 0 * 300 + 77))+
 ((GET_INPUT(2600 + 1 * 300 + 44)<<16) + GET_INPUT(2600 + 1 * 300 + 45))+((GET_INPUT(2600 + 1 * 300 + 76)<<16) + GET_INPUT(2600 + 1 * 300 + 77))+
 ((GET_INPUT(2600 + 2 * 300 + 44)<<16) + GET_INPUT(2600 + 2 * 300 + 45))+((GET_INPUT(2600 + 2 * 300 + 76)<<16) + GET_INPUT(2600 + 2 * 300 + 77))+
 ((GET_INPUT(2600 + 3 * 300 + 44)<<16) + GET_INPUT(2600 + 3 * 300 + 45))+((GET_INPUT(2600 + 3 * 300 + 76)<<16) + GET_INPUT(2600 + 3 * 300 + 77))
 )*10;
     SET_INPUT(Total_charged_EnergyH, (INT16U)(Total_Total_charged_Energy >> 16));
     SET_INPUT(Total_charged_EnergyL, (INT16U)(Total_Total_charged_Energy & 0xFFFF));
////

INT32U AC_Total_Daily_discharged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 98)<<16) + GET_INPUT(2600 + 0 * 300 + 99))+
 ((GET_INPUT(2600 + 1 * 300 + 98)<<16) + GET_INPUT(2600 + 1 * 300 + 99))+
 ((GET_INPUT(2600 + 2 * 300 + 98)<<16) + GET_INPUT(2600 + 2 * 300 + 99))+
 ((GET_INPUT(2600 + 3 * 300 + 98)<<16) + GET_INPUT(2600 + 3 * 300 + 99))
 )*10;
     SET_INPUT(AC_Daily_discharged_EnergyH, (INT16U)(AC_Total_Daily_discharged_Energy >> 16));
     SET_INPUT(AC_Daily_discharged_EnergyL, (INT16U)(AC_Total_Daily_discharged_Energy & 0xFFFF));

 INT32U AC_Total_Daily_charged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 100)<<16) + GET_INPUT(2600 + 0 * 300 + 101))+
 ((GET_INPUT(2600 + 1 * 300 + 100)<<16) + GET_INPUT(2600 + 1 * 300 + 101))+
 ((GET_INPUT(2600 + 2 * 300 + 100)<<16) + GET_INPUT(2600 + 2 * 300 + 101))+
 ((GET_INPUT(2600 + 3 * 300 + 100)<<16) + GET_INPUT(2600 + 3 * 300 + 101))
 )*10;
     SET_INPUT(AC_Daily_charged_EnergyH, (INT16U)(AC_Total_Daily_charged_Energy >> 16));
     SET_INPUT(AC_Daily_charged_EnergyL, (INT16U)(AC_Total_Daily_charged_Energy & 0xFFFF));


 INT32U AC_Total_Total_discharged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 106)<<16) + GET_INPUT(2600 + 0 * 300 + 107))+
 ((GET_INPUT(2600 + 1 * 300 + 106)<<16) + GET_INPUT(2600 + 1 * 300 + 107))+
 ((GET_INPUT(2600 + 2 * 300 + 106)<<16) + GET_INPUT(2600 + 2 * 300 + 107))+
 ((GET_INPUT(2600 + 3 * 300 + 106)<<16) + GET_INPUT(2600 + 3 * 300 + 107))
 )*10;
     SET_INPUT(AC_Total_discharged_EnergyH, (INT16U)(AC_Total_Total_discharged_Energy >> 16));
     SET_INPUT(AC_Total_discharged_EnergyL, (INT16U)(AC_Total_Total_discharged_Energy & 0xFFFF));

 INT32U AC_Total_Total_charged_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 108)<<16) + GET_INPUT(2600 + 0 * 300 + 109))+
 ((GET_INPUT(2600 + 1 * 300 + 108)<<16) + GET_INPUT(2600 + 1 * 300 + 109))+
 ((GET_INPUT(2600 + 2 * 300 + 108)<<16) + GET_INPUT(2600 + 2 * 300 + 109))+
 ((GET_INPUT(2600 + 3 * 300 + 108)<<16) + GET_INPUT(2600 + 3 * 300 + 109))
 )*10;
     SET_INPUT(AC_Total_charged_EnergyH, (INT16U)(AC_Total_Total_charged_Energy >> 16));
     SET_INPUT(AC_Total_charged_EnergyL, (INT16U)(AC_Total_Total_charged_Energy & 0xFFFF));
///
INT32U AC_Daily_Cap_Reactive_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 122)<<16) + GET_INPUT(2600 + 0 * 300 + 123))+
 ((GET_INPUT(2600 + 1 * 300 + 122)<<16) + GET_INPUT(2600 + 1 * 300 + 123))+
 ((GET_INPUT(2600 + 2 * 300 + 122)<<16) + GET_INPUT(2600 + 2 * 300 + 123))+
 ((GET_INPUT(2600 + 3 * 300 + 122)<<16) + GET_INPUT(2600 + 3 * 300 + 123))
 )*10;
     SET_INPUT(AC_Daily_Cap_Reactive_EnergyH, (INT16U)(AC_Daily_Cap_Reactive_Energy >> 16));
     SET_INPUT(AC_Daily_Cap_Reactive_EnergyL, (INT16U)(AC_Daily_Cap_Reactive_Energy & 0xFFFF));

 INT32U AC_Daily_Ind_Reactive_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 124)<<16) + GET_INPUT(2600 + 0 * 300 + 125))+
 ((GET_INPUT(2600 + 1 * 300 + 124)<<16) + GET_INPUT(2600 + 1 * 300 + 125))+
 ((GET_INPUT(2600 + 2 * 300 + 124)<<16) + GET_INPUT(2600 + 2 * 300 + 125))+
 ((GET_INPUT(2600 + 3 * 300 + 124)<<16) + GET_INPUT(2600 + 3 * 300 + 125))
 )*10;
     SET_INPUT(AC_Daily_Ind_Reactive_EnergyH, (INT16U)(AC_Daily_Ind_Reactive_Energy >> 16));
     SET_INPUT(AC_Daily_Ind_Reactive_EnergyL, (INT16U)(AC_Daily_Ind_Reactive_Energy & 0xFFFF));


 INT32U AC_Total_Cap_Reactive_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 130)<<16) + GET_INPUT(2600 + 0 * 131 + 107))+
 ((GET_INPUT(2600 + 1 * 300 + 130)<<16) + GET_INPUT(2600 + 1 * 131 + 107))+
 ((GET_INPUT(2600 + 2 * 300 + 130)<<16) + GET_INPUT(2600 + 2 * 131 + 107))+
 ((GET_INPUT(2600 + 3 * 300 + 130)<<16) + GET_INPUT(2600 + 3 * 131 + 107))
 )*10;
     SET_INPUT(AC_Total_Cap_Reactive_EnergyH, (INT16U)(AC_Total_Cap_Reactive_Energy >> 16));
     SET_INPUT(AC_Total_Cap_Reactive_EnergyL, (INT16U)(AC_Total_Cap_Reactive_Energy & 0xFFFF));

 INT32U AC_Total_Ind_Reactive_Energy = 
 (((GET_INPUT(2600 + 0 * 300 + 132)<<16) + GET_INPUT(2600 + 0 * 300 + 133))+
 ((GET_INPUT(2600 + 1 * 300 + 132)<<16) + GET_INPUT(2600 + 1 * 300 + 133))+
 ((GET_INPUT(2600 + 2 * 300 + 132)<<16) + GET_INPUT(2600 + 2 * 300 + 133))+
 ((GET_INPUT(2600 + 3 * 300 + 132)<<16) + GET_INPUT(2600 + 3 * 300 + 133))
 )*10;
     SET_INPUT(AC_Total_Ind_Reactive_EnergyH, (INT16U)(AC_Total_Ind_Reactive_Energy >> 16));
     SET_INPUT(AC_Total_Ind_Reactive_EnergyL, (INT16U)(AC_Total_Ind_Reactive_Energy & 0xFFFF));

///
    const char *time_str = get_current_time();
int ret = sscanf(time_str,
                 "%u-%u-%u %u:%u:%u",
                 &year,
                 &month,
                 &day,
                 &hour,
                 &minute,
                 &second);

if (ret == 6)
{
    SET_INPUT(Year,   (INT16U)year);
    SET_INPUT(Month,  (INT16U)month);
    SET_INPUT(Day,    (INT16U)day);
    SET_INPUT(Hour,   (INT16U)hour);
    SET_INPUT(Minute, (INT16U)minute);
    SET_INPUT(Second, (INT16U)second);
}
    if((GET_INPUT(27631) == 1)&& (GET_INPUT(27632) == 0))//高压侧开关分闸
    {
        SET_INPUT(High_voltage_switch_status,0xEE);
    }
    else if(GET_INPUT(27631) == 0 && GET_INPUT(27632) == 1)//高压侧开关合闸
    {
        SET_INPUT(High_voltage_switch_status,0xAA);
    }
    INT32S PCS1_status = ((INT16S)GET_INPUT(2600 + 0 * 300 + 18));
    INT32S PCS2_status = ((INT16S)GET_INPUT(2600 + 1 * 300 + 18));
    INT32S PCS3_status = ((INT16S)GET_INPUT(2600 + 2 * 300 + 18));
    INT32S PCS4_status = ((INT16S)GET_INPUT(2600 + 3 * 300 + 18));

    SET_INPUT(17000 + 300 * 0 + 32, PCS_Status_To_RegValue(PCS1_status));
    SET_INPUT(17000 + 300 * 0 + 33, PCS_Status_To_RegValue(PCS2_status));
    SET_INPUT(17000 + 300 * 1 + 32, PCS_Status_To_RegValue(PCS3_status));
    SET_INPUT(17000 + 300 * 1 + 33, PCS_Status_To_RegValue(PCS4_status));
}

void SUB_State_Run(volatile SUB_State_ENUM *state, INT8U sys_num, INT8U sub_num)
{
    for (INT8U i = 0; i < LIST_SIZE; i++) {
        if (state_list[i].now_state == state[sys_num * MAX_SUB_NUM + sub_num]) {

            check_limit(sys_num, sub_num);
            state_list[i].fun((INT16U *)Event_Id, sys_num, sub_num,
                              state[sys_num * MAX_SUB_NUM + sub_num]);

            if (state_list[i].eventId == Event_Id[sys_num * MAX_SUB_NUM + sub_num]) {
                state[sys_num * MAX_SUB_NUM + sub_num] = state_list[i].next_state;
                if (state_list[i].string != NULL) {
                    LOG_INFO("sys-%d sub-%d %s", sys_num, sub_num, state_list[i].string);
                }
            }
        }
    }
}

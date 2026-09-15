#include "main.h"
#include "log.h"
#include "timed_check.h"
#include "state_sys.h"
#include "state_task.h"
#include "blackstart_sys.h"
#include <stdarg.h>


static void slave_pcs_clear_points_on_fault(int pcs);
static void pcs_clear_points_on_fault(int pcs_group);
/* ===================== 通信断联→强制回初始 ===================== */
static volatile INT8U g_force_init_req[MAX_SYS_NUM] = {0};
static uint64_t         g_force_init_last_ts[MAX_SYS_NUM] = {0};  /* 防抖（10ms tick） */

static volatile INT8U Standby_Cmd_Zero_Power[MAX_SYS_NUM] = {0};//MV一体机下发待机命令标志位

static INT8U  clear_pending[MAX_SYS_NUM] = {0};
static uint64_t clear_req_ts[MAX_SYS_NUM]  = {0};
static INT32U last_time[MAX_SYS_NUM]; 


#define MAX_PCS 4


static INT8U  s_prev_comm_fault[MAX_PCS] = {0};  /* 0:正常, 1:故障 */
static INT8U  s_inited = 0;


extern INT8U BusType;

extern uint8_t black_start ;
extern uint8_t black_end   ;
/* ===================== 清点位：PCS 断联时用 ===================== */
/**
 * @brief 清除故障相关的点位数据
 * 
 * 该函数用于在故障发生时清除指定PCS和PCS组的点位数据，包括INPUT和HOLD寄存器中的特定范围。
 * 清除操作通过将指定范围的点位设置为0来实现。
 * 
 * @param pcs PCS单元编号，用于计算INPUT和HOLD寄存器的偏移量
 * @param pcs_group PCS组编号，用于计算INPUT和HOLD寄存器的偏移量
 * 
 * @note 清除的点位范围包括：
 *       - INPUT 2600~2898 (偏移 pcs*300)
 *       - INPUT 17000~17298 (偏移 pcs_group*300)
 *       - HOLD 12000~12298 (偏移 pcs*300)
 *       - HOLD 27000~27298 (偏移 pcs_group*300)
 * 
 * @return 无返回值
 */
static void pcs_clear_points_on_fault(int pcs_group)
{
    // /* 清 INPUT 2600~2898 (偏移 pcs*300) */
    // for (int i = 2600; i < 2899; ++i)
    //     SET_INPUT(i + pcs * 300, 0);

    /* 清 INPUT 17000~17298 (偏移 group*300) */
    for (int i = 17000; i < 17299; ++i)
        SET_INPUT(i + pcs_group * 300, 0);

    // /* 清 HOLD 12000~12298 (偏移 pcs*300) */
    // for (int i = 12000; i < 12299; ++i)
    //     SET_HOLD(i + pcs * 300, 0);

    /* 清 HOLD 27000~27298 (偏移 group*300) */
    for (int i = 27000; i < 27299; ++i)
        SET_HOLD(i + pcs_group * 300, 0);
}

static void slave_pcs_clear_points_on_fault(int pcs)
{
    /* 清 INPUT 2600~2898 (偏移 pcs*300) */
    for (int i = 2600; i < 2899; ++i)
        SET_INPUT(i + pcs * 300, 0);

    // /* 清 INPUT 17000~17298 (偏移 group*300) */
    // for (int i = 17000; i < 17299; ++i)
    //     SET_INPUT(i + pcs_group * 300, 0);

    /* 清 HOLD 12000~12298 (偏移 pcs*300) */
    for (int i = 12000; i < 12299; ++i)
        SET_HOLD(i + pcs * 300, 0);

    // /* 清 HOLD 27000~27298 (偏移 group*300) */
    // for (int i = 27000; i < 27299; ++i)
    //     SET_HOLD(i + pcs_group * 300, 0);
}








static INT32U EMS_GetSec(void)
{

    return (INT32U)(Timer_GetTick() / 100U);
}




/* ===================== 运行时全局（事件/计时等） ===================== */



static INT16U volatile event_id     [MAX_SYS_NUM] = {0};

static INT16U volatile event_in     [MAX_SYS_NUM] = {0};
static INT16U volatile event_out    [MAX_SYS_NUM] = {0};
static uint64_t volatile event_in_time [MAX_SYS_NUM] = {0};
static uint64_t volatile event_out_time[MAX_SYS_NUM] = {0};
    /* 每个系统独立计数：最多允许 5 次触发清理 */
    static INT8U slave_clean_times[MAX_SYS_NUM] = {0};  // SYS_NUM_MAX 改成项目里的系统数上限
/* ===================== 事件检测函数原型 ===================== */
static void Event_Init_Check          (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Fault_Check         (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Start_Check     (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Stop_Check      (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Fault_Stop_Check    (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Stop_Check          (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Stop_Check_PCS(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Island_CMD_Send(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
void Island_CMD_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_BLACKRUN_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
/* ===================== 状态转移表 ===================== */
const static StateSYSform state_list[] = {
    /* 当前状态 ************                事件 ******************目标状态 ********* 检测函数 **********************************描述 */
    {SYSInit,                       SYS_EVENT_INIT,             SYSStopped,             Event_Init_Check,                   "黑启动事件：初始化->停机"},

    {SYSStopPCS,                    SYS_EVENT_WAIT_STOPED,      SYSStopped,             Event_Stop_Check,                   "下发停机：PCS停机->PCS已停机"},

    {SYSFault,                      SYS_EVENT_FAULT_STOP,       SYSStopPCS,             Event_Fault_Stop_Check,             "停机命令：故障停机->停机中"},

    {SYSStopped,                    SYS_EVENT_ISLAND_CMD,       SYSBlackStopsetvalue,   Island_CMD_Send,                    "孤岛参数命令下发：停止->下发孤岛参数"},
   
    {SYSBlackStopsetvalue,          SYS_EVENT_ISLAND_CMD,       SYSBlackStopcheckvalue, Island_CMD_Check,                   "黑启动事件：已停机->孤岛参数已下发"},
 
    {SYSBlackStopcheckvalue,        SYS_EVENT_CMD_START,        SYSBLACKSTART,          Event_CMD_Start_Check,              "黑启动命令：已停机->启动中"},
   
  
    {SYSBLACKSTART,                 SYS_EVENT_FAULT,            SYSFault,               Event_Fault_Check,                  "故障事件：黑启动中->故障停机"},
   
    {SYSBLACKSTART,                 SYS_EVENT_CMD_BLACK_RUN,    SYSBLACKRun,            Event_SYS_BLACKRUN_Check,            "黑启动运行：启动中->黑启动运行"},

     {SYSBLACKRun,                  SYS_EVENT_ZERO_POWER,       SYSStopPCS,             Event_CMD_Stop_Check,               "停机命令：运行->停机中"},

    {SYSBLACKRun,                   SYS_EVENT_STOP,             SYSInit,                Event_CMD_Stop_Check_PCS,           "停机命令：运行->停机"},   

};

#define LIST_SIZE (sizeof(state_list) / sizeof(StateSYSform))





/**
 * @brief 初始化事件检查函数
 * @details 检查并处理系统初始化相关事件，包括事件队列初始化和初始化超时检测
 * 
 * @param eid 事件ID数组指针，用于存储或更新系统事件
 * @param sys_num 系统编号，用于标识当前处理的系统
 * @param real_state 系统实际状态枚举值（当前未使用，保留参数）
 * 
 * @note 函数会重置所有系统的事件队列(event_in/event_out)
 * @note 对于未初始化的系统，会记录初始时间并设置初始化标志
 * @note 对于已初始化的系统，会检查5秒超时条件，超时后触发初始化事件
 * @note 包含对特殊字符(\t, \r, \n)的处理
 * 
 * @return 无返回值
 */
static void Event_Init_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    static INT8U init_flag[MAX_SYS_NUM] = {0};

    for (int i = 0; i < MAX_SYS_NUM; i++) 
    {
        event_in[i]  = 0;
        event_out[i] = 0;
    }
      Clear_In_Sys(sys_num);
      
    //   LOG_INFO("event_in :%d,sys_num : %d", event_in[sys_num],sys_num);
    if (init_flag[sys_num] == 0) {
        last_time[sys_num]  = Timer_GetTick();
        init_flag[sys_num]  = 1;
    } 
    else 
    {
        if (!Soft_Timer(last_time[sys_num], 5 * 1000)) 
        {
            last_time[sys_num] = Timer_GetTick();
            slave_clean_times[sys_num]=0;
            *(eid + sys_num)   = SYS_EVENT_INIT;
        }
    }
}
/**
 * @brief 故障停止检查事件处理函数
 * 
 * 该函数用于处理系统故障停止检查事件，主要功能包括：
 * 1. 更新系统最后运行时间
 * 2. 设置故障停止事件ID
 * 3. 下发0功率指令
 * 4. 清除系统输入状态
 * 
 * @param eid 事件ID数组指针，用于存储系统事件
 * @param sys_num 系统编号，标识当前处理的系统
 * @param real_state 系统实际状态（本函数中未使用，仅保留参数）
 * 
 * @note 函数内部会忽略real_state参数
 * @note 会调用Timer_GetTick()获取当前时间戳
 * @note 会调用Set_Out_Sys()和Clear_In_Sys()进行系统控制
 */
static void Event_Fault_Stop_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    last_time[sys_num] = Timer_GetTick();
    *(eid + sys_num)   = SYS_EVENT_FAULT_STOP;
    Set_Out_Sys(sys_num, SYS_EVENT_ZERO_POWER); /* 下发 0 功率 */
    Clear_In_Sys(sys_num);
}

/**
 * @brief 检查系统故障状态并更新事件ID
 * 
 * 该函数检查指定系统的故障状态，包括子系统故障和系统故障总信号。
 * 如果检测到故障，将更新对应的事件ID并记录故障发生时间。
 * 
 * @param[out] eid 事件ID数组指针，用于存储检测到的事件ID
 * @param[in] sys_num 系统编号，指定要检查的系统
 * @param[in] real_state 系统实际状态（当前未使用）
 * 
 * @note 函数会检查以下故障条件：
 *       - 任意子系统处于故障状态
 *       - 系统故障总信号有效
 *       - 调试模式下模拟故障状态
 * 
 * @note 函数内部维护静态变量s_inited和s_prev_comm_fault用于初始化和通信故障记录
 */

void Island_CMD_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    SUB_State_ENUM state;
    INT8U island_value = false;
    sysPara *sys_cfg = SysConf_GetInfo();

    if ((GET_HOLD(27000+300*sys_num+111) == 100)&&(GET_HOLD(27000+300*sys_num+112) == 5000))  island_value = true;  
    if ((island_value == true)) 
    {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_ISLAND_CMD;
    }
}






static void Island_CMD_Send(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    int pcs_num=0;
    int pcs_addr=0;
    INT8U  is_Fault = IsNoFault;
    INT16U PCS_sys_state       = GET_INPUT(17000 + 300 * sys_num + 60);

    if ((((PCS_sys_state >> 1) & 0x1u) == 1u))
    {
        is_Fault = IsNoFault;
    }

    if ((is_Fault == IsNoFault) ) 
    {
        pcs_num = (sys_num!= 0) ? 2 : 0;
        pcs_addr = (sys_num!= 0) ? 1 : 0;
     if (pcs_num >= 0)
    {
        PcsWriteReq pcsreq;
        pcsreq.addr     = 27000+300*sys_num+111;   //下发孤岛电压比例
        pcsreq.value    = 100;
        pcsreq.len      = 1;
        pcsreq.is_multi = false;

        // 去重，保留最新值
        LOG_INFO("addr is %d,num is %d",pcsreq.addr ,pcs_num);
        Pcs_Write_Enqueue_Dedup_By_Addr(pcs_num, &pcsreq);//放入缓冲区中
    }
     if (pcs_num >= 0)
    {
        PcsWriteReq pcsreq;
        pcsreq.addr     = 27000+300*sys_num+112;   //下发孤岛频率
        pcsreq.value    = 5000;//先默认50HZ//后续放到配置文件中进行配置 
        pcsreq.len      = 1;
        pcsreq.is_multi = false;

        // 去重，保留最新值
        LOG_INFO("addr is %d,num is %d",pcsreq.addr ,pcs_num);
        Pcs_Write_Enqueue_Dedup_By_Addr(pcs_num, &pcsreq);//放入缓冲区中
    }


        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_ISLAND_CMD;

    }
}


/**
 * @brief 处理系统启动命令检查事件
 * 
 * 该函数用于检查系统状态并处理启动命令。主要功能包括：
 * 1. 检查所有PCS（功率转换系统）是否处于关闭状态
 * 2. 如果所有PCS都关闭，则启动计时器并在100ms后发送关闭RMU信号
 * 3. 检查是否收到启动命令，如果是则记录事件ID和最后触发时间
 * 
 * @param eid 事件ID数组指针，用于存储触发的事件
 * @param sys_num 系统编号，标识当前处理的系统
 * @param real_state 系统实际状态（当前未使用，保留参数）
 * 
 * @note 函数中包含被注释掉的BMS（电池管理系统）状态检查代码
 * @note 计时器精度为100ms
 * @note 函数会修改全局变量：measureflag, measurecloseflag, last_time[]
 */
static void Event_CMD_Start_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
     sysPara *sys_cfg = SysConf_GetInfo();
        INT16U all_close = 1;
        INT16U pcs_sys_state=0;
        uint64_t flag_set_start = 0;
        INT8U flag_timer_started = 0;
        int pcs_num=0;
        int pcs_addr=0;
    if ((SYS_EVENT_CMD_START == Get_In_Sys(sys_num))) 
        {
        last_time[sys_num] = Timer_GetTick();

        pcs_num = (sys_num!= 0) ? 2 : 0;
        pcs_addr = (sys_num!= 0) ? 1 : 0;
     if (pcs_num >= 0)
    {
        PcsWriteReq pcsreq;
        pcsreq.addr     = 27000+300*sys_num+103;   //下发开机
        pcsreq.value    = 1;
        pcsreq.len      = 1;
        pcsreq.is_multi = false;

        // 去重，保留最新值
        LOG_INFO("addr is %d,num is %d",pcsreq.addr ,pcs_num);
        Pcs_Write_Enqueue_Dedup_By_Addr(pcs_num, &pcsreq);//放入缓冲区中
    }



        *(eid + sys_num)   = SYS_EVENT_CMD_START;
   
        }
}


/**
 * @brief 检查并处理停止命令事件
 * @details 当系统接收到停止命令时，执行关机操作并更新系统状态
 * @param eid 指向事件ID数组的指针，用于存储处理后的系统事件ID
 * @param sys_num 系统编号，用于标识具体的系统实例
 * @param real_state 系统实际状态（未使用，保留参数）
 * @note 函数会：
 *       - 检查系统是否收到停止命令（SYS_EVENT_CMD_STOP）
 *       - 设置保持寄存器（27107 + sys_num * 300）为0
 *       - 将系统输出状态设置为零功率状态（SYS_EVENT_ZERO_POWER）
 *       - 记录关机日志
 *       - 更新最后操作时间
 *       - 更新事件ID并清除系统输入状态
 * @warning 该函数会修改eid指向的数组和系统状态
 */
static void Event_CMD_Stop_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    int pcs_num=0;
    int pcs_addr=0;
    if ((SYS_EVENT_CMD_STOP == Get_In_Sys(sys_num)) ||
        (Debug_State_Sys(sys_num, SYS_EVENT_CMD_STOP) == 1)) 
    {

        pcs_num = (sys_num!= 0) ? 2 : 0;
        pcs_addr = (sys_num!= 0) ? 1 : 0;
     if (pcs_num >= 0)
    {
        PcsWriteReq pcsreq;
        pcsreq.addr     = 27000+300*sys_num+103;   
        pcsreq.value    = 0;
        pcsreq.len      = 1;
        pcsreq.is_multi = false;

        // 去重，保留最新值
        LOG_INFO("addr is %d,num is %d",pcsreq.addr ,pcs_num);
        Pcs_Write_Enqueue_Dedup_By_Addr(pcs_num, &pcsreq);//放入缓冲区中
    }
        LOG_INFO("下发关机: sys_num=%d", sys_num);

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_ZERO_POWER;
        Clear_In_Sys(sys_num);
        LOG_INFO("event_in :%d,sys_num : %d", event_in[sys_num],sys_num);
    }
}

static void Event_CMD_Stop_Check_PCS_Status(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    INT16U PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 60);
    INT8U  master_fault  = (INT8U)(((PCS_sys_state >> 4) & 0x1u) == 1u);
    INT8U  master_no_run  = (INT8U)(((PCS_sys_state >> 3) & 0x1u) == 1u);
    INT16U slave_PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 62);
    INT8U  slave_fault = (INT8U)(((slave_PCS_sys_state >> 1) & 0x1u) == 1u);
    INT8U  slave_no_run = (INT8U)(((slave_PCS_sys_state >> 0) & 0x1u) == 1u);
    if ((master_fault==1) && ((slave_fault==1)||(slave_no_run==1)||(slave_PCS_sys_state==0))
) 
    {

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_STOP;
        Clear_In_Sys(sys_num);
        Clear_Out_Sys(sys_num);
    }
}

static void Event_CMD_Stop_Check_PCS(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    INT16U PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 60);
    INT8U  master_fault  = (INT8U)(((PCS_sys_state >> 4) & 0x1u) == 1u);
    INT8U  master_no_run  = (INT8U)(((PCS_sys_state >> 3) & 0x1u) == 1u);
    INT16U slave_PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 62);
    INT8U  slave_fault = (INT8U)(((slave_PCS_sys_state >> 1) & 0x1u) == 1u);
    INT8U  slave_no_run = (INT8U)(((slave_PCS_sys_state >> 0) & 0x1u) == 1u);
    if (((master_fault==1)||(master_no_run!=1)) && ((slave_fault==1)||(slave_no_run==1)||(slave_PCS_sys_state==0))
) 
    {

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_STOP;
        Clear_In_Sys(sys_num);
        Clear_Out_Sys(sys_num);
    }
}






/**
 * @brief 检查并执行子系统停止事件
 * 
 * @param eid 事件ID数组指针，用于存储系统事件
 * @param sys_num 系统编号
 * @param real_state 系统实际状态（未使用）
 * 
 * @details
 * 该函数用于检查PCS系统状态，并根据特定条件触发子系统停止命令。
 * 当满足以下任一条件时，会向所有启用的子系统发送停止命令：
 * 1. PCS状态位1为1且位2、位3不为1
 * 2. PCS状态位1为0
 * 3. PCS状态位4为1
 * 
 * 停止命令会发送给系统中所有启用的子系统，并记录最后操作时间。
 * 同时更新事件ID为SYS_EVENT_STOP_SUB。
 */
static void Event_Stop_PCS_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    INT8U PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 60); /* 主机PCS当前状态 */
    INT8U PCS_slave_state = GET_INPUT(17000 + 300 * sys_num + 62); /* 从机PCS当前状态 */

   //LOG_INFO("warningflag[%d]:%d",sys_num,warningflag[sys_num]);
   if ((((((PCS_sys_state >> 1 )&1u)== 1)&&(((PCS_sys_state >> 2 )&1u)!= 1)&&(((PCS_sys_state >> 3 )&1u)!= 1))|| (((PCS_sys_state >> 1 )&1u)== 0)|| (((PCS_sys_state >> 4 )&1u)== 1))&&((((PCS_slave_state >>0 )&1u)== 1)||(((PCS_slave_state >>1 )&1u)== 1)||(PCS_slave_state==0)) ) 
   {
    // if (((PCS_sys_state == 2) || (PCS_sys_state == 0)) ) {    
        for (INT8U sub = 0; (sub < sys_cfg->subNum) && (sub < MAX_SUB_NUM); sub++) 
        {
            if (!SUB_ENABLED_SYS(sys_num, sub)) continue;
            Set_In_Sub(sys_num, sub, SUB_EVENT_CMD_STOP);
            LOG_INFO("停止命令: sys=%d, sub=%d", sys_num, sub);
        }
        last_time[sys_num] = Timer_GetTick();
    
        *(eid + sys_num)   = SYS_EVENT_STOP_SUB;
    }
}

/**
 * @brief 检查系统停止事件
 * @details 根据PCS系统状态和警告标志，判断是否需要触发系统停止事件
 * 
 * @param [out] eid 指向事件ID数组的指针，用于存储触发的事件ID
 * @param [in] sys_num 系统编号，用于标识当前检查的是哪个系统
 * @param [in] real_state 系统实际状态（本函数中未使用）
 * 
 * @note 函数会检查以下条件来决定是否触发停止事件：
 *       1. PCS系统状态满足特定条件（运行状态且非充电状态且非放电状态，或为故障状态）
 *       2. 当前系统存在警告标志
 * @warning 函数会修改全局变量last_time和warningflag
 */
static void Event_Stop_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    INT8U all_safe = 1;
    INT8U sub_begin = (INT8U)(sys_num * BMS_PER_SYS);       
    INT8U sub_end   = (INT8U)(sub_begin + BMS_PER_SYS);
    INT8U PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 60);
  
    if ((((((PCS_sys_state >> 1 )&1u)== 1)&&(((PCS_sys_state >> 2 )&1u)!= 1)&&(((PCS_sys_state >> 3 )&1u)!= 1))|| (((PCS_sys_state >> 4 )&1u)== 1)))
    {
 
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_WAIT_STOPED;
    }
 }


#define WARN_DEBOUNCE_TICKS_100ms   (10)   /* 10ms tick -> 5s */





static void Event_SYS_BLACKRUN_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
 INT8U  is_Fault = IsFault;
    sysPara *sys_cfg = SysConf_GetInfo();
    INT8U is_no_run = 0;
     INT16U PCS_sys_state       = GET_INPUT(17000 + 300 * sys_num + 60);

    if ((((PCS_sys_state >> 3) & 0x1u) == 1u))
    {
        is_Fault = IsNoFault;
    }
 

  if(is_Fault == IsNoFault)
    { 
    last_time[sys_num] = Timer_GetTick();
   *(eid + sys_num)   = SYS_EVENT_CMD_BLACK_RUN;
        
    }
}

static void Event_Fault_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    SUB_State_ENUM state;
    INT8U is_Fault = IsNoFault;
    sysPara *sys_cfg = SysConf_GetInfo();

    if (!s_inited) 
    {
        for (int k = 0; k < MAX_PCS; ++k) s_prev_comm_fault[k] = 0;
        s_inited = 1;
    }

    for (INT8U sub = 0; sub < sys_cfg->subNum && sub < MAX_SUB_NUM; ++sub) 
    {
        if (!SUB_ENABLED_SYS(sys_num, sub)) continue;
        state = Get_State_Sub(sys_num, sub);
        if (state == SUBFault) { is_Fault = IsFault; break; }
    }
    if (GET_INPUT(SYSTEM_FAULT_SUMMARY) == 1)      is_Fault = IsFault;  /* 系统故障总 */
    if ((is_Fault == IsFault) || Debug_State_Sys(sys_num, SYS_EVENT_FAULT)) 
    {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_FAULT;
    }
}

/**
 * @brief 检查并处理PCS（Power Conversion System）限制相关逻辑
 * 
 * 该函数主要执行以下功能：
 * 1. 监控P2P模式变化，当检测到P2P模式从1变为0时，更新BMS和PCS的状态字及数量
 * 2. 检测PCS通信状态，处理通信故障和恢复情况
 * 3. 在特定条件下触发系统强制初始化
 * 
 * @note 函数内部使用静态变量记录上次P2P模式状态，避免重复触发
 * @note 包含防抖处理（300个tick周期）
 * @note 仅当主机PCS（PCS0或PCS2）断连时才会触发告警和状态机复位
 */
static void check_pcs_limit(void)
{
    sysPara *sys_cfg = SysConf_GetInfo();

        static INT8S last_p2p_mode = -1;                  // -1 表示尚未初始化
        INT16U cur_p2p = GET_INPUT(P2P_mode);

        if (last_p2p_mode < 0) 
        {
        
            last_p2p_mode = (INT8S)(cur_p2p ? 1 : 0);
        } else 
        {
            if (last_p2p_mode == 1 && cur_p2p == 0) 
            {
                    /* ---- 触发：1 -> 0 ---- */
            if(sys_cfg->bmsNum==8)
            {
            SET_INPUT(STATUS_WORD3,0xFF);
            
            
            BMS_Unpack_To_Set_Hold(0xFF);
            SET_INPUT(NUMBER_OF_BMSs,sys_cfg->bmsNum);
            }
            else if(sys_cfg->bmsNum==4)
            {
            SET_INPUT(STATUS_WORD3,0xF);
            
                BMS_Unpack_To_Set_Hold(0xF);
            SET_INPUT(NUMBER_OF_BMSs,sys_cfg->bmsNum);

            }
            else
            {
            SET_INPUT(STATUS_WORD3,0x3);
            BMS_Unpack_To_Set_Hold(0x3);
            SET_INPUT(NUMBER_OF_BMSs,sys_cfg->bmsNum);
            }


            if(sys_cfg->pcsNum==4)
            {
            SET_INPUT(STATUS_WORD1,0xF);
            
            PCS_Unpack_To_Set_Hold(0xF);

            SET_INPUT(NUMBER_OF_PCSs,sys_cfg->pcsNum);
            }
            else if(sys_cfg->pcsNum==2)
            {
            SET_INPUT(STATUS_WORD1,0x3); 
            PCS_Unpack_To_Set_Hold(0x3);
            SET_INPUT(NUMBER_OF_PCSs,sys_cfg->pcsNum);    
            }
            else
            {
            SET_INPUT(STATUS_WORD1,0x1); 
                PCS_Unpack_To_Set_Hold(0x1);
            SET_INPUT(NUMBER_OF_PCSs,sys_cfg->pcsNum);  
            }
            /*版本号*/
            SET_INPUT(LC_PROTOCOL_VERSION,14);
            SET_INPUT(SOFTWARE_MAJOR_VERSION,1);
            SET_INPUT(SOFTWARE_MINOR_VERSION_H,1);
            SET_INPUT(SOFTWARE_MINOR_VERSION_L,1);
            SET_INPUT(SOFTWARE_SPC_VERSION,105);

            LOG_INFO("P2P_mode 1->0: masks updated (BMS=%d, PCS=%d).", sys_cfg->bmsNum, sys_cfg->pcsNum);
            }
            /* 更新上一拍 */
            last_p2p_mode = (INT8S)(cur_p2p ? 1 : 0);
        }

            INT16U statusword = (INT16U)GET_INPUT(STATUS_WORD1);

    for (int pcs = 0; pcs < sys_cfg->pcsNum && pcs < MAX_PCS; ++pcs) 
    {
        int curr_fault = (Get_PCS_Comm(pcs) == 1);  /* 1=通信故障 */
       
        /* 正常 -> 故障（上升沿） */
        if (curr_fault && !s_prev_comm_fault[pcs]) 
        {
            /* 清在线位 */
            statusword &= (INT16U)~(1u << pcs);
            SET_INPUT(STATUS_WORD1, statusword);
          
           slave_pcs_clear_points_on_fault(pcs);

            /* （3s 防抖：10ms tick => 300） */
            if((pcs==0)||(pcs==2))//只有当主机1PCS或者主机2PCS断连的时候才告警并让状态机回到初始状态
            {
                uint64_t now = Timer_GetTick();


                int sys_num = (pcs == 2) ? 1 : 0;

                 /* 清一次点位（避免每周期清） */
                 pcs_clear_points_on_fault(sys_num);

                if (now - g_force_init_last_ts[sys_num] > 300) 
                {
                    g_force_init_last_ts[sys_num] = now;
                    g_force_init_req[sys_num]     = 1;
                    LOG_INFO("SYS-%d request FORCE-INIT due to PCS-%d comm lost.", sys_num, pcs);
                }
            }
            LOG_INFO("PCS-%d communication FAULT (edge). Points cleared.", pcs);
        }
        /* 故障 -> 恢复（下降沿） */
        else if (!curr_fault && s_prev_comm_fault[pcs]==1) 
        {
            statusword |= (INT16U)(1u << pcs);
            SET_INPUT(STATUS_WORD1, statusword);
            LOG_INFO("PCS-%d communication RECOVERED (edge).", pcs);
        }

        s_prev_comm_fault[pcs] = (INT8S)curr_fault;
    }
}
/**
 * @brief 检查系统运行状态并更新事件ID
 * 
 * @param event_id 事件ID数组指针，用于存储系统运行事件
 * @param sys_num 系统编号，标识要检查的系统
 * @param real_state 系统当前的实际状态（SYS_State_ENUM类型）
 * 
 * @note 该函数会检查系统运行状态，并在满足条件时更新事件ID
 *       检查条件包括：
 *       1. 系统未停止(is_no_run==0)且输出状态为SYS_EVENT_START_PCS+200
 *       2. 或系统调试状态返回1(resD==1)
 *       满足任一条件时，会更新last_time和event_id
 * 
 * @warning 函数内部使用了Timer_GetTick()获取系统时间，需确保定时器已初始化
 *          函数内部使用了SysConf_GetInfo()获取系统配置，需确保配置已加载
 * 
 * @return 无返回值

/*
/* ===================== 全局状态聚合 + 锁存 ===================== */


#ifndef WARNRUN
#define WARNRUN  (0x1234u) 
#endif

/* ========== 全局状态聚合：sys0/sys1 缓存 ========== */
static volatile INT16U g_sys_status_cache[MAX_SYS_NUM] = {0};
static volatile INT8U  g_sys_status_seen_mask = 0;

/* ========== 每个 sys 的告警/报警锁存（出现后保持，直到清除条件满足） ========== */
static volatile INT16U g_sys_alarm_latch[MAX_SYS_NUM] = {0}; /* 0 表示未锁存 */


static INT8U sys_state_rank(INT16U st)
{
    switch (st) {
    case SYSRun:      return 13;   
    case SYSSTARTPQ: return 12;
    case SYSPQMode: return 11;
    case SYSStartPCS: return 10;
    case SYSStartSUB: return 9;  
    case SYSStarting: return 8;
    case SYSStandby:  return 7;
    case SYSStopping: return 6;
    case SYSZEROPOWER:  return 5;
    case SYSSTOPPQ:  return 4;
    case SYSSTOPPQED:  return 3;
    case SYSClearFAULT:return 2;
    case SYSInit:     return 1;
    default:          return 0;
    }
}
static INT8U sys_state_rank_simple(INT16U st)
{
    switch (st) {
    case SYSStarting: return 80;
    case SYSRun:      return 70;

    case SYSStopping: return 50;


    case SYSStopped:  return 30;
    case SYSClearFAULT:
    case SYSInit:     return 20;

    default:          return 10;
    }
}

/* 严重度优先级：ALARM > WARN > WARNRUN > 其他(0) */
static INT8U severity_rank(INT16U st)
{
    switch (st) {
    case FAULT:   return 4;
    case ALARM:   return 3;
    case WARN:   
    case WARNRUN: return 1;
    default:      return 0;
    }
}


/**
 * @brief 计算系统输出状态
 * 
 * 根据当前系统状态(sys_num)和当前状态(now_state)计算并返回系统的输出状态。
 * 该函数处理包括警告、故障、初始化、停止、运行等多种系统状态的映射。
 * 
 * @param sys_num 系统编号（当前未使用，保留用于未来扩展）
 * @param now_state 当前系统状态，取值范围包括：
 *                   - SYSZWARNINGSTOP: 系统警告停止
 *                   - SYSWarn: 系统警告
 *                   - SYSWarnRun: 系统警告运行
 *                   - SYSFault: 系统故障
 *                   - SYSInit: 系统初始化
 *                   - SYSStopping: 系统停止中
 *                   - SYSStopped: 系统已停止
 *                   - SYSStandby: 系统待机
 *                   - SYSRun: 系统运行
 *                   - SYSStarting: 系统启动中
 *                   - SYSClearPower: 清除功率
 *                   - SYSStopPCS: 停止PCS
 *                   - SYSStopSUB: 停止SUB
 *                   - SYSStartSUB: 启动SUB
 *                   - SYSStartPCS: 启动PCS
 *                   - SYSClearFAULT: 清除故障
 *                   - SYSPQMode: PQ模式
 *                   - SYSSTARTPQ: 启动PQ
 *                   - SYSSTOPPQED: 停止PQ
 *                   - SYSZEROPOWER: 零功率
 * 
 * @return INT16U 返回计算后的系统输出状态，可能的值包括：
 *                 - ALARM: 报警状态
 *                 - WARN: 警告状态
 *                 - FAULT: 故障状态
 *                 - SYSInit: 初始化状态
 *                 - 或其他与输入状态相同的值
 * 
 * @note 特殊处理：
 *       1. 当now_state为SYSInit时，会检查STATUS_WORD1的值：
 *          - 若为0xF、0x5、0x13或0x7（PCS断连情况），返回SYSInit
 *          - 否则返回ALARM
 *       2. SYSWarnRun状态单独映射为WARN
 *       3. 函数中未处理的输入状态将返回未定义值
 */
static INT16U calc_sys_status_out(INT8U sys_num, INT16U now_state)
{
    (void)sys_num;
    sysPara *sys_cfg = SysConf_GetInfo();
    switch (now_state) 
    {
    case SYSZWARNINGSTOP:
        return ALARM;

    case SYSWarn:
        return WARN;

    case SYSWarnRun:
        return WARN;  /* 关键：单独映射 */
    case SYSFault:
        return FAULT; 
    case SYSInit:
 
    if ((GET_INPUT(STATUS_WORD1) == 0xF)||(GET_INPUT(STATUS_WORD1) == 0x5)||(GET_INPUT(STATUS_WORD1) == 0x13)||(GET_INPUT(STATUS_WORD1) == 0x7)||(GET_INPUT(STATUS_WORD1) == 0x12)||(GET_INPUT(STATUS_WORD1) == 0x3)||(GET_INPUT(STATUS_WORD1) == 0x7)||(GET_INPUT(STATUS_WORD1) == 0x1)||(GET_INPUT(STATUS_WORD1) == 0x4)){//当存在PCS断连时，系统状态会到ALARM，当PCS全在线或者整组在线的时候，系统状态回到init
        return SYSInit;
    } 
    else 
    {
    
        return ALARM;
    }

    case SYSStopping:
        return SYSStopping;

    case SYSStopped:
        return SYSStopped;

    case SYSStandby:
        return SYSStandby;

    case SYSRun:
        return SYSRun;

    case SYSStarting:
        return SYSStarting;

    case SYSClearPower:
        return SYSClearPower;

    case SYSStopPCS:
        return SYSStopPCS;

    case SYSStopSUB:
        return SYSStopSUB;   
    case SYSStartSUB:
        return SYSStartSUB;

    case SYSStartPCS:
        return SYSStartPCS;

    case SYSClearFAULT:
        return SYSClearFAULT;   


    case SYSPQMode:
        return SYSPQMode;

    case SYSSTARTPQ:
        return SYSSTARTPQ;   
    case SYSSTOPPQED:
        return SYSSTOPPQED;

    case SYSZEROPOWER:
        return SYSZEROPOWER;
  
    }
}


/**
 * @brief 检查指定系统的PCS通信状态
 * 
 * 该函数用于检查指定系统(sys_num)对应的两个PCS(Power Conversion System)的通信状态。
 * 根据系统编号确定对应的PCS编号，并检查这些PCS是否通信正常。
 * 
 * @param sys_num 系统编号，用于确定对应的PCS编号
 *                - 0: 对应PCS编号为0和1
 *                - 1: 对应PCS编号为2和3
 * 
 * @return INT8U 返回通信状态
 *               - 0: 表示至少有一个PCS通信正常
 *               - 1: 表示所有PCS通信都失败
 * 
 * @note 函数内部使用SysConf_GetInfo()获取系统配置信息
 *       通过Get_PCS_Comm()函数获取指定PCS的通信状态
 */
static INT8U sys_comm_ok(INT8U sys_num)
{
    sysPara *cfg = SysConf_GetInfo();

    INT8U pcs0 = (sys_num == 0) ? 0 : 2;
    INT8U pcs1 = (sys_num == 0) ? 1 : 3;

    if (pcs0 < cfg->pcsNum) {
        if (Get_PCS_Comm(pcs0) == 1) return 0;
    }
    if (pcs1 < cfg->pcsNum) {
        if (Get_PCS_Comm(pcs1) == 1) return 0;
    }
    return 1;
}


//当系统从ALARM状态恢复时，进行清锁存操作。清除之后就可以显示恢复之后的状态
/**
 * @brief 检查指定系统的报警源是否已清除
 * 
 * 该函数用于检查指定系统的报警源是否满足以下条件：
 * 1. 系统通信正常
 * 2. 系统无警告标志
 * 3. 系统故障寄存器已清零
 * 
 * @param sys_num 系统编号，用于标识要检查的系统
 * @return INT8U 返回1表示报警源已清除，返回0表示报警源未清除
 * 
 * @note 检查的故障寄存器包括：
 *       - 17066 + sys_num * 300
 *       - 17068 + sys_num * 300
 *       - 17069 + sys_num * 300
 */
volatile static INT8U warningflag[2]={0};
static INT8U sys_alarm_sources_cleared(INT8U sys_num)
{
  //if (!sys_comm_ok(sys_num)) return 0;


    if (warningflag[sys_num] == IsWarn) return 0;

    /* 系统故障字清零（Event_Warning_Check/Recovery 的寄存器  */
    if (GET_INPUT(17066 + sys_num * 300) != 0) return 0;
    if (GET_INPUT(17068 + sys_num * 300) != 0) return 0;
    if (GET_INPUT(17069 + sys_num * 300) != 0) return 0;
    return 1;
}

/* 清锁存的“稳定时间”判据：避免抖动（例如 5 秒稳定） */
/**
 * @brief 检查系统清除锁存状态是否稳定
 * 
 * 该函数用于判断指定系统的清除锁存状态是否已保持稳定超过2秒。
 * 只有当系统所有报警源都已清除，且该状态持续保持超过2秒时，
 * 函数才返回成功状态。
 * 
 * @param sys_num 系统编号，用于标识要检查的系统
 * @return INT8U 返回1表示清除锁存状态已稳定超过2秒，返回0表示未稳定或未满足条件
 * 
 * @note 函数内部使用静态数组记录每个系统清除稳定的开始时间
 * @note 时间比较使用无符号整数减法，避免溢出问题
 * @note 当检测到系统报警源未清除时，会重置该系统的稳定计时
 */
static INT8U sys_clear_latch_ok(INT8U sys_num)
{
    static INT32U s_clear_stable_start[MAX_SYS_NUM] = {0};
    INT32U now_s = EMS_GetSec(); 

    if (!sys_alarm_sources_cleared(sys_num)) 
    {
        s_clear_stable_start[sys_num] = 0;
        return 0;
    }

    if (s_clear_stable_start[sys_num] == 0) 
    {
        s_clear_stable_start[sys_num] = now_s;
        return 0;
    }

    if ((INT32U)(now_s - s_clear_stable_start[sys_num]) >= 2U) 
    {
        return 1;
    }
    return 0;
}


/**
 * @brief 更新全局系统状态
 * 
 * 根据两个子系统的状态(g_sys_status_cache[0]和g_sys_status_cache[1])来更新全局系统状态。
 * 函数会根据以下规则处理状态更新：
 * 1. 当只有一个子系统启动时(seen == 1或seen == 2)，直接采用该子系统的状态
 * 2. 当两个子系统都启动时(seen == 3)，按优先级规则合并状态：
 *    - 任一子系统为ALARM，全局状态为ALARM
 *    - 任一子系统为FAULT或输入107为真，全局状态为FAULT
 *    - 任一子系统为WARN，全局状态为WARN
 *    - 任一子系统为WARNRUN，全局状态为WARN
 *    - 其他情况按优先级选择一个状态作为全局状态
 * 
 * @note 特殊状态处理：
 *       - 状态13,14,23,24,25,27会被映射为SYSStopping(停止中)
 *       - 状态0,20会被映射为SYSClearFAULT(配置中)
 *       - 状态2,15,16,21,22会被映射为SYSStarting(启动中)
 * 
 * @param void 无参数
 * @return void 无返回值
 * 
 * @see SET_INPUT
 * @see sys_state_rank
 * @see sys_state_rank_simple
 */
static void update_global_SYSTEM_STATUS(void)
{
    INT8U seen = g_sys_status_seen_mask;

    /* 第一次启动系统的时候，只要不是故障，谁快就按照谁的状态定义总状态，当两组都启动之后，那么seen就会变成3，走下面的逻辑 */
    if (seen == (1u << 0)) 
    { 
        SET_INPUT(106, g_sys_status_cache[0]); 
        if(g_sys_status_cache[0]==13||g_sys_status_cache[0]==14||g_sys_status_cache[0]==23||g_sys_status_cache[0]==24||
        g_sys_status_cache[0]==25||g_sys_status_cache[0]==27)
        {
        SET_INPUT(108, SYSStopping);//停止中
        }
        else if(g_sys_status_cache[0]==0||g_sys_status_cache[0]==20)
        {
        SET_INPUT(108, SYSClearFAULT);//配置中
        }
        else if(g_sys_status_cache[0]==2||g_sys_status_cache[0]==15||g_sys_status_cache[0]==16||g_sys_status_cache[0]==21
        ||g_sys_status_cache[0]==22)
        {
        SET_INPUT(108, SYSStarting);//启动中
        }
        else 
        {
        SET_INPUT(108, g_sys_status_cache[0]);
        }
        return;
    }
    if (seen == (1u << 1)) 
    { 
        SET_INPUT(106, g_sys_status_cache[1]); 
       // LOG_INFO("g_sys_status_cache[1]=%d",g_sys_status_cache[1]);
        if(g_sys_status_cache[1]==13||g_sys_status_cache[1]==14||g_sys_status_cache[1]==23||g_sys_status_cache[1]==24||
        g_sys_status_cache[1]==25||g_sys_status_cache[1]==27)
        {
        SET_INPUT(108, SYSStopping);//停止中
        }
        else if(g_sys_status_cache[1]==0||g_sys_status_cache[1]==20)
        {
        SET_INPUT(108, SYSClearFAULT);//配置中
        }
        else if(g_sys_status_cache[1]==2||g_sys_status_cache[1]==15||g_sys_status_cache[1]==16||g_sys_status_cache[1]==21
        ||g_sys_status_cache[1]==22)
        {
        SET_INPUT(108, SYSStarting);//启动中
        }
        else 
        {
        SET_INPUT(108, g_sys_status_cache[1]);
        }
        return; 
    }

    INT16U s0 = g_sys_status_cache[0];
    INT16U s1 = g_sys_status_cache[1];

    /* 规则 1：任一 sys 是 ALARM，全局 ALARM */
    if (s0 == ALARM || s1 == ALARM) 
    {
        SET_INPUT(106, ALARM);
        SET_INPUT(108, ALARM);
        return;
    }
    /* 规则 4：否则任一 sys 是 FAULT FAULT */
    if (s0 == FAULT || s1 == FAULT||GET_INPUT(107)) 
    {
        SET_INPUT(106, FAULT);
        SET_INPUT(108, FAULT);
        return;
    }
    /* 规则 2：否则任一 sys 是 WARN，全局 WARN */
    if (s0 == WARN || s1 == WARN) 
    {
        SET_INPUT(106, WARN);
        SET_INPUT(108, WARN);
        return;
    }

    /* 规则 3：否则任一 sys 是 WARNRUN，全局 WARNRUN */
    if (s0 == WARNRUN || s1 == WARNRUN) 
    {

        SET_INPUT(106, WARN);
        SET_INPUT(108, WARN);
        return;
    }

    /* 规则 5：否则按 优先级 选一个代表整体的运行态 */
    SET_INPUT(106, (sys_state_rank(s1) > sys_state_rank(s0)) ? s1 : s0);
    if(g_sys_status_cache[0]==13||g_sys_status_cache[0]==14||g_sys_status_cache[0]==23||g_sys_status_cache[0]==24||
    g_sys_status_cache[0]==25||g_sys_status_cache[0]==27)
    {

        s0 = SYSStopping;        
    }
    else if(g_sys_status_cache[0]==0||g_sys_status_cache[0]==20)
    {

        s0 = SYSClearFAULT;   
    }
    else if(g_sys_status_cache[0]==2||g_sys_status_cache[0]==15||g_sys_status_cache[0]==16||g_sys_status_cache[0]==21
    ||g_sys_status_cache[0]==22)
    {

        s0 = SYSStarting;   
    }

    if(g_sys_status_cache[1]==13||g_sys_status_cache[1]==14||g_sys_status_cache[1]==23||g_sys_status_cache[1]==24||
    g_sys_status_cache[1]==25||g_sys_status_cache[1]==27)
    {

        s1 = SYSStopping;        
    }
    else if(g_sys_status_cache[1]==0||g_sys_status_cache[1]==20)
    {

        s1 = SYSClearFAULT;   
    }
    else if(g_sys_status_cache[1]==2||g_sys_status_cache[1]==15||g_sys_status_cache[1]==16||g_sys_status_cache[1]==21
    ||g_sys_status_cache[1]==22)
    {

         s1 = SYSStarting;   
    }
    SET_INPUT(108, (sys_state_rank_simple(s1) > sys_state_rank_simple(s0)) ? s1 : s0);

}

/* ===================== SYS_State_Run：仅展示与锁存相关改动 ===================== */
/**
 * @brief 运行系统状态机，处理状态转换和事件响应
 * 
 * @param state 指向系统状态枚举数组的指针，volatile确保多线程/中断安全
 * @param sys_num 系统编号，用于标识当前处理的系统实例
 * 
 * @note 函数主要功能包括：
 *       1. 同步EMS掩码
 *       2. 处理强制初始化请求（PCS通信断联场景）
 *       3. 在特定状态下应用拓扑变化
 *       4. 处理未启用系统的状态
 *       5. 执行正常状态机推进逻辑
 * 
 * @warning 函数会修改全局变量：g_force_init_req, event_id, last_time, 
 *          SUB_state, g_sys_alarm_latch, g_sys_status_cache, 
 *          g_sys_status_seen_mask, g_topology_dirty
 * 
 * @details 状态机处理流程：
 *          - 首先检查强制初始化请求，若存在则重置系统状态
 *          - 在停止/初始化状态下应用拓扑变化
 *          - 未启用系统仅处理告警锁存逻辑
 *          - 启用系统执行完整状态机转换，包括：
 *            * 通信边沿检测
 *            * 告警锁存处理
 *            * 状态转换执行
 *            * 日志记录
 */
void SYS_State_BlackStart_Run(volatile SYS_State_ENUM *state, INT8U sys_num)
{
      sysPara *sys_cfg = SysConf_GetInfo();
    /* 0) 每次循环：同步 EMS 掩码 */
    EMS_Read_Control_Mask();

    /* --- 强制回初始：PCS 通信断联 --- */
    if (g_force_init_req[sys_num]) 
    {
        g_force_init_req[sys_num] = 0;

        Clear_In_Sys(sys_num);
        Clear_Out_Sys(sys_num);
        event_id[sys_num]  = 0;
        last_time[sys_num] = Timer_GetTick();

        for (INT8U sub = 0; sub < SUBS_PER_SYS; ++sub) 
        {
            Clear_In_Sub(sys_num, sub);
            Clear_Out_Sub(sys_num, sub);
            if (Get_State_Sub(sys_num, sub) != SUBInit) 
            {
                SUB_state[sys_num * SUBS_PER_SYS + sub] = SUBInit;
            }
        }

        state[sys_num] = SYSInit;

        /* 断联直接锁存 ALARM，避免后续被 SYSInit/SYSRun 覆盖 */
        g_sys_alarm_latch[sys_num] = ALARM;
        g_sys_status_cache[sys_num] = ALARM;
        g_sys_status_seen_mask |= (INT8U)(1u << sys_num);
        update_global_SYSTEM_STATUS();

        LOG_INFO("SYS-%d forced to SYSInit (PCS comm lost).", sys_num);
        return;
    }
    if (black_start==1) 
    {
        black_start = 0;
        for(uint8_t sys_num=0;(sys_num < sys_cfg->sysNum);sys_num++)
        {
        Clear_In_Sys(sys_num);
        Clear_Out_Sys(sys_num);
        event_id[sys_num]  = 0;
        last_time[sys_num] = Timer_GetTick();
        state[sys_num] = SYSInit;  
        }


    }

    /* 3) 正常推进：先做通信边沿检测 */
    check_pcs_limit();
    Update_Input_Bit_Status(sys_num);//更新LC对EMS的映射状态点位
    for (INT8U i = 0; i < LIST_SIZE; i++) {
        if (state_list[i].now_state == state[sys_num]) 
        {

            INT16U now = (INT16U)state_list[i].now_state;
            INT16U out = calc_sys_status_out(sys_num, now);          
            if (severity_rank(out) > 0) {
                /* 出现 ALARM/WARN/WARNRUN：锁存（只升不降） */
                if (severity_rank(out) > severity_rank(g_sys_alarm_latch[sys_num])) 
                {
                    g_sys_alarm_latch[sys_num] = out;
                }
            } else {
                /* 非告警态：只有满足清除条件才清锁存 */
                if (g_sys_alarm_latch[sys_num] != 0 && sys_clear_latch_ok(sys_num)) 
                {
                    g_sys_alarm_latch[sys_num] = 0;
                }
            }

            /* 对外展示*/
            if (g_sys_alarm_latch[sys_num] != 0) 
            {
                out = g_sys_alarm_latch[sys_num];
            }

            /* 缓存并聚合到全局 106 */
            g_sys_status_cache[sys_num] = out;
            g_sys_status_seen_mask |= (INT8U)(1u << sys_num);
           // LOG_INFO("sys %d, state %d, out %d,g_sys_status_seen_mask:%d\n", sys_num, state[sys_num], out,g_sys_status_seen_mask);
            update_global_SYSTEM_STATUS();       
            state_list[i].fun((INT16U *)event_id, sys_num, state[sys_num]);

            if (state_list[i].eventId == event_id[sys_num]) 
            {
                state[sys_num] = state_list[i].next_state;
                if (state_list[i].string != NULL) 
                {
                    LOG_INFO("SYS-%d %s", sys_num, state_list[i].string);
                }
            }
        }
    }
}


#ifndef STATE_SYS_H
#define STATE_SYS_H
#include "system.h"

typedef enum
{
    SYSInit = 0,     //initialization
    SYSStopped = 1,     //have stopped
    SYSStarting = 2, //have been starting
    SYSRun = 3,     // have been running
    SYSStandby =4 , //standby
    SYSFault = 5,  //Fault
    SYSWarn = 6, //warn
    SYSWarnRun = 7, //warn run
    SYSCheckModeRun = 8, //Check Mode
    SYSBackStop = 9, //return SYSStop
    SYSBackRun = 10, //return SYSRun
    SYSStopping = 11,     //be stopping
    SYSClearPower = 12,     //Set Power 0
    SYSStopPCS = 13,     //Stop PCS
    SYSStopSUB = 14,     //Stop SUB
    SYSStartSUB = 15,     //Start SUB
    SYSStartPCS = 16,     //Start PCS
    SYSReset = 17 ,      //Reset
    SYSLimPower = 18,   //limit power
    SYSLimSUB = 19,      //limit SUB
    SYSClearFAULT=20,     //clear fault
    SYSPQMode=21,     //PQ Mode
    SYSSTARTPQ=22,     //startPQ Mode
    SYSSTOPPQ=23,     //stopPQ Mode
    SYSSTOPPQED=24,     //stopPQ Mode
    SYSZEROPOWER=25, // send zero power
    SYSZWARNING=26, // warning
    SYSZWARNINGSTOP=27, // warningstop
    SYSSTANDYZEROPOWER=28, // send zero power
    SYSBLACKSTART=29,     //black start
    SYSBLACKRun = 30,     // have been running
    SYSBlackStopsetvalue = 31, //return SYSStop
    SYSBlackStopcheckvalue = 32, //return SYSStop
}SYS_State_ENUM;


typedef void (*sys_s_state_fun)(INT16U *event_id,INT8U sub_num,SYS_State_ENUM real_state);
#define EVENT_TIMEOUT_MS   (120 * 100)   
#define SYS_EVENT_NULL              (0)             //无事件 No events
#define SYS_EVENT_INIT              (1)             //初始化 initialization
#define SYS_EVENT_FAULT             (2)             //发生故障 Fault
#define SYS_EVENT_CMD_START         (3)             //发送启动命令 Send start command
#define SYS_EVENT_CMD_STOP          (4)             //发送停机命令 Send stop command
#define SYS_EVENT_CMD_STANDBY       (5)             //发送待机命令 Send standby command
#define SYS_EVENT_CMD_RESET         (6)             //发送复位命令 Send reset command
#define SYS_EVENT_SYS_RUN           (7)             //SYS进入运行 SYS enters running
#define SYS_EVENT_START_WARN       (8)             //启动时出现告警    
#define SYS_EVENT_WARN_RECOVERY           (9)             //启动告警恢复
#define SYS_EVENT_FAULT_STOP        (10)            //故障停机  Fault stop     
#define SYS_EVENT_CMD_POWER         (11)            //下发功率  set power
#define SYS_EVENT_CHECK_POWER       (12)            //功率生效  Power settings feedback is correct
#define SYS_EVENT_STOP_MODE         (13)            //停机时核对模式  Verification mode during shutdown
#define SYS_EVENT_RUN_MODE          (14)            //运行时核对模式  Verification mode during run

#define SYS_EVENT_STOP_PCS          (19)            //使PCS停机   set PCS stop
#define SYS_EVENT_STOP_SUB          (20)            //全部子系统停机  All subsystems shut down
#define SYS_EVENT_WAIT_STOPED       (21)            //系统已经停机 The system has been shut down
#define SYS_EVENT_START_PCS         (22)            //启动PCS Start PCS
#define SYS_EVENT_START_SUB         (23)            //启动子系统 Start subsystem
#define SYS_EVENT_WAIT_RESET        (24)            //系统已经复位 The system has been reset
#define SYS_EVENT_WAIT_STANDBY      (25)            //系统已经待机 The pcs has been standby

#define SYS_EVENT_CHECK_SUB         (28)            //限制子系统功率生效  limit subsys feedback

#define SYS_EVENT_START_FAULT       (29)            //启动时存在故障
#define SYS_EVENT_START_FAULT_CLEAR (30)            //启动故障已经清除

#define SYS_EVENT_START_PQ          (31)             //PQ开启

#define SYS_EVENT_PQ                (32)              //正在运行PQ
#define SYS_EVENT_STOP_PQ           (33)             //停止PQ
#define SYS_EVENT_STOP_PQ_CHECK     (37)             //停止PQ检查
#define SYS_EVENT_ZERO_POWER        (34)            //停机0功率时间

#define SYS_EVENT_WARNING           (35)            //告警停机
#define SYS_EVENT_WARNING_STOP      (36)      //告警停机，只停止PCS
#define SYS_WARNING_STOP            (37)  

#define SYS_EVENT_RESET_RUN         (38)            //系统复位继续回到运行

#define SYS_EVENT_STANDBY_ZERO_POWER         (39)            //系统待机下发零功率
#define SYS_EVENT_STOP                        (40)             //在运行过程中主机和从机都因故障停机 
#define SYS_EVENT_STANDBY        (41)            //待机
#define ZERO_POWER_LIMIT            (500)   //50.0KW   

#define SYS_EVENT_STANDBY_RUN         (42)            //系统继续回到运行
#define SYS_EVENT_STOPPED_RUN         (43)            //系统继续回到运行
#define SYS_EVENT_ISLAND_CMD         (44)            //系统继续回到运行
#define SYS_EVENT_CMD_BLACK_START         (45)             //发送黑启动命令 Send start command
#define SYS_EVENT_CMD_BLACK_CLOSE         (46)             //发送黑启动关闭命令 Send start command
#define SYS_EVENT_CMD_BLACK_RUN        (46)             //黑启动运行 
#define TICKS_5S   500         // 10ms/tick -> 5s = 500 ticks（如不是10ms请调整）

/*切入切出相关变量 */
// --- 拓扑规模 ---
#define SYS_COUNT      2
#define PCS_PER_SYS    2
//#define BMS_PER_SYS    2
//#define SUBS_PER_SYS   BMS_PER_SYS        
#define MAX_PCS        (SYS_COUNT * PCS_PER_SYS)   // 4
//#define MAX_BMS        (SYS_COUNT * BMS_PER_SYS)   // 8

#define PCS_G0_MASK  0x03   // PCS0,1
#define PCS_G1_MASK  0x0C   // PCS2,3
#define PCS_G2_MASK  0x0F   // PCS2,3
// #define BMS_G0_MASK  0x03   // BMS0..3
// #define BMS_G1_MASK  0x0C   // BMS4..7
// #define BMS_G2_MASK  0x0F   // BMS4..7

#define EMS_PCS_MASK_REG    1506   // EMS 下发：PCS 使能掩码（bit0..bit3）
#define EMS_BMS_MASK_REG    1507   // EMS 下发：BMS 使能掩码（bit0..bit3）
typedef struct {
    INT16U offset;     // 相对 2600 + 300*(n-1) 的偏移
    const char* name;    // 日志标题
} fault_item_t;
typedef struct {
SYS_State_ENUM now_state;          //当前状态 Current status
INT8U  eventId;                   //事件ID  Event ID  
SYS_State_ENUM next_state;         //下个状态  next state
sys_s_state_fun fun;               //执行函数  Execute function
char *string;                       //打印的日志内容 Printed log content
}StateSYSform;
typedef enum {
    TOPO_NONE      = 0,
    TOPO_FROM_PCS  = 1,
    TOPO_FROM_BMS  = 2,
    TOPO_FROM_BOTH = 3
} topo_src_t;
extern INT8U BMS_PER_SYS;
extern INT8U SUBS_PER_SYS;
extern INT8U MAX_BMS;
extern INT8U BMS_G0_MASK;
extern INT8U BMS_G1_MASK;
extern INT8U BMS_G2_MASK;
/**
 * @brief 状态机执行函数；
 * State machine execution function
 * @param[in] state - Current status
 * @param[in] sys_num-sys serial number, 0 represents the first unit
 * @retval null
 */
void SYS_State_Run(volatile SYS_State_ENUM *state, INT8U sys_num);

INT8U Set_Out_Sys(INT8U sys_num,INT16U event_id);
INT8U Get_Out_Sys(INT8U sys_num);
uint64_t Get_Out_Time_Sys(INT8U sys_num);
void Clear_Out_Sys(INT8U sys_num);
void Wait_Out_Sys(INT8U sys_num);
INT8U Set_In_Sys(INT8U sys_num,INT16U event_id);
INT8U Get_In_Sys(INT8U sys_num);
void Clear_In_Sys(INT8U sys_num);
void Wait_In_Sys(INT8U sys_num);
uint64_t Get_In_Time_Sys(INT8U sys_num);
SYS_State_ENUM Get_State_Sys(INT8U sys_num);
void Success_Out_Sys(INT8U sys_num);
void Success_In_Sys(INT8U sys_num);

INT8U SUB_ENABLED_SYS(INT8U sys, INT8U sub);
extern INT16S volatile LimPcharge[MAX_PCS_NUM][MAX_SUB_NUM];
extern INT16S volatile LimPdischarge[MAX_PCS_NUM][MAX_SUB_NUM];
void PCS_Unpack_To_Set_Hold(INT16U value);
void BMS_Unpack_To_Set_Hold(INT16U value);
extern volatile INT8U  g_enabled_mask_pcs;      // 低 4bit：PCS0..3
extern volatile INT16U g_enabled_mask_bms ;      // 低 8bit：BMS0..7
extern volatile  INT16U measureflag ;

extern volatile  INT16U measurecloseflag ;
#endif
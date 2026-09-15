#include "main.h"
#include "log.h"
#include "timed_check.h"
#include "state_sys.h"
#include "state_task.h"
#include <stdarg.h>

volatile  INT16U measurecloseflag = false;
volatile  INT16U measureflag = false;
static void slave_pcs_clear_points_on_fault(int pcs);
static void pcs_clear_points_on_fault(int pcs_group);
/* ===================== 通信断联→强制回初始 ===================== */
static volatile INT8U g_force_init_req[MAX_SYS_NUM] = {0};
static uint64_t         g_force_init_last_ts[MAX_SYS_NUM] = {0};  /* 防抖（10ms tick） */

static volatile INT8U Standby_Cmd_Zero_Power[MAX_SYS_NUM] = {0};//MV一体机下发待机命令标志位
/* ===================== 切入切出/拓扑与掩码 ===================== */
/* --- EMS 原始掩码（寄存器 204 / 205 解出的原始位图）--- */
volatile INT8U  g_ems_mask_pcs_raw = 0x0F;   /* 低 4bit：PCS0..3 */
volatile INT16U g_ems_mask_bms_raw = 0x0F;   /* 低 8bit：BMS0..7 */

/* --- 生效后的掩码（“点一台→整组下线”的规则重建）--- */
volatile INT8U  g_enabled_mask_pcs = 0xFF;   /* 低 4bit：PCS0..3 */
volatile INT16U g_enabled_mask_bms = 0xFF;   /* 低 8bit：BMS0..7 */
static INT8U  clear_pending[MAX_SYS_NUM] = {0};
static uint64_t clear_req_ts[MAX_SYS_NUM]  = {0};
static INT32U last_time[MAX_SYS_NUM]; 
INT8U pcs_mask_raw = 0x0F;
INT8U bms_mask_raw = 0x0F;

#define MAX_PCS 4
INT8U BMS_PER_SYS;
INT8U SUBS_PER_SYS;
INT8U MAX_BMS;
INT8U BMS_G0_MASK;
INT8U BMS_G1_MASK;
INT8U BMS_G2_MASK;
extern uint8_t black_end  ;
static INT8U  s_prev_comm_fault[MAX_PCS] = {0};  /* 0:正常, 1:故障 */
static INT8U  s_inited = 0;
volatile INT8U g_topology_dirty = 0;
extern INT8U BusType;

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

INT16U PCS_Pack_Bits(void)//将PCS切入切出的点位组合成一个值
{
    INT16U value = 0;

    for (int i = 0; i < 4; i++) 
    {
        INT16U bit = GET_HOLD(1506 + i) & 0x01;  
        value |= (bit << i);
    }

    return value;   // bit0 对应1506 … bit15 对应1521
}
INT16U BMS_Pack_Bits(void) //将BMS切入切出的点位组合成一个值
{
    INT16U value = 0;

    for (int i = 0; i < 8; i++) 
    {
        INT16U bit = GET_HOLD(1538 + i) & 0x01;  
        value |= (bit << i);
    }

    return value;   // bit0 对应1506 … bit15 对应1521
}
/**
 * @brief 将值拆分放到PCS的切入切出点位中
 * 
 * 该函数将输入的16位无符号整数的低4位逐个拆分，并将每一位的值
 * 设置到对应的PCS寄存器中。寄存器地址从1506开始，依次递增。
 * 
 * @param value 要拆分的16位无符号整数，其低4位将被使用
 * 
 * @note 该函数会修改以下寄存器：
 *       - 1506: 对应value的第0位
 *       - 1507: 对应value的第1位
 *       - 1508: 对应value的第2位
 *       - 1509: 对应value的第3位
 * 
 * @warning 请确保value的低4位有效，高位将被忽略
 * 
 * @return 无返回值
 */
void PCS_Unpack_To_Set_Hold(INT16U value)//将值拆分放到PCS的切入切出点位中
{
    for (int i = 0; i < 4; i++) 
    {
        INT16U bit = (value >> i) & 0x01;  // 取第 i 位
        SET_HOLD(1506 + i, bit);             
    }
}

/**
 * @brief 将输入的16位无符号整数值拆分为8个独立的位，并分别设置到BMS的切入切出点位中
 * 
 * 该函数通过位操作提取输入值的每一位，并将这些位依次设置到BMS的指定寄存器中。
 * 起始地址为1538，共占用8个连续的寄存器地址（1538-1545）。
 * 
 * @param value 输入的16位无符号整数，将被拆分为8个位进行设置
 * 
 * @note 每个位对应一个寄存器地址，从1538开始依次递增
 * @note 处理的位顺序是从最低位(LSB)到最高位(MSB)
 * @note 特殊字符处理：\t(制表符), \r(回车符), \n(换行符)
 */
void BMS_Unpack_To_Set_Hold(INT16U value)//将值拆分放到BMS的切入切出点位中
{
    for (int i = 0; i < 8; i++) 
    {
        INT16U bit = (value >> i) & 0x01;  // 取第 i 位
        SET_HOLD(1538 + i, bit);            
    }
}



//如果当前sys被启用就返回1
/**
 * @brief 检查指定系统组的PCS引脚对是否全部使能
 * 
 * 该函数用于检查特定系统组(system group)的两个PCS引脚是否都已使能。
 * 系统组0对应PCS0和PCS1，系统组1对应PCS2和PCS3。
 * 
 * @param sys 系统组标识符(0或1)
 *            - 0: 检查系统组0(PCS0和PCS1)
 *            - 1: 检查系统组1(PCS2和PCS3)
 *            - 其他值: 返回0(无效输入)
 * 
 * @return INT8U 返回检查结果
 *               - 1: 指定系统组的两个PCS引脚都已使能
 *               - 0: 至少有一个PCS引脚未使能或输入参数无效
 * 
 * @note 函数内部使用全局变量g_enabled_mask_pcs来获取PCS引脚的使能状态
 *       - PCS_G0_MASK (0x03): 用于检查PCS0和PCS1
 *       - PCS_G1_MASK (0x0C): 用于检查PCS2和PCS3
 * 
 * @warning 函数不会检查g_enabled_mask_pcs是否为NULL
 */
static inline INT8U pair_enabled(INT8U sys)
{
    if (sys > 1) return 0;
    INT8U mask;
    if (sys == 0) 
    {
        mask = PCS_G0_MASK;   // 0x03 -> 检查 PCS0 和 PCS1
    } else 
    {
        mask = PCS_G1_MASK;   // 0x0C -> 检查 PCS2 和 PCS3
    }
    INT8U both_on = (INT8U)(g_enabled_mask_pcs & mask);

    if (both_on == mask) 
    {
        return 1; 
    } else 
    {
        return 0;  
    }
}
static volatile topo_src_t g_topo_src = TOPO_NONE;

/**
 * @brief 从原始数据重建EMS启用的掩码
 * @details 根据不同的拓扑源(TOPO_FROM_PCS/TOPO_FROM_BMS/TOPO_FROM_BOTH)，
 *          从原始PCS和BMS掩码数据中解析出有效的启用掩码，并更新全局变量
 * @note 该函数会处理以下特殊字符：\t(制表符)、\r(回车符)、\n(换行符)
 * @param void 无输入参数
 * @return void 无返回值
 * 
 * 处理逻辑说明：
 * 1. 提取原始PCS和BMS掩码的有效位
 * 2. 根据拓扑源类型(g_topo_src)进行分支处理：
 *    - TOPO_FROM_PCS: 基于PCS原始值确定掩码
 *    - TOPO_FROM_BMS: 基于BMS原始值和总线类型确定掩码
 *    - TOPO_FROM_BOTH: (当前未实现)
 * 3. 更新全局启用掩码并调用解包函数
 * 4. 记录日志信息
 */
/* PCS拓扑映射表 */
static void EMS_Rebuild_Enabled_Mask_From_Raw(void)
{
    INT8U  pcs_raw = (INT8U)(g_ems_mask_pcs_raw & 0x0F);   /* 有效 4 位 */
    uint16_t bms_raw = (uint16_t)(g_ems_mask_bms_raw & BusType);  /* 有效 8 位 */
    INT8U pcs;
    uint16_t bms;
 
    switch (g_topo_src) 
    {
    case TOPO_FROM_PCS:
        if(pcs_raw==7)
        {
            pcs=PCS_G0_MASK;
            bms=BMS_G0_MASK;
        }
        else if(pcs_raw==11)
        {
            pcs=PCS_G0_MASK;
            bms=BMS_G0_MASK;
        }
        else if(pcs_raw==3)
        {
            pcs=PCS_G0_MASK;
            bms=BMS_G0_MASK;
        }

        else if(pcs_raw==12)
        {
            pcs=PCS_G1_MASK;
            bms=BMS_G1_MASK;
        }
        else if(pcs_raw==13)
        {
            pcs=PCS_G1_MASK;
            bms=BMS_G1_MASK;
        }
        else if(pcs_raw==14)
        {
            pcs=PCS_G1_MASK;
            bms=BMS_G1_MASK;
        }
        else if(pcs_raw==15)
        {
            pcs=PCS_G2_MASK;
            bms=BMS_G2_MASK;
        }
        else 
        {
            pcs=0;
            bms=0;
        }
        break;

    case TOPO_FROM_BMS:
        if(BusType==S_BUS)
		{
            if(bms_raw==7)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==11)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==3)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }

            else if(bms_raw==12)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==13)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==14)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==15)
            {
                pcs=PCS_G2_MASK;
                bms=BMS_G2_MASK;
            }
            else 
            {
                pcs=0;
                bms=0;
            }
        }
        else
        {
            if(bms_raw==254)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==253)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==251)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }

            else if(bms_raw==247)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==243)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==254)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }
            else if(bms_raw==245)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            }        

            else if(bms_raw==252)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            } 
            else if(bms_raw==241)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            } 
            else if(bms_raw==248)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            } 
            else if(bms_raw==240)
            {
                pcs=PCS_G1_MASK;
                bms=BMS_G1_MASK;
            } 
    ////////////////////////////
            else if(bms_raw==127)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==191)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==223)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }

            else if(bms_raw==239)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==63)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==159)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }
            else if(bms_raw==207)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            }        

            else if(bms_raw==31)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            } 
            else if(bms_raw==143)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            } 
            else if(bms_raw==15)
            {
                pcs=PCS_G0_MASK;
                bms=BMS_G0_MASK;
            } 
            else if(bms_raw==255)
            {
                pcs=PCS_G2_MASK;
                bms=BMS_G2_MASK;
            } 
            else 
            {
                pcs=0;
                bms=0;  
            }

        }
        break;

    case TOPO_FROM_BOTH:
        
        break;

    default:
        break;
    }

    g_enabled_mask_pcs = pcs;
    g_enabled_mask_bms = bms;
    PCS_Unpack_To_Set_Hold(g_enabled_mask_pcs);
    BMS_Unpack_To_Set_Hold(g_enabled_mask_bms);

    LOG_INFO("[EMS] rebuild enabled -> PCS=0x%02X, BMS=0x%02X (raw pcs=0x%02X, bms=0x%02X, src=%d)",
             g_enabled_mask_pcs, (INT8U)g_enabled_mask_bms,
             pcs_raw, (INT8U)bms_raw, (int)g_topo_src);
}

static INT32U EMS_GetSec(void)
{

    return (INT32U)(Timer_GetTick() / 100U);
}

static INT8U  s_pcs_bms_waiting = 0;
static INT32U s_pcs_bms_start_s = 0;

static INT8U  s_bms_pcs_waiting = 0;
static INT32U s_bms_pcs_start_s = 0;

/**
 * @brief 读取并更新EMS控制掩码
 * 
 * 该函数用于处理PCS(变流器)和BMS(电池管理系统)的使能标志，
 * 在满足延时条件后读取并更新控制掩码，同时检测拓扑变化。
 * 
 * @details 功能说明：
 *          1. 处理PCS_BMS使能标志：
 *             - 当PCS_BMS_Enable_Flag为真时，启动10秒延时
 *             - 延时结束后读取PCS和BMS寄存器值
 *             - 检测PCS掩码变化并更新全局变量
 *          2. 处理BMS_PCS使能标志：
 *             - 当BMS_PCS_Enable_Flag为真时，启动10秒延时
 *             - 延时结束后读取PCS和BMS寄存器值
 *             - 检测BMS掩码变化并更新全局变量
 *          3. 特殊字符处理：
 *             - 函数会正确处理\t(制表符)、\r(回车符)、\n(换行符)等特殊字符
 * 
 * @note 全局变量说明：
 *       - g_ems_mask_pcs_raw: PCS掩码原始值
 *       - g_ems_mask_bms_raw: BMS掩码原始值
 *       - g_topology_dirty: 拓扑变化标志
 *       - g_topo_src: 拓扑变化来源(TOPO_FROM_PCS/TOPO_FROM_BMS)
 *       - s_pcs_bms_waiting: PCS_BMS等待标志
 *       - s_bms_pcs_waiting: BMS_PCS等待标志
 *       - s_pcs_bms_start_s: PCS_BMS开始时间(秒)
 *       - s_bms_pcs_start_s: BMS_PCS开始时间(秒)
 * 
 * @warning 注意事项：
 *          - 函数会修改全局变量g_ems_mask_pcs_raw、g_ems_mask_bms_raw
 *          - 函数会设置g_topology_dirty标志
 *          - 函数会清除PCS_BMS_Enable_Flag和BMS_PCS_Enable_Flag标志
 */
void EMS_Read_Control_Mask(void)
{
    INT32U now_s = EMS_GetSec();


    if (PCS_BMS_Enable_Flag) 
    {

        if (!s_pcs_bms_waiting) 
        {
            s_pcs_bms_waiting = 1;
            s_pcs_bms_start_s = now_s;
            LOG_INFO("[EMS] PCS_BMS_Enable_Flag set, start 10s delay");
        }
        
        else if ((INT32U)(now_s - s_pcs_bms_start_s) >= 10U) 
        {

      
            s_pcs_bms_waiting = 0;
            PCS_BMS_Enable_Flag = false;

            INT16U pcs_reg = PCS_Pack_Bits();
            INT16U bms_reg = BMS_Pack_Bits();
            LOG_INFO("pcs_reg:%d,bms_reg:%d", pcs_reg, bms_reg);

            INT8U  pcs_mask_raw = (INT8U)(pcs_reg & 0x0F);    /* PCS0..3 */
            INT16U bms_mask_raw = (INT16U)(bms_reg & BusType); /* BMS0..7 */


            INT8U pcs_changed = (pcs_mask_raw != g_ems_mask_pcs_raw);

            if (pcs_changed) 
            {
                g_topo_src = TOPO_FROM_PCS;    
            } else 
            {

                return;
            }

            g_ems_mask_pcs_raw = pcs_mask_raw;
            g_ems_mask_bms_raw = bms_mask_raw;

            g_topology_dirty = 1;
            LOG_INFO("[EMS] mask changed (raw) -> PCS=0x%02X, BMS=0x%02X",
                     g_ems_mask_pcs_raw, (INT8U)g_ems_mask_bms_raw);
        }
    }
     else 
    {
      
        s_pcs_bms_waiting = 0;
    }


    if (BMS_PCS_Enable_Flag) 
    {

        if (!s_bms_pcs_waiting) 
        {
            s_bms_pcs_waiting  = 1;
            s_bms_pcs_start_s  = now_s;
            LOG_INFO("[EMS] BMS_PCS_Enable_Flag set, start 30s delay");
        }
        else if ((INT32U)(now_s - s_bms_pcs_start_s) >= 10U) 
        {

            s_bms_pcs_waiting  = 0;
            BMS_PCS_Enable_Flag = false;

            INT16U pcs_reg = PCS_Pack_Bits();
            INT16U bms_reg = BMS_Pack_Bits();
            LOG_INFO("pcs_reg:%d,bms_reg:%d", pcs_reg, bms_reg);

            INT8U  pcs_mask_raw = (INT8U)(pcs_reg & 0x0F);    /* PCS0..3 */
            INT16U bms_mask_raw = (INT16U)(bms_reg & BusType); /* BMS0..7 */

            /* 对比旧缓存，判断哪个侧发生变化 */
            INT8U bms_changed = (bms_mask_raw != g_ems_mask_bms_raw);

            if (bms_changed) 
            {
                g_topo_src = TOPO_FROM_BMS;    // 只 BMS 变
            } else 
            {
                return;
            }

            g_ems_mask_pcs_raw = pcs_mask_raw;
            g_ems_mask_bms_raw = bms_mask_raw;

            g_topology_dirty = 1;
            LOG_INFO("[EMS] mask changed (raw) -> PCS=0x%02X, BMS=0x%02X",
                     g_ems_mask_pcs_raw, (INT8U)g_ems_mask_bms_raw);
        }
    } 
    else 
    {
        s_bms_pcs_waiting = 0;
    }
}
//如果当前的支路被启用则返回1
INT8U SUB_ENABLED_SYS(INT8U sys, INT8U sub)
{
    if (!pair_enabled(sys)) return 0;
    INT8U gi = (INT8U)(sys * BMS_PER_SYS + sub); /* 全局 BMS 索引 0..7 */
   
    return (g_enabled_mask_bms >> gi) & 1u;
}

/* ===================== 运行时全局（事件/计时等） ===================== */
INT16S volatile LimPcharge[MAX_PCS_NUM][MAX_SUB_NUM]    = {0};
INT16S volatile LimPdischarge[MAX_PCS_NUM][MAX_SUB_NUM] = {0};
extern unsigned short pcs_state[MAX_PCS_NUM];

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
static void Event_SYS_Run_Check       (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Start_Fault_Check   (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Stop_PCS_Check      (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Start_SUB_Check     (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Start_PCS_Check     (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Stop_SUB_Check      (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Stopped_Check   (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Timeout_Check       (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Wait_Stopped_Check  (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Fault_Stop_Check    (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Reset_Check     (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Wait_Reset_Check    (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Clear_Fault_Check   (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_PQ_Mode_Check   (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Stop_PQ_Check   (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_ZERO_POWER_Check(INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Stop_Check          (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Warning_Check       (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Check_Sub_Check     (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_PQ_Check        (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_STOP_PQ_Mode_Check(INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Warning_Recovery    (INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Standby_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Start_Warn_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_Start_Warn_Recovery(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_STANDBY_ZERO_POWER_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_SYS_STANDBY_STATUS_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Stop_Check_PCS_Status(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Run_Check_PCS_Status(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Stop_Check_PCS(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
static void Event_CMD_Time_Out(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state);
/* ===================== 状态转移表 ===================== */
const static StateSYSform state_list[] = {
    /* 当前状态 ************事件 ***************************目标状态 ********* *****检测函数 **********************************描述 */
    {SYSInit,           SYS_EVENT_INIT,                 SYSStopped,         Event_Init_Check,                   "初始化事件：初始化->停机"},

    {SYSStopping,       SYS_EVENT_STOP_PCS,             SYSStopPCS,         Event_Stop_PCS_Check,               "PCS停机：停机中->PCS停机"},
    
    {SYSStopPCS,        SYS_EVENT_WARNING_STOP,         SYSZWARNINGSTOP,    Event_Stop_Check,                   "一般故障停机：PCS停机->一般故障停机"},
    {SYSStopPCS,        SYS_EVENT_STOP_SUB,             SYSStopSUB,         Event_Stop_SUB_Check,               "下发停机：PCS停机->子系统停机"},
   
    {SYSZWARNINGSTOP,   SYS_WARNING_STOP,               SYSInit,            Event_Warning_Recovery,             "一般故障停机：一般故障停机->一般故障恢复"},
    {SYSStopSUB,        SYS_EVENT_WAIT_STOPED,          SYSStopped,         Event_Wait_Stopped_Check,           "等待停机：子系统停机->已停机"},

    {SYSFault,          SYS_EVENT_FAULT_STOP,           SYSZEROPOWER,       Event_Fault_Stop_Check,             "停机命令：故障停机->停机中"},

    {SYSStopped,        SYS_EVENT_START_FAULT,          SYSClearFAULT,      Event_Start_Fault_Check,            "故障事件：启动中->故障清除"},
    {SYSStopped,        SYS_EVENT_CMD_START,            SYSStarting,        Event_CMD_Start_Check,              "启动命令：已停机->启动中"},
    {SYSStopped,        SYS_EVENT_CMD_RESET,            SYSReset,           Event_CMD_Reset_Check,              "重置命令： 重置"},
   // {SYSStopped,      SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：启动中->故障停机"},
    {SYSStopped,        SYS_EVENT_START_WARN,           SYSWarn,            Event_Start_Warn_Check,             "告警事件：已停机->告警中"},
    {SYSStopped,        SYS_EVENT_STOPPED_RUN,          SYSRun,              Event_CMD_Run_Check_PCS_Status,   "运行事件：已停机->运行"},//针对用户从台达web去开机的操作

    {SYSWarn,           SYS_EVENT_START_FAULT,          SYSClearFAULT,      Event_Start_Fault_Check,            "故障事件：启动中->故障清除"},
    {SYSWarn,           SYS_EVENT_CMD_START,            SYSStarting,        Event_CMD_Start_Check,              "启动命令：已停机->启动中"},
    {SYSWarn,           SYS_EVENT_WARNING,              SYSZWARNINGSTOP,    Event_Warning_Check,                "一般故障事件：已停机->一般故障"},
    {SYSWarn,           SYS_EVENT_CMD_RESET,            SYSReset,           Event_CMD_Reset_Check,              "重置命令： 重置"},
    {SYSWarn,           SYS_EVENT_WARN_RECOVERY,        SYSStopped,         Event_Start_Warn_Recovery,          "告警事件：告警中->告警恢复"},
    //{SYSWarn,         SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：启动中->故障停机"},
    {SYSWarn,        SYS_EVENT_STOPPED_RUN,             SYSRun,              Event_CMD_Run_Check_PCS_Status,   "运行事件：已停机->运行"},//针对用户从台达web去开机的操作
    {SYSReset,          SYS_EVENT_WAIT_RESET,           SYSStopped,         Event_Wait_Reset_Check,             "等待重置：重置->已停机"},

   // {SYSStarting,     SYS_EVENT_START_WARN,           SYSWarn,            Event_Start_Warn_Check,             "告警事件：已停机->告警中"},
    {SYSStarting,       SYS_EVENT_WARNING,              SYSStopping,        Event_Warning_Check,                "一般故障事件：启动中->一般故障停机"},
    {SYSStarting,       SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：启动中->故障停机"},
    {SYSStarting,       SYS_EVENT_ZERO_POWER,             SYSStopping,        Event_CMD_Stop_Check,               "停机命令：启动中->停机中"},
    {SYSStarting,       SYS_EVENT_START_SUB,            SYSStartSUB,        Event_Start_SUB_Check,              "子系统启动：启动中->子系统启动"},
  // {SYSStarting,       SYS_EVENT_STOP,                  SYSStopping,         Event_CMD_Stop_Check_PCS_Status,    "停机命令：运行->停机"},
    {SYSStarting,       SYS_EVENT_STOP,                  SYSInit,          Event_CMD_Time_Out,                   "超时事件：超时->初始化"},
  {SYSClearFAULT,     SYS_EVENT_START_FAULT_CLEAR,     SYSStopped,         Event_Clear_Fault_Check,            "故障事件：故障清除->启动中"},

    {SYSStartSUB,       SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：子系统启动->故障停机"},
    {SYSStartSUB,       SYS_EVENT_START_PCS,            SYSStartPCS,        Event_Start_PCS_Check,              "PCS启动：子系统启动->PCS启动"},
  //  {SYSStartSUB,     SYS_EVENT_START_WARN,           SYSWarn,            Event_Start_Warn_Check,             "告警事件：已停机->告警中"},
    {SYSStartSUB,       SYS_EVENT_WARNING,              SYSStopping,        Event_Warning_Check,                "一般故障事件：启动中->一般故障停机"},
    {SYSStartSUB,         SYS_EVENT_ZERO_POWER,           SYSStopping,        Event_CMD_Stop_Check,               "停机命令：子系统启动->停机中"},
    {SYSStartSUB,       SYS_EVENT_STOP,                  SYSInit,          Event_CMD_Time_Out,                   "超时事件：超时->初始化"},   
    //{SYSStartSUB,       SYS_EVENT_STOP,                  SYSInit,         Event_CMD_Stop_Check_PCS_Status,    "停机命令：运行->停机"},
   // {SYSStartPCS,     SYS_EVENT_START_WARN,           SYSWarn,            Event_Start_Warn_Check,             "告警事件：已停机->告警中"},
    {SYSStartPCS,       SYS_EVENT_WARNING,              SYSStopping,        Event_Warning_Check,                "一般故障事件：启动中->一般故障停机"},
    {SYSStartPCS,       SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：PCS启动->故障停机"},
    {SYSStartPCS,       SYS_EVENT_SYS_RUN,              SYSPQMode,          Event_SYS_Run_Check,                "系统运行：PCS启动->启动PQ模式"},
    {SYSStartPCS,         SYS_EVENT_ZERO_POWER,           SYSStopping,        Event_CMD_Stop_Check,               "停机命令：PCS启动->停机中"},
   // {SYSStartPCS,       SYS_EVENT_STOP,                  SYSInit,         Event_CMD_Stop_Check_PCS_Status,    "停机命令：运行->停机"},
    {SYSStartPCS,          SYS_EVENT_STOP,                  SYSInit,          Event_CMD_Time_Out,                   "超时事件：超时->初始化"},
   
    {SYSPQMode,         SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：启动PQ模式->故障停机"},
    {SYSPQMode,         SYS_EVENT_ZERO_POWER,           SYSZEROPOWER,       Event_CMD_Stop_Check,               "停机命令：PQ模式中->停机中"},
    {SYSPQMode,         SYS_EVENT_START_PQ,             SYSSTARTPQ,         Event_SYS_PQ_Check,                 "系统运行：启动PQ模式->PQ模式中"},
    {SYSPQMode,         SYS_EVENT_WARNING,              SYSZEROPOWER,       Event_Warning_Check,                "一般故障事件：启动PQ模式->一般故障停机"},
    {SYSPQMode,         SYS_EVENT_ZERO_POWER,           SYSStopping,        Event_CMD_Stop_Check,               "停机命令：启动PQ模式->停机中"},
    {SYSPQMode,          SYS_EVENT_STOP,                  SYSInit,          Event_CMD_Time_Out,                   "超时事件：超时->初始化"},


    {SYSSTARTPQ,        SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：启动PQ模式->故障停机"},
    {SYSSTARTPQ,        SYS_EVENT_WARNING,              SYSZEROPOWER,       Event_Warning_Check,                "一般故障事件：启动PQ模式->一般故障停机"},
    {SYSSTARTPQ,        SYS_EVENT_PQ,                   SYSRun,             Event_SYS_PQ_Mode_Check,            "系统运行：PQ模式中->系统运行"},
  //  {SYSSTARTPQ,       SYS_EVENT_STOP,                  SYSInit,         Event_CMD_Stop_Check_PCS_Status,       "停机命令：运行->停机"},
    {SYSSTARTPQ,         SYS_EVENT_ZERO_POWER,           SYSStopping,        Event_CMD_Stop_Check,               "停机命令：PQ模式中->停机中"},

    {SYSRun,            SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：运行->故障停机"},
    {SYSRun,            SYS_EVENT_ZERO_POWER,           SYSZEROPOWER,       Event_CMD_Stop_Check,               "停机命令：运行->停机中"},

    {SYSRun,            SYS_EVENT_STOP,                 SYSInit,         Event_CMD_Stop_Check_PCS,           "停机命令：运行->停机"},

    {SYSRun,            SYS_EVENT_WARNING,              SYSZEROPOWER,       Event_Warning_Check,                "一般故障事件：运行->一般故障停机"},

    {SYSRun,            SYS_EVENT_ZERO_POWER,           SYSStandby,         Event_CMD_Standby_Check,            "待机命令：运行->待机"},
    {SYSRun,            SYS_EVENT_START_WARN,           SYSWarnRun,         Event_Start_Warn_Check,             "告警事件：运行->告警运行"},

    {SYSWarnRun,        SYS_EVENT_FAULT,                SYSFault,           Event_Fault_Check,                  "故障事件：告警运行->故障停机"},
    {SYSWarnRun,        SYS_EVENT_ZERO_POWER,           SYSZEROPOWER,       Event_CMD_Stop_Check,               "停机命令：告警运行->停机中"},
    {SYSWarnRun,        SYS_EVENT_WARNING,              SYSZEROPOWER,       Event_Warning_Check,                "一般故障事件：告警运行->一般故障停机"},
    {SYSWarnRun,        SYS_EVENT_STOP,                 SYSInit,               Event_CMD_Stop_Check_PCS,     "停机命令：运行->停机"},
    {SYSWarnRun,        SYS_EVENT_ZERO_POWER,           SYSStandby,         Event_CMD_Standby_Check,            "待机命令：告警运行->待机"},

    {SYSWarnRun,        SYS_EVENT_WARN_RECOVERY,        SYSRun,             Event_Start_Warn_Recovery,          "告警事件：告警恢复->运行"},


    {SYSZEROPOWER,      SYS_EVENT_CMD_STOP,             SYSSTOPPQ,          Event_SYS_ZERO_POWER_Check,         "停机命令：停机中->下发零功率"},

    {SYSSTANDYZEROPOWER,SYS_EVENT_STANDBY_ZERO_POWER,   SYSStandby,         Event_SYS_STANDBY_ZERO_POWER_Check, "待机命令：下发零功率->待机"},
    {SYSStandby,        SYS_EVENT_STANDBY_RUN,            SYSRun,           Event_SYS_STANDBY_STATUS_Check,     "开机命令：待机->运行"},
    {SYSSTOPPQ,         SYS_EVENT_STOP_PQ_CHECK,        SYSSTOPPQED,        Event_CMD_Stop_PQ_Check,            "停机命令：下发零功率->停PQ"},
    {SYSSTOPPQED,       SYS_EVENT_STOP_PQ,              SYSStopping,        Event_SYS_STOP_PQ_Mode_Check,       "停机命令：停PQ->正在停机"},

};

#define LIST_SIZE (sizeof(state_list) / sizeof(StateSYSform))



INT8U Debug_State_Sys(INT8U sys_num, INT8U sta)
{
    (void)sys_num;
    INT8U debug_sta = 0;
    return (debug_sta == sta) ? 1 : 0;
}

SYS_State_ENUM Get_State_Sys(INT8U sys_num)
{
    return SYS_state[sys_num];
}

uint64_t Get_State_Time_Sys(INT8U sys_num)
{
    uint64_t t = 0;
    if (Timer_GetTick() > last_time[sys_num])
        t = Timer_GetTick() - last_time[sys_num];
    return t;
}

INT8U Set_Out_Sys(INT8U sys_num, INT16U evt)
{
    event_out[sys_num]     = evt;
    event_out_time[sys_num] = Timer_GetTick();
    return event_out[sys_num];
}

INT8U Get_Out_Sys(INT8U sys_num)
{
    return event_out[sys_num];
}

uint64_t Get_Out_Time_Sys(INT8U sys_num)
{
    uint64_t t = 0;
    if (Timer_GetTick() > event_out_time[sys_num])
        t = Timer_GetTick() - event_out_time[sys_num];
    return t;
}

void Clear_Out_Sys(INT8U sys_num)
{
    event_out_time[sys_num] = Timer_GetTick();
    event_out[sys_num]      = 0;
}

void Wait_Out_Sys(INT8U sys_num)
{
    if ((event_out[sys_num] > 0) && (event_out[sys_num] <= 100))
        event_out[sys_num] += 100;
}

void Success_Out_Sys(INT8U sys_num)
{
    if ((event_out[sys_num] > 100) && (event_out[sys_num] <= 200))
        event_out[sys_num] += 100;
}

INT8U Set_In_Sys(INT8U sys_num, INT16U evt)
{
    event_in[sys_num]   = evt;
    event_in_time[sys_num] = Timer_GetTick();
    return event_in[sys_num];
}

INT8U Get_In_Sys(INT8U sys_num)
{
    return event_in[sys_num];
}

void Clear_In_Sys(INT8U sys_num)
{
    event_in_time[sys_num] = Timer_GetTick();
    event_in[sys_num]      = 0;
}

void Wait_In_Sys(INT8U sys_num)
{
    if ((event_in[sys_num] > 0) && (event_in[sys_num] <= 100))
        event_in[sys_num] += 100;
}

void Success_In_Sys(INT8U sys_num)
{
    if ((event_in[sys_num] > 100) && (event_in[sys_num] <= 200))
        event_in[sys_num] += 100;
}

uint64_t Get_In_Time_Sys(INT8U sys_num)
{
    uint64_t t = 0;
    if (Timer_GetTick() > event_in_time[sys_num])
        t = Timer_GetTick() - event_in_time[sys_num];
    return t;
}


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
    //     for (INT8U sub = 0; sub < SUBS_PER_SYS; ++sub) 
    // {
    //     Clear_In_Sub(sys_num, sub);
    //     Clear_Out_Sub(sys_num, sub);
    //     if (Get_State_Sub(sys_num, sub) != SUBInit) 
    //     {
    //         SUB_state[sys_num * SUBS_PER_SYS + sub] = SUBInit;
    //     }
    // }  
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
void Event_Fault_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
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
static INT32U event_timeout_start[MAX_SYS_NUM] = {0};
static void Event_CMD_Time_Out(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    INT32U now = Timer_GetTick();

    // 1️ 判断是否超时
    if ((now - event_timeout_start[sys_num]) >= EVENT_TIMEOUT_MS)
    {
        *(eid + sys_num) = SYS_EVENT_STOP;
        Clear_In_Sys(sys_num);
        Clear_Out_Sys(sys_num);

  
        event_timeout_start[sys_num] = now;
    }
}



volatile static INT8U warningflag[2]={0};

static INT8U s_prev_warn[2] = {0};              // 上一次告警状态（用于边沿）

#define WARN_DEBOUNCE_TICKS_100ms   (10u)   /* 10ms tick -> 5s */
static inline INT8U bms_is_online(INT16U mask, INT8U bitpos)
{
    if (bitpos >= 16) return 0;                // 简单保护；若只有 8 台也可以 >=8
    return (INT8U)((mask >> bitpos) & 0x01u);
}
/**
 * @brief 检查并处理系统告警事件
 * 
 * 该函数用于检查指定系统的各种告警条件，包括BMS故障、PCS故障和系统级故障。
 * 当检测到告警时，会进行防抖处理（连续5s稳定才确认告警），并触发相应的告警事件。
 * 
 * @param eid 指向事件ID数组的指针，用于存储触发的告警事件ID
 * @param sys_num 系统编号，用于指定要检查的系统
 * @param real_state 系统实际状态（当前未使用）
 * 
 * @note 函数内部会检查以下故障类型：
 *       1. BMS系统故障（包括系统故障总、通讯故障、一级故障）
 *       2. PCS模块故障（通过PrintModuleFaultWords打印）
 *       3. 系统级故障（包括电源故障、通讯故障、心跳丢失等）
 * 
 * @warning 当检测到严重告警（SYSTEM_FAULT_SUMMARY）时，会覆盖一般告警状态
 */
static void Event_Warning_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();


    INT8U is_warn_raw = 0;//主从机都需要停机
    INT8U is_warn_no_stop = 0;//主机故障，主机自停，从机不停

    INT8U sub_begin = (INT8U)(sys_num * BMS_PER_SYS);
    INT8U sub_end   = (INT8U)(sub_begin + BMS_PER_SYS);

    // LOG_INFO("sys_num is %d,",sys_num);
    for (INT8U sub = sub_begin; sub < sub_end; ++sub) 
    {

        INT8U sub_in_sys = (INT8U)(sub - sub_begin);

        if(sub==2||sub==3||sub==6||sub==7)
        {
          continue;
        }
        if ( (((GET_INPUT(28039 + sub * 200)) >> 10) & 1u) ||   /* BMS系统故障总 bit10 */
             (((GET_INPUT(28039 + sub * 200)) >> 3)  & 1u) ||   /* EMS与BMS通讯故障报警 bit3 */
              (GET_INPUT(28040 + sub * 200) != 0) )             /* BMS一级故障 */
        {
            // is_warn_raw = 1;
            // if(((GET_INPUT(28039 + sub * 200)) >> 3))
            // {
            //    LOG_INFO("BMS%d EMS与BMS通讯故障报警!",sub);
            // }
            // if((((GET_INPUT(28039 + sub * 200)) >> 10) & 1u))
            // {
            //    LOG_INFO("BMS%dBMS系统故障总!",sub);
            // }
            // if((GET_INPUT(28040 + sub * 200) != 0))
            // {
            //    LOG_INFO("BMS%dBMS一级故障:%d!",sub,GET_INPUT(28040 + sub * 200));
            // }            
            break;
        }
    }

    if (!is_warn_no_stop) {
       //  LOG_INFO("数据采集：%d",GET_INPUT(3427));
            for (INT16U addr = 2819; addr <= 2830; ++addr) {
                if (GET_INPUT(addr + 0 * 300) != 0) { is_warn_no_stop = 1; break; }
            }
           for (INT16U addr = 2819; addr <= 2830; ++addr) {
 
                if (GET_INPUT(addr + 2 * 300) != 0) { is_warn_no_stop = 1; break; }
            }
            if (GET_INPUT(2817 + 0 * 300) != 0) { is_warn_no_stop = 1;}
            if (GET_INPUT(2817 + 2 * 300) != 0) { is_warn_no_stop = 1;}
            for (INT16U addr = 2833; addr <= 2844; ++addr) {
                if (GET_INPUT(addr + 0 * 300) != 0) { is_warn_no_stop = 1; break; }
            }
            for (INT16U addr = 2833; addr <= 2844; ++addr) {
                if (GET_INPUT(addr + 2 * 300) != 0) { is_warn_no_stop = 1; break; }
            }
    }
            //PrintModuleFaultWords(0);//打印PCS0一般告警信息
            // PrintModuleFaultWords(2);//打印PCS2一般告警信息


    if (!is_warn_no_stop) {
        if ( GET_INPUT(17066 + sys_num * 300) != 0 ||                   /* 系统故障1 */

            (((GET_INPUT(17067 + sys_num * 300)) >> 0)  & 1u) ||        /* CtrlPowerLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 1)  & 1u) ||        /* CommPowerLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 2)  & 1u) ||        /* IOPowerLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 3)  & 1u) ||        /* SDSPFPGAEmifError */
            (((GET_INPUT(17067 + sys_num * 300)) >> 6)  & 1u) ||        /* CtrlHeartLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 7)  & 1u) ||        /* J1DSPHeartLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 8)  & 1u) ||        /* J2DSPHeartLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 10) & 1u) ||        /* MasterSlave1CommLost */
            (((GET_INPUT(17067 + sys_num * 300)) >> 12) & 1u) ||        /* ExtInputFault2 */

             GET_INPUT(17068 + sys_num * 300) != 0 ||                   /* 系统故障3 */
             GET_INPUT(17069 + sys_num * 300) != 0 )                    /* 系统故障4 */
        {
            is_warn_no_stop = 1;

        }
    }
    /* 3) 系统故障字 */
    if (!is_warn_raw) {
        if (
            (((GET_INPUT(17067 + sys_num * 300)) >> 4)  & 1u) ||       
            (((GET_INPUT(17067 + sys_num * 300)) >> 5)  & 1u) )                   
        {
            is_warn_raw = 1;

        }
    }


    static INT8U  s_warn_raw_prev[MAX_SYS_NUM]      = {0};
    static uint64_t s_warn_last_change[MAX_SYS_NUM]   = {0};
    static INT8U  s_warn_stable[MAX_SYS_NUM]        = {0};
    static INT8U  s_warn_stable_prev[MAX_SYS_NUM]   = {0};

    uint64_t now = Timer_GetTick();

    /* 原始告警变化，记录时间 */
    if (is_warn_raw != s_warn_raw_prev[sys_num]) 
    {
        s_warn_raw_prev[sys_num]    = is_warn_raw;
        s_warn_last_change[sys_num] = now;
    }

    /* 连续稳定 >= 100ms 才更新稳定态 */
    if ((uint64_t)(now - s_warn_last_change[sys_num]) >= (uint64_t)WARN_DEBOUNCE_TICKS_100ms) 
    {
        s_warn_stable[sys_num] = is_warn_raw;
    }
    
  if (GET_INPUT(SYSTEM_FAULT_SUMMARY) != 1) //如果出现严重告警和一般告警，那么以严重告警为主
  {
    warningflag[sys_num] = s_warn_stable[sys_num] ? IsWarn : IsnoWarn;

    if (s_warn_stable[sys_num] && !s_warn_stable_prev[sys_num]) 
    {

        last_time[sys_num] = now;
        *(eid + sys_num)   = SYS_EVENT_WARNING;

        Set_Out_Sys(sys_num, SYS_EVENT_ZERO_POWER);

        LOG_INFO("[WARN] sys=%d warning STABLE-RISE(>=100ms) -> trigger ZERO_POWER", sys_num);
    }
  
    s_warn_stable_prev[sys_num] = s_warn_stable[sys_num];


    s_prev_warn[sys_num] = s_warn_stable[sys_num];
}
}

/**
 * @brief 检查并处理系统启动时的告警状态
 * 
 * 该函数检查系统各个子模块（BMS、PCS、测控、UPS）的告警状态，
 * 如果检测到任何告警，则设置相应的告警标志并记录事件ID。
 * 
 * @param[out] eid 事件ID数组指针，用于存储系统事件ID
 * @param[in] sys_num 系统编号，用于标识当前检查的系统
 * @param[in] real_state 系统实际状态（当前未使用，保留参数）
 * 
 * @note 函数内部会检查以下告警源：
 *       - BMS二级故障（地址28041 + sub*200）
 *       - BMS三级故障（地址28042 + sub*200）
 *       - PCS告警（地址17060 + sys_num*300的第6位）
 *       - 测控告警总（地址27301）
 *       - UPS故障（地址27109的第4位）
 * 
 * @warning 函数会修改eid数组对应位置的值
 */
static void Event_Start_Warn_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    INT8U warningflag=0;
    INT8U is_Warning = IsnoWarn;
    sysPara *sys_cfg = SysConf_GetInfo();

    INT8U sub_begin = (INT8U)(sys_num * BMS_PER_SYS);       
    INT8U sub_end   = (INT8U)(sub_begin + BMS_PER_SYS);

   //LOG_INFO("sys_num is %d",sys_num);
    for (INT8U sub = sub_begin; sub < sub_end; ++sub) 
    {

        INT8U sub_in_sys = (INT8U)(sub - sub_begin);
        if (!SUB_ENABLED_SYS(sys_num, sub_in_sys)) continue;
    if  (GET_INPUT(28041 + sub * 200) != 0//BMS二级故障
        ||GET_INPUT(28042 + sub * 200) != 0 //BMS三级故障
       )
        {
           
            is_Warning = IsWarn;
            if(GET_INPUT(28041 + sub * 200) != 0)
            {
                LOG_INFO("BMS%d二级故障:%d!",sub,GET_INPUT(28041 + sub * 200));
            }
            if(GET_INPUT(28042 + sub * 200) != 0)
            {
                LOG_INFO("BMS%d三级故障:%d!",sub,GET_INPUT(28042 + sub * 200));
            }
            break;
        }
    }

        if (((GET_INPUT(17060 + sys_num * 300)) >> 6) & 1u)//PCS告警

        {
         is_Warning = IsWarn;
         LOG_INFO("system%d告警!",sys_num);

        }
        if (GET_INPUT(27301)==1)//测控告警总

        {
            is_Warning = IsWarn;
            LOG_INFO("测控告警总!");
        }
       if (((GET_INPUT(27109 )) >> 4) & 1u)//UPS故障

        {
            is_Warning = IsWarn;
           LOG_INFO("UPS故障总!");
        }
       warningflag = (is_Warning == IsWarn) ? IsWarn : IsnoWarn;

    if (warningflag== IsWarn ) {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_START_WARN;

    }

}

/**
 * @brief 处理系统告警恢复事件
 * 
 * @param eid 指向事件ID数组的指针，用于存储触发的事件ID
 * @param sys_num 系统编号，标识当前处理的是哪个系统
 * @param real_state 系统当前的实际状态（此参数在函数中未使用）
 * 
 * @details 该函数执行以下操作：
 *          1. 检查系统下所有BMS子模块的告警状态（二级和三级告警）
 *          2. 检查系统级别的告警条件
 *          3. 如果所有告警条件都满足安全状态，则：
 *             - 清除告警标志
 *             - 更新最后时间戳
 *             - 触发告警恢复事件
 *          4. 记录恢复日志信息
 * 
 * @note 函数会遍历系统下所有BMS子模块，检查其告警状态
 *       函数会检查多个系统级告警条件
 *       如果任何告警条件存在，则不会触发恢复事件
 */
static void Event_Start_Warn_Recovery(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{

    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();

    INT8U all_safe = 1;
 

    INT8U sub_begin = (INT8U)(sys_num * BMS_PER_SYS);       
    INT8U sub_end   = (INT8U)(sub_begin + BMS_PER_SYS);

    for (INT8U sub = sub_begin; sub < sub_end; ++sub) 
    {

        INT8U sub_in_sys = (INT8U)(sub - sub_begin);
        if (!SUB_ENABLED_SYS(sys_num, sub_in_sys)) continue;
       
        INT16U bms_warn_codelv2 = GET_INPUT(28041 + sub  * 200);//BMS告警二级
        INT16U bms_warn_codelv3 = GET_INPUT(28042 + sub  * 200);//BMS告警三级    
    
        if ( bms_warn_codelv2 || bms_warn_codelv3) {
            all_safe = 0;
            break;
        }
    }
    if (((((GET_INPUT(17060 + sys_num * 300)) >> 6) & 1u)==1)||(GET_INPUT(27301)==1)||((((GET_INPUT(27109 )) >> 4) & 1u)==1)) 
    {
        all_safe = false;
    } 

    /* -----------------------------
     * 4) 结论：恢复成立 -> 清 warningflag + 触发恢复事件
     * ----------------------------- */
    if (all_safe) 
    {
        warningflag[sys_num] = IsnoWarn;     /* 关键：清 0，避免"粘住" */
        last_time[sys_num]   = Timer_GetTick();
        *(eid + sys_num)     = SYS_EVENT_WARN_RECOVERY;

        LOG_INFO("SYS-%d warning recovery OK -> clear warningflag, emit SYS_WARNING_STOP", sys_num);
    }
    /* else：未恢复，不触发事件，不改 warningflag（由 Event_Warning_Check 继续维持/刷新） */
}

 
/**
 * @brief 事件启动故障检查函数
 * 
 * 该函数用于检查PCS系统启动过程中的故障状态，包括主机和从机的故障检测。
 * 当检测到故障时，会记录故障事件ID并更新最后故障时间。
 * 
 * @param eid 指向事件ID数组的指针，用于存储检测到的故障事件ID
 * @param sys_num 系统编号，用于标识当前检查的是哪个系统
 * @param real_state 系统实际状态枚举值（当前未使用，保留参数）
 * 
 * @note 函数内部会检查以下故障条件：
 *       - 主机PCS故障：通过检查PCS_sys_state的第4位
 *       - 从机PCS故障：通过检查slave_PCS_sys_state的第1位
 *       - 调试状态故障：通过Debug_State_Sys函数检查
 * 
 * @warning 从机故障清除次数(slave_clean_times)小于20时才会记录故障事件
 * 
 * @return 无返回值(void)
 */
static void Event_Start_Fault_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    INT8U  is_Fault = IsNoFault;
    INT16U PCS_sys_state       = GET_INPUT(17000 + 300 * sys_num + 60);
    INT16U slave_PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 62);

    /* 从机故障：((slave_PCS_sys_state >> 1) & 1) == 1 */
    INT8U slave_fault = (INT8U)(((slave_PCS_sys_state >> 1) & 0x1u) == 1u);


    clear_pending[sys_num] =0;

    if ((((PCS_sys_state >> 4) & 0x1u) == 1u)&&(slave_fault==1))
    {
        is_Fault = IsFault;
    }

    if ((is_Fault == IsFault) ) 
    {
    //    if (slave_clean_times[sys_num]<20)
    //    {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_START_FAULT;
    //    }
    }
}



/**
 * @brief 检查并处理故障清除事件
 * 
 * 该函数用于检查PCS系统的主从故障状态，并在检测到故障后启动一个5秒的延时清除流程。
 * 如果故障持续存在超过5秒，将触发故障清除事件。如果故障已清除，则设置相应的清除事件ID。
 * 
 * @param eid    事件ID数组指针，用于存储系统事件状态
 * @param sys_num 系统编号，用于标识当前处理的系统
 * @param real_state 系统实际状态枚举值（当前未使用）
 * 
 * @note 函数使用非阻塞方式实现5秒延时清除流程
 * @note 从机故障最多尝试清除10次，超过后将强制清除故障状态
 * @note 函数会更新last_time[sys_num]为当前系统时间戳
 * 
 * @warning 函数依赖全局变量：last_time[], clear_pending[], clear_req_ts[], slave_clean_times[]
 * @warning 函数会修改传入的eid数组内容
 */
static void Event_Clear_Fault_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    INT16U PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 60);
    INT8U  master_fault  = (INT8U)(((PCS_sys_state >> 4) & 0x1u) == 1u);
    INT16U slave_PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 62);
    INT8U  slave_fault = (INT8U)(((slave_PCS_sys_state >> 1) & 0x1u) == 1u);
   // INT8U fault_now = (INT8U)(master_fault || slave_fault);

    uint64_t now = Timer_GetTick();
    last_time[sys_num] = now;

    /* 触发一次"5秒后置位"的流程（非阻塞） */
    if ((master_fault!=1)||(slave_fault!=1) ) 
    {
        clear_pending[sys_num] = 1u;
        //clear_req_ts[sys_num]  = now;
        LOG_INFO("Event_Clear_Fault_Check: clear_pending[%d] = 1u\n", sys_num);
    }

    // /* 延时到达：发清理事件 */
    // if (clear_pending[sys_num]) {
    //     if ((now - clear_req_ts[sys_num]) >= TICKS_5S) 
    //     {
   
    //         clear_pending[sys_num] = 0u;
    //     if (slave_fault) 
    //         {
    //             if (slave_clean_times[sys_num] < 10u) 
    //             {
    //                 slave_clean_times[sys_num]++;   // 
    //            // LOG_INFO("清理次数 ：%d",slave_clean_times[sys_num]);
    //             }
    //             else 
    //             {
    //              slave_fault=0;
    //             }
    //         } 
    //     else 
    //         {

    //             slave_clean_times[sys_num] = 0;
    //         }

    //        Set_Out_Sys(sys_num, SYS_EVENT_START_FAULT);
    //     }
    // }
    if(clear_pending[sys_num]==1)
    {

        *(eid + sys_num) = SYS_EVENT_START_FAULT_CLEAR;  
        LOG_INFO("Event_Clear_Fault_Check: clear_pending[%d] = 1u\n", sys_num);
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
        for (INT16U sysnum = 0; sysnum < sys_cfg->sysNum; sysnum++)
        {
            pcs_sys_state = GET_INPUT(17000 + 300 * sysnum + 60);
           if ((((((pcs_sys_state >> 1 )&1u)== 1)&&(((pcs_sys_state >> 2 )&1u)== 1)&&(((pcs_sys_state >> 3 )&1u)== 1))))
            {
                all_close = 0;
                break;
            }

        if (!all_close) break;
        }
        if (all_close)
        {     
         
            if (measureflag == 1)
            {
      
            uint64_t now = Timer_GetTick();
            if (!flag_timer_started) 
            {
                    flag_set_start = now;
                    flag_timer_started = 1;
            }
            if (flag_timer_started && !measurecloseflag) 
            {

                    measureflag = 0;
                    measurecloseflag = true;
                    LOG_INFO("发送关闭RMU信号！");


            }            

            }
        }
   //  LOG_INFO("event_in :%d,sys_num : %d", event_in[sys_num],sys_num);
    if ((SYS_EVENT_CMD_START == Get_In_Sys(sys_num)) ||
        (Debug_State_Sys(sys_num, SYS_EVENT_CMD_START) == 1)) 
        {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_CMD_START;
        event_timeout_start[sys_num] = Timer_GetTick();
        }
}

/**
 * @brief 检查系统停止PQ模式事件
 * 
 * @param eid 事件ID数组指针，用于存储检测到的事件ID
 * @param sys_num 系统编号，用于标识当前检查的系统
 * @param real_state 系统实际状态（未使用）
 * 
 * @note 该函数检查系统是否处于停止PQ模式，并更新相应的事件ID和时间戳
 * @note 函数会忽略real_state参数
 * @note Debug_State_Sys变量未使用
 */
static void Event_SYS_STOP_PQ_Mode_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    (void)Debug_State_Sys;

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_STOP_PQ;
 
}

/**
 * @brief 检查系统零功率事件
 * 
 * @param eid 指向事件ID数组的指针，用于存储生成的事件ID
 * @param sys_num 系统编号，用于指定要检查的系统
 * @param real_state 系统实际状态（本函数中未使用）
 * 
 * @note 该函数检查指定系统是否处于零功率状态，如果是则生成停止命令事件
 * @note 函数内部会记录当前时间戳到last_time数组中
 * @note 函数会忽略real_state和Debug_State_Sys参数
 * 
 * @return 无返回值
 */
static void Event_SYS_ZERO_POWER_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    (void)Debug_State_Sys;

    if ((SYS_EVENT_ZERO_POWER + 200) == Get_Out_Sys(sys_num)) 
    {
        last_time[sys_num] = Timer_GetTick();

        *(eid + sys_num)   = SYS_EVENT_CMD_STOP;  
    
    }
}
/**
 * @brief 检查系统零功耗待机事件
 * 
 * @param eid 事件ID数组指针，用于存储触发的事件ID
 * @param sys_num 系统编号
 * @param real_state 系统实际状态（未使用）
 * @note 该函数会检查系统是否处于零功耗状态，如果是则触发待机零功耗事件
 * @warning 函数会修改Standby_Cmd_Zero_Power[sys_num]和eid[sys_num]的值
 * @see SYS_EVENT_ZERO_POWER
 * @see SYS_EVENT_STANDBY_ZERO_POWER
 * @see Timer_GetTick()
 */
static void Event_SYS_STANDBY_ZERO_POWER_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    (void)Debug_State_Sys;

    if ((SYS_EVENT_STANDBY + 200) == Get_Out_Sys(sys_num)) 
    {
        last_time[sys_num] = Timer_GetTick();
        if(Standby_Cmd_Zero_Power[sys_num]==1)
        {

         Standby_Cmd_Zero_Power[sys_num]=0;  
        *(eid + sys_num)   = SYS_EVENT_STANDBY_ZERO_POWER;


        }

    }
}
/**
 * @brief 检查系统待机状态并处理相应事件
 * 
 * 该函数用于检查指定系统的待机状态，当检测到特定输入状态时，
 * 会更新系统事件ID并记录当前时间戳。
 * 
 * @param eid 指向系统事件ID数组的指针，用于存储触发的事件ID
 * @param sys_num 系统编号，用于标识当前检查的是哪个系统
 * @param real_state 系统实际状态参数（当前未使用）
 * 
 * @note 函数会检查输入状态GET_INPUT(17063 + sys_num * 300)是否等于8
 *       （PCS System Operation Status），如果是则触发SYS_EVENT_CMD_START事件
 * 
 * @warning 函数会忽略real_state和Debug_State_Sys参数
 * 
 * @return 无返回值
 */
static void Event_SYS_STANDBY_STATUS_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    (void)Debug_State_Sys;

 
      
       if( (GET_INPUT(17063 + sys_num * 300) == 8 )|| (GET_INPUT(17062 + sys_num * 300) == 4 ))//PCS System Operation Status
        {
        last_time[sys_num] = Timer_GetTick();
 
        *(eid + sys_num)   = SYS_EVENT_STANDBY_RUN;


        }

    
}
// static void Event_CMD_Reset_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
// {
//     (void)real_state;

//     if ((SYS_EVENT_CMD_RESET == Get_In_Sys(sys_num)) ||
//         (Debug_State_Sys(sys_num, SYS_EVENT_CMD_RESET) == 1)) {
//         sysPara *sys_cfg = SysConf_GetInfo();
//         last_time[sys_num] = Timer_GetTick();
//         *(eid + sys_num)   = SYS_EVENT_CMD_RESET;

//         for (INT8U sub = 0; (sub < sys_cfg->subNum) && (sub < MAX_SUB_NUM); sub++) {
//             if (!SUB_ENABLED_SYS(sys_num, sub)) continue;
//             Set_In_Sub(sys_num, sub, SUB_EVENT_CMD_RESET);
//         }
//         Clear_In_Sys(sys_num);
//     }
// }

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

        SET_HOLD(27107 + sys_num * 300, 0);
        Set_Out_Sys(sys_num, SYS_EVENT_ZERO_POWER);

        LOG_INFO("下发关机: sys_num=%d", sys_num);

        pcs_num = (sys_num!= 0) ? 2 : 0;
        pcs_addr = (sys_num!= 0) ? 1 : 0;
     if (pcs_num >= 0)
    {
        PcsWriteReq pcsreq;
        pcsreq.addr     = 27000+300*sys_num+109;   //清除视在功率
        pcsreq.value    = 0;
        pcsreq.len      = 1;
        pcsreq.is_multi = false;

        // 去重，保留最新值
        LOG_INFO("addr is %d,num is %d",pcsreq.addr ,pcs_num);
        Pcs_Write_Enqueue_Dedup_By_Addr(pcs_num, &pcsreq);//放入缓冲区中
    }



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


static void Event_CMD_Run_Check_PCS_Status(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    INT16U PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 60);
    INT8U  master_run  = (INT8U)(((PCS_sys_state >> 3) & 0x1u) == 1u);
    INT16U slave_PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 62);
    INT8U  slave_run = (INT8U)(((slave_PCS_sys_state >> 2) & 0x1u) == 1u);
  
    if ((master_run==1) || ((slave_run==1))
) 
    {

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_STOPPED_RUN;

    }
}
/**
 * @brief 检查并处理待机命令事件
 * 
 * @param eid 事件ID数组指针，用于存储系统事件ID
 * @param sys_num 系统编号，指定要处理的系统
 * @param real_state 系统实际状态（未使用）
 * 
 * @note 当系统处于待机命令状态或调试状态为待机命令时，执行以下操作：
 *       1. 设置系统输出为零功率状态
 *       2. 标记待机命令零功率状态为真
 *       3. 记录日志信息
 *       4. 更新最后时间戳
 *       5. 更新事件ID为零功率事件
 *       6. 清除系统输入状态
 * 
 * @attention 参数real_state被显式忽略，保留用于未来扩展
 * @attention 函数内部使用了全局变量：Standby_Cmd_Zero_Power[]和last_time[]
 */
static void Event_CMD_Standby_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    if ((SYS_EVENT_CMD_STANDBY == Get_In_Sys(sys_num)) ||
        (Debug_State_Sys(sys_num, SYS_EVENT_CMD_STANDBY) == 1)) 
    {

      
        Set_Out_Sys(sys_num, SYS_EVENT_STANDBY);
        Standby_Cmd_Zero_Power[sys_num]=true;
        LOG_INFO("下发零功率: sys_num=%d", sys_num);

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_ZERO_POWER;
        Clear_In_Sys(sys_num);
    }
}


/**
 * @brief 检查并处理系统重置命令事件
 * 
 * @param eid 指向事件ID数组的指针，用于存储触发的事件ID
 * @param sys_num 系统编号，用于标识具体是哪个系统
 * @param real_state 系统当前的实际状态（本函数中未使用）
 * 
 * @note 函数会检查当前系统是否收到重置命令或调试状态下是否触发重置，
 *       若满足条件则执行重置操作并记录事件
 * 
 * @warning 函数会修改eid指向的数组元素和last_time全局变量
 */
static void Event_CMD_Reset_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    if ((SYS_EVENT_CMD_RESET == Get_In_Sys(sys_num)) ||
        (Debug_State_Sys(sys_num, SYS_EVENT_CMD_RESET) == 1)) 
    {

        SET_HOLD(27107 + sys_num * 300, 0);
        Set_Out_Sys(sys_num, SYS_EVENT_CMD_RESET);

        LOG_INFO("下发重置: sys_num=%d", sys_num);

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_CMD_RESET;
        Clear_In_Sys(sys_num);
    }
}


/**
 * @brief 停止PQ检查事件处理函数
 * 
 * @param eid 事件ID指针，用于存储生成的事件ID
 * @param sys_num 系统编号
 * @param real_state 系统当前状态（本函数未使用该参数）
 * 
 * @note 该函数会：
 *       1. 设置对应系统的事件ID为SYS_EVENT_STOP_PQ_CHECK
 *       2. 调用Set_Out_Sys()设置系统输出事件为SYS_EVENT_STOP_PQ
 *       3. 记录停止PQ的日志信息
 * 
 * @return 无返回值
 */
static void Event_CMD_Stop_PQ_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    *(eid + sys_num) = SYS_EVENT_STOP_PQ_CHECK;
    Set_Out_Sys(sys_num, SYS_EVENT_STOP_PQ);
    LOG_INFO("停止PQ");
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
   int pcs_num=0;
    int pcs_addr=0;
    last_time[sys_num] = Timer_GetTick();
    *(eid + sys_num)   = SYS_EVENT_FAULT_STOP;
    Set_Out_Sys(sys_num, SYS_EVENT_ZERO_POWER); /* 下发 0 功率 */

      pcs_num = (sys_num!= 0) ? 2 : 0;
        pcs_addr = (sys_num!= 0) ? 1 : 0;
     if (pcs_num >= 0)
    {
        PcsWriteReq pcsreq;
        pcsreq.addr     = 27000+300*sys_num+109;   
        pcsreq.value    = 0;
        pcsreq.len      = 1;
        pcsreq.is_multi = false;

        // 去重，保留最新值
        LOG_INFO("addr is %d,num is %d",pcsreq.addr ,pcs_num);
        Pcs_Write_Enqueue_Dedup_By_Addr(pcs_num, &pcsreq);//放入缓冲区中
    }

    Clear_In_Sys(sys_num);
}

/**
 * @brief 检查并等待系统复位事件
 * 
 * 该函数用于检查系统是否需要执行复位操作。当检测到系统输出状态为复位命令时，
 * 更新系统最后活动时间，并将对应的事件ID设置为等待复位状态。
 * 
 * @param eid 指向事件ID数组的指针，用于存储系统事件状态
 * @param sys_num 系统编号，用于标识当前操作的子系统
 * @param real_state 系统当前的实际状态（该参数未使用）
 * 
 * @note 该函数会修改eid数组和last_time数组
 * @note real_state参数被显式忽略（(void)real_state）
 * 
 * @return 无返回值
 */
static void Event_Wait_Reset_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;


    if ((SYS_EVENT_CMD_RESET + 200) == Get_Out_Sys(sys_num)) 
    {
        last_time[sys_num] = Timer_GetTick();
      
            *(eid + sys_num) = SYS_EVENT_WAIT_RESET;
        
    }
}

/**
 * @brief 检查并执行PCS停机事件
 * 
 * @param eid 事件ID数组指针，用于存储系统事件ID
 * @param sys_num 系统编号，用于标识具体的系统
 * @param real_state 系统实际状态，此参数在函数中未使用
 * 
 * 该函数用于检查系统是否满足PCS停机条件，并在满足条件时执行停机操作。
 * 停机条件包括：
 * 1. 系统有功功率在零功率限制范围内
 * 2. 系统处于调试状态且停机事件被触发
 * 
 * 当满足停机条件时，函数会：
 * 1. 延时1秒
 * 2. 记录当前时间
 * 3. 设置系统事件ID为停机事件
 * 4. 更新系统输出状态
 * 5. 记录停机日志
 */
static void Event_Stop_PCS_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;
    int pcs_num=0;
    int pcs_addr=0;
    u32_conv sys_active_power;
    sys_active_power.DS32 = GET_INPUT(125);  
   INT32S Atvice_power = ((INT16S)GET_INPUT(17000 + sys_num * 300 + 40) )*10;
    if (( Atvice_power <  ZERO_POWER_LIMIT &&
          Atvice_power > -ZERO_POWER_LIMIT) ||
        (Debug_State_Sys(sys_num, SYS_EVENT_STOP_PCS) == 1)) 
    {
        sleep(1);
        last_time[sys_num] = Timer_GetTick();
        // Clear_Out_Sys(sys_num);
    

        //  Set_Out_Sys(sys_num, SYS_EVENT_STOP_PCS);
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
        LOG_INFO("发送停机：%d",sys_num);
        *(eid + sys_num)   = SYS_EVENT_STOP_PCS;
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
static void Event_Stop_SUB_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
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
  
    if ((((((PCS_sys_state >> 1 )&1u)== 1)&&(((PCS_sys_state >> 2 )&1u)!= 1)&&(((PCS_sys_state >> 3 )&1u)!= 1))|| (((PCS_sys_state >> 4 )&1u)== 1))&&(warningflag[sys_num] == IsWarn)) 
    {
 
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_WARNING_STOP;
    }
 }


#define WARN_DEBOUNCE_TICKS_100ms   (10)   /* 10ms tick -> 5s */

/**
 * @brief 处理系统告警恢复事件
 * @param eid 事件ID数组指针，用于存储产生的事件
 * @param sys_num 系统编号，标识当前处理的是哪个系统
 * @param real_state 系统实际状态（未使用）
 * 
 * 该函数用于检测系统告警恢复条件，主要包含以下步骤：
 * 1. 计算原始恢复条件(all_safe_raw)：
 *    - 检查BMS状态
 *    - 检查PCS状态（根据总线类型S_BUS或其他类型采用不同检查方式）
 *    - 检查系统故障字
 * 2. 对原始恢复条件进行5秒防抖处理，得到稳定的恢复状态
 * 3. 在稳定恢复状态上升沿时触发一次恢复事件：
 *    - 清除告警标志
 *    - 记录恢复时间
 *    - 发送SYS_WARNING_STOP事件
 * 
 * @note 该函数使用静态变量来保存历史状态，实现防抖功能
 */
static void Event_Warning_Recovery(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();

    /* ========== 1) 计算"原始恢复条件" all_safe_raw（原逻辑） ========== */
    INT8U all_safe_raw = 1;

    INT8U sub_begin = (INT8U)(sys_num * BMS_PER_SYS);
    INT8U sub_end   = (INT8U)(sub_begin + BMS_PER_SYS);

    /* 1) BMS 扫描：任意 BMS 仍有告警/故障 => 不安全 */
    for (INT8U sub = sub_begin; sub < sub_end; ++sub) 
    {

        INT8U sub_in_sys = (INT8U)(sub - sub_begin);

        if(sub==2||sub==3||sub==6||sub==7)
        {
          continue;
        }
        INT16U bms_sum_bit10 = (INT16U)(((GET_INPUT(28039 + sub * 200)) >> 10) & 1u);
        INT16U bms_sum_bit3  = (INT16U)(((GET_INPUT(28039 + sub * 200)) >> 3)  & 1u);
        INT16U bms_lv1       = (INT16U) GET_INPUT(28040 + sub * 200);
        if (bms_sum_bit10 || bms_sum_bit3 || (bms_lv1 != 0u)) {
            all_safe_raw = 0;
            break;
        }
    }
    // /* 2) PCS 扫描：只扫当前 sys 的 2 台 */
    // if (all_safe_raw) 
    // {
    //     if(BusType==S_BUS)
    //     {
    //         for (INT16U addr = 2819; addr <= 2830; ++addr) {
    //             if (GET_INPUT(addr + 0 * 300) != 0u) 
    //             { 
    //                 all_safe_raw = 0; 
    //                 break; 
    //             }
    //         }
    //         for (INT16U addr = 2833; addr <= 2844; ++addr) {
    //             if (GET_INPUT(addr +0 * 300) != 0u) 
    //             { 
    //                 all_safe_raw = 0;
    //                 break; 
    //             }
    //         }
    //         for (INT16U addr = 2819; addr <= 2830; ++addr) {
    //             if (GET_INPUT(addr + 2 * 300) != 0u) 
    //             { 
    //                 all_safe_raw = 0; 
    //                 break; 
    //             }
    //         }
    //         if (GET_INPUT(2817 + 0 * 300) != 0) { all_safe_raw = 0;}
    //         if (GET_INPUT(2817 + 2 * 300) != 0) { all_safe_raw = 0;}
    //         for (INT16U addr = 2833; addr <= 2844; ++addr) 
    //         {
    //             if (GET_INPUT(addr + 2 * 300) != 0u)
    //             { 
    //                 all_safe_raw = 0; 
    //                 break; 
    //             }
    //         }
    //     }
    //     else
    //     {
    //         INT8U pcs_begin = (INT8U)(sys_num * 2);
    //         INT8U pcs_end   = (INT8U)(pcs_begin + 2);

    //         if (pcs_end > sys_cfg->pcsNum) pcs_end = (INT8U)sys_cfg->pcsNum;
    //         if (pcs_end > MAX_PCS)         pcs_end = (INT8U)MAX_PCS;
    //         if (pcs_begin > pcs_end)       pcs_begin = pcs_end;

    //         for (INT8U pcs_num = pcs_begin; pcs_num < pcs_end; ++pcs_num) 
    //         {

    //             for (INT16U addr = 2819; addr <= 2830; ++addr) 
    //             {
    //                 if (GET_INPUT(addr + pcs_num * 300) != 0u) 
    //                 { 
    //                     all_safe_raw = 0; 
    //                     break; 
    //                 }
    //             }
    //             if (!all_safe_raw) break;

    //             for (INT16U addr = 2833; addr <= 2844; ++addr) 
    //             {
    //                 if (GET_INPUT(addr + pcs_num * 300) != 0u) 
    //                 { 
    //                     all_safe_raw = 0; 
    //                     break; 
    //                 }
    //             }
    //             if (!all_safe_raw) break;
    //         }
    //     }
    // }

    /* 3) 系统故障字：任意仍置位 => 不安全 */
    if (all_safe_raw) 
    {
        if ( 
            (((GET_INPUT(17067 + sys_num * 300)) >> 4)  & 1u) ||
            (((GET_INPUT(17067 + sys_num * 300)) >> 5)  & 1u)
)
        {
            all_safe_raw = 0;
        }
    }

    /* ========== 2) 5 秒防抖：all_safe_raw -> all_safe_stable ========== */
    static INT8U  s_safe_raw_prev[MAX_SYS_NUM]     = {0};
    static uint64_t s_safe_last_change[MAX_SYS_NUM]  = {0};
    static INT8U  s_safe_stable[MAX_SYS_NUM]       = {0};
    static INT8U  s_safe_stable_prev[MAX_SYS_NUM]  = {0};

    uint64_t now = Timer_GetTick();

    /* 原始安全状态变化，记录变化时间 */
    if (all_safe_raw != s_safe_raw_prev[sys_num]) 
    {
        s_safe_raw_prev[sys_num]     = all_safe_raw;
        s_safe_last_change[sys_num]  = now;
    }

    /* 连续稳定 >=5s 才更新稳定安全态 */
    if ((uint64_t)(now - s_safe_last_change[sys_num]) >= (uint64_t)WARN_DEBOUNCE_TICKS_100ms) 
    {
        s_safe_stable[sys_num] = all_safe_raw;
    }

    /* ========== 3) 稳定恢复上升沿触发一次 ========== */
    if (s_safe_stable[sys_num] && !s_safe_stable_prev[sys_num]) 
    {

        warningflag[sys_num] = IsnoWarn;     /* 清 0，避免"粘住" */
        last_time[sys_num]   = now;
        *(eid + sys_num)     = SYS_WARNING_STOP;

        LOG_INFO("SYS-%d warning recovery STABLE(>=100ms) -> clear warningflag, emit SYS_WARNING_STOP", sys_num);
    }

    s_safe_stable_prev[sys_num] = s_safe_stable[sys_num];
}

/**
 * @brief 检查并启动指定系统的子系统
 * 
 * @param eid 事件ID数组指针，用于存储系统事件
 * @param sys_num 系统编号
 * @param real_state 系统实际状态（未使用）
 * 
 * @note 该函数会检查PCS系统状态，当满足条件时启动所有启用的子系统
 * @note 子系统状态为SUBReady时才会响应启动命令
 * @note 会记录日志并更新最后操作时间
 * 
 * @details 函数执行流程：
 *          1. 获取系统配置信息
 *          2. 检查PCS系统状态的第1位是否置位
 *          3. 遍历所有子系统，检查是否启用
 *          4. 清除子系统输入并检查状态
 *          5. 对处于SUBReady状态的子系统发送启动命令
 *          6. 更新最后操作时间和事件ID
 */
static void Event_Start_SUB_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    INT16U PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 60);
     INT16U slave_PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 62);
    INT8U slave_standby = (INT8U)(((slave_PCS_sys_state >> 0) & 0x1u) == 1u);
    if ((((PCS_sys_state >> 1 )&1u)== 1)||(slave_standby==1)) {
        for (INT8U sub = 0; (sub < sys_cfg->subNum) && (sub < MAX_SUB_NUM); sub++) 
        {
            if (!SUB_ENABLED_SYS(sys_num, sub)) continue;
            Clear_In_Sub(sys_num, sub);

            SUB_State_ENUM s = Get_State_Sub(sys_num, sub);
            if (s == SUBReady) {
                Set_In_Sub(sys_num, sub, SUB_EVENT_CMD_START);
                LOG_INFO("子系统启动: sys=%d, sub=%d", sys_num, sub);
            }
        }
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_START_SUB;
    }
}

static void Event_Start_PCS_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    INT8U is_no_run = 0;
    INT16U PCS_sys_state = (INT16U)GET_INPUT(17000 + 300 * sys_num + 60);
    INT8U  BusDischarge   = (INT8U)(((PCS_sys_state >> 12) & 0x1u) == 0);//台达PCS开机时候该点位必须为0，否则不启动
    /* 读取一次在线位图，避免循环里重复 GET_INPUT */
    INT16U bms_online_mask = (INT16U)GET_INPUT(220);

        if(BusType==D_BUS)
        {

            if(((Get_State_Sub(sys_num, 0) == SUBRun) && (Get_State_Sub(sys_num, 1) == SUBRun)) || ((Get_State_Sub(sys_num, 2) == SUBRun) &&(Get_State_Sub(sys_num, 3) == SUBRun)))
            {
                is_no_run = 0;
            }
            else
            {
                is_no_run = 1;
            }

        }
        else if (BusType==S_BUS)
        {
            if(((Get_State_Sub(sys_num, 0) == SUBRun)|| (Get_State_Sub(sys_num, 1) == SUBRun)) )
            {
                is_no_run = 0;
            }
            else
            {
                is_no_run = 1;
            }
        }




    if (((is_no_run == 0)&&(BusDischarge==1)) || (Debug_State_Sys(sys_num, SYS_EVENT_START_PCS) == 1)) 
    {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_START_PCS;
        Set_Out_Sys(sys_num, SYS_EVENT_START_PCS);


    }
}


/**
 * @brief 检查系统PQ事件是否满足触发条件
 * 
 * @param[in,out] eid 事件ID数组指针，用于存储触发的事件ID
 * @param[in] sys_num 系统编号，用于标识具体的系统
 * @param[in] real_state 系统的实际状态，当前未使用
 * 
 * @note 该函数检查以下条件是否满足：
 *       1. 所有子系统都处于运行状态，或者系统处于调试状态
 *       2. PCS系统状态的第1、2、3位都为1
 *       满足条件时触发SYS_EVENT_START_PQ事件
 * 
 * @warning 不要修改real_state参数，虽然传入但当前未使用
 */
static void Event_SYS_PQ_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    INT8U is_no_run = 0;

    INT8U PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 60);

    INT16U slave_PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 62);
    INT8U slave_run = (INT8U)(((slave_PCS_sys_state >> 2) & 0x1u) == 1u);
    if ((((is_no_run == 0) || (Debug_State_Sys(sys_num, SYS_EVENT_START_PQ) == 1)) &&
        (((PCS_sys_state >> 1) & 1u) == 1) &&
        (((PCS_sys_state >> 2) & 1u) == 1) &&
        (((PCS_sys_state >> 3) & 1u) == 1))||(slave_run==1)
    
    ) 
    
    {

        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_START_PQ;
        Set_Out_Sys(sys_num, SYS_EVENT_START_PQ);  /* 下发 PQ */
    }
}

/**
 * @brief 检查系统PQ模式状态并设置相应的事件ID
 * 
 * 该函数用于检查指定系统的PQ模式状态，根据子系统的运行状态和PCS系统模式
 * 来决定是否触发PQ事件。该函数会更新last_time时间戳和事件ID数组。
 * 
 * @param eid 指向事件ID数组的指针，用于存储触发的事件ID
 * @param sys_num 系统编号，用于标识要检查的系统
 * @param real_state 系统的实际状态（参数未使用）
 * @note 函数会检查所有子系统的运行状态，如果所有子系统都处于运行状态或
 *       处于调试启动PQ模式，且PCS系统模式位0为1时，才会触发PQ事件
 * @note 函数会根据总线类型(S_BUS或其他)执行相同的逻辑
 * @warning 函数会修改eid数组和last_time数组
 */
static void Event_SYS_PQ_Mode_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    INT8U is_no_run = 0;
    INT8U PCS_sys_mode = GET_HOLD(27000 + 300 * sys_num + 16);

    if(BusType==S_BUS)
    {
           // &&((SYS_EVENT_START_PQ + 200) == Get_Out_Sys(sys_num))
        if ((is_no_run == 0 || (Debug_State_Sys(sys_num, SYS_EVENT_START_PQ) == 1))     
    &&((PCS_sys_mode  == 1))
    
    ) {
            last_time[sys_num] = Timer_GetTick();
            *(eid + sys_num)   = SYS_EVENT_PQ;
        }
    }
    else
    {
           // 
        if ((is_no_run == 0 || (Debug_State_Sys(sys_num, SYS_EVENT_START_PQ) == 1)) 
     &&((PCS_sys_mode  == 1))
    ) {
            last_time[sys_num] = Timer_GetTick();
            *(eid + sys_num)   = SYS_EVENT_PQ;
        }
    }
}

static void Event_Timeout_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)eid;
    (void)sys_num;
    (void)real_state;

    
}

/**
 * @brief 检查并处理系统等待停止状态的事件
 * 
 * 该函数用于检查指定系统的所有子系统是否已停止，并根据PCS系统状态和调试状态
 * 决定是否触发SYS_EVENT_WAIT_STOPED事件。当满足停止条件时，将更新系统事件ID
 * 并重置相关计时器和标志位。
 * 
 * @param eid 事件ID数组指针，用于存储系统当前事件
 * @param sys_num 系统编号，指定要检查的系统
 * @param real_state 系统实际状态（未使用，保留参数）
 * 
 * @note 函数会检查以下条件：
 *       1. 所有子系统是否处于Ready或Fault状态
 *       2. PCS系统状态是否满足停止条件
 *       3. 系统是否处于调试等待停止状态
 * @note 满足任一条件时会触发SYS_EVENT_WAIT_STOPED事件
 * @note 函数内部会处理特殊字符如\t, \r, \n
 */
static void Event_Wait_Stopped_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    INT8U is_no_stopped = 0;
    INT8U PCS_sys_state = GET_INPUT(17000 + 300 * sys_num + 60);


    for (INT8U sub = 0; (sub < sys_cfg->subNum) && (sub < MAX_SUB_NUM); sub++) 
    {
        if (!SUB_ENABLED_SYS(sys_num, sub)) continue;
        SUB_State_ENUM s = Get_State_Sub(sys_num, sub);
        if ((s != SUBReady) && (s != SUBFault)) 
        {
            is_no_stopped = 1;
           
            break;
        }
    }

    if (((is_no_stopped == 0) &&
        ((((PCS_sys_state >> 1 )&1u)== 1)&&(((PCS_sys_state >> 2 )&1u)!= 1)&&(((PCS_sys_state >> 3 )&1u)!= 1))|| (((PCS_sys_state >> 1 )&1u)== 0)||(((PCS_sys_state >> 4 )&1u)== 1))||
        (Debug_State_Sys(sys_num, SYS_EVENT_WAIT_STOPED) == 1)) 
    {
        last_time[sys_num] = Timer_GetTick();
         event_in[sys_num]=0;
        slave_clean_times[sys_num]=0;
        *(eid + sys_num)   = SYS_EVENT_WAIT_STOPED;
    }
}

/**
 * @brief 检查子系统状态并触发相应事件
 * 
 * 该函数用于检查指定系统的所有子系统状态，当满足特定条件时触发子系统检查事件。
 * 检查条件包括：
 * 1. 任一子系统处于限制状态(SUBLim)
 * 2. 系统处于当前状态的时间超过5000ms
 * 3. 系统处于调试模式(SYS_EVENT_CHECK_SUB)
 * 
 * @param eid 事件ID数组指针，用于存储触发的事件ID
 * @param sys_num 系统编号，指定要检查的系统
 * @param real_state 系统当前实际状态(本函数中未使用)
 * 
 * @note 函数会修改eid数组和last_time全局变量
 * @note 最大检查的子系统数量由MAX_SUB_NUM定义
 * @note 当检测到任一子系统处于限制状态时立即终止检查
 */
static void Event_Check_Sub_Check(INT16U *eid, INT8U sys_num, SYS_State_ENUM real_state)
{
    (void)real_state;

    sysPara *sys_cfg = SysConf_GetInfo();
    uint64_t t = Get_State_Time_Sys(sys_num);
    INT8U is_lim = 0;

    for (INT8U sub = 0; (sub < sys_cfg->subNum) && (sub < MAX_SUB_NUM); sub++) 
    {
        SUB_State_ENUM s = Get_State_Sub(sys_num, sub);
        if (s == SUBLim) 
        { 
            is_lim = 1; 
            break; 
        }
    }

    if ((is_lim == 0) || (t > 5000) || (Debug_State_Sys(sys_num, SYS_EVENT_CHECK_SUB) == 1)) 
    {
        last_time[sys_num] = Timer_GetTick();
        *(eid + sys_num)   = SYS_EVENT_CHECK_SUB;
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
        else if (!curr_fault && s_prev_comm_fault[pcs]) 
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
 */
static void Event_SYS_Run_Check(INT16U *event_id, INT8U sys_num, SYS_State_ENUM real_state)
{
    uint64_t t= 0;
    INT8U sub_num;
    INT8U is_no_run = 0;
    sysPara *sys_cfg = SysConf_GetInfo();

    INT8U resD = Debug_State_Sys(sys_num,SYS_EVENT_SYS_RUN);

    // LOG_INFO("PCS-%d状态%d", sys_num,Get_Out_Sys(sys_num));
    if(((0==is_no_run)&&((SYS_EVENT_START_PCS+200)==Get_Out_Sys(sys_num)))
    ||(1==resD))
    {
        last_time[sys_num] = Timer_GetTick();
        *(event_id + sys_num) = SYS_EVENT_SYS_RUN;
    }

}
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
    case SYSStopped:  
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

    // LOG_INFO("[GLOBAL106] seen=0x%02X s0=%d rank0=%d s1=%d rank1=%d",
    //          seen,
    //          s0,
    //          sys_state_rank(s0),
    //          s1,
    //          sys_state_rank(s1));

    // SET_INPUT(106,
    //     (sys_state_rank(s1) > sys_state_rank(s0)) ? s1 : s0);


    // LOG_INFO("[GLOBAL106] final106=%d", GET_INPUT(106));
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
void SYS_State_Run(volatile SYS_State_ENUM *state, INT8U sys_num)
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
    if (black_end==1) 
    {
        black_end = 0;
        for(uint8_t sys_num=0;(sys_num < sys_cfg->sysNum);sys_num++)
        {
        Clear_In_Sys(sys_num);
        Clear_Out_Sys(sys_num);
        event_id[sys_num]  = 0;
        last_time[sys_num] = Timer_GetTick();
        state[sys_num] = SYSInit;  
       

        for (INT8U sub = 0; sub < SUBS_PER_SYS; ++sub) 
        {
            Clear_In_Sub(sys_num, sub);
            Clear_Out_Sub(sys_num, sub);
            if (Get_State_Sub(sys_num, sub) != SUBInit) 
            {
                SUB_state[sys_num * SUBS_PER_SYS + sub] = SUBInit;
            }
        }
       }  
    }
    /* 1) 在 Stopped/StopSUB/StopPCS/Init 时应用拓扑变化 */
    if (state[sys_num] == SYSStopped ||
        state[sys_num] == SYSStopSUB ||
        state[sys_num] == SYSStopPCS ||
        state[sys_num] == SYSInit    ||
        state[sys_num] == SYSZWARNINGSTOP
    ) {

        if (g_topology_dirty) {
            for (INT8U sub = 0; sub < SUBS_PER_SYS; ++sub) 
            {
                if (!SUB_ENABLED_SYS(sys_num, sub)) 
                {
                    Clear_In_Sub (sys_num, sub);
                    Clear_Out_Sub(sys_num, sub);
                    Clear_In_Sys (sys_num);
                    Clear_Out_Sys(sys_num);

                    if (Get_State_Sub(sys_num, sub) != SUBReady) 
                    {
                        SUB_state[sys_num * SUBS_PER_SYS + sub] = SUBReady;
                    }
                }
            }
            EMS_Rebuild_Enabled_Mask_From_Raw();
            last_time[sys_num] = Timer_GetTick();
            g_topology_dirty   = 0;
            LOG_INFO("[TOPO] applied at stopped: wait EMS to start");
        }
    }

    /* 2) 若该 sys 未启用：不推进状态机 */
    if (!pair_enabled(sys_num)) 
    {
        INT16U out = calc_sys_status_out(sys_num, (INT16U)state[sys_num]);

        /* 锁存逻辑同样适用：未启用不代表可以清锁存 */
        if (severity_rank(out) > 0) {
            if (severity_rank(out) > severity_rank(g_sys_alarm_latch[sys_num])) 
            {
                g_sys_alarm_latch[sys_num] = out;
            }
        } else {
            if (g_sys_alarm_latch[sys_num] != 0 && sys_clear_latch_ok(sys_num)) 
            {
                g_sys_alarm_latch[sys_num] = 0;
            }
        }
        if (g_sys_alarm_latch[sys_num] != 0) out = g_sys_alarm_latch[sys_num];

        g_sys_status_cache[sys_num] = out;
        g_sys_status_seen_mask |= (INT8U)(1u << sys_num);
        LOG_INFO("[GLOBAL106_CACHE] RUN sys=%d state=%d out=%d cache0=%d cache1=%d seen=0x%02X",
         sys_num,
         state[sys_num],
         out,
         g_sys_status_cache[0],
         g_sys_status_cache[1],
         g_sys_status_seen_mask);
        update_global_SYSTEM_STATUS();
        return;
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


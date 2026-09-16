#ifndef REACTIVE_ALLOCATE_H
#define REACTIVE_ALLOCATE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* PCS有效额定视在功率 */
#define PCS_RATED_S_FULL_KVA        2500.0f
#define PCS_RATED_S_HALF_KVA        1250.0f
#define PCS_RATED_S_ZERO_KVA        0.0f

/* PCS运行支路数量 */
#define PCS_BRANCH_NONE             0u
#define PCS_BRANCH_SINGLE           1u
#define PCS_BRANCH_DOUBLE           2u

/* 有功分配系数基准：1000 = 100% */
#define PCS_POWER_COEF_BASE         1000u

/*
 * PCS两个支路状态。
 * 当前容量计算只依赖 branch1_run / branch2_run；
 * standby / fault 字段保留，便于后续诊断或扩展。
 */
typedef struct
{
    uint16_t branch1_run;
    uint16_t branch2_run;

    uint16_t branch1_standby;
    uint16_t branch2_standby;

    uint16_t branch1_fault;
    uint16_t branch2_fault;
} PCS_Branch_Status;

/* 单台PCS用于无功能力计算的信息 */
typedef struct
{
    PCS_Branch_Status branch;

    /* 计划有功，单位kW，正数放电、负数充电 */
    float p_kw;

    /* 当前有效额定视在功率：2500 / 1250 / 0 kVA */
    float rated_s_kva;

    /* 当前运行支路数量：0 / 1 / 2 */
    uint16_t run_branch_num;
} PCS_Q_Info;

/* 有功分配结果，方案A/B共用 */
typedef struct
{
    int16_t group1_total_p;
    int16_t group2_total_p;

    int16_t pcs1_p_set;
    int16_t pcs2_p_set;
    int16_t pcs3_p_set;
    int16_t pcs4_p_set;

    uint16_t group1_master_discharge_coef;
    uint16_t group1_slave_discharge_coef;
    uint16_t group1_master_charge_coef;
    uint16_t group1_slave_charge_coef;

    uint16_t group2_master_discharge_coef;
    uint16_t group2_slave_discharge_coef;
    uint16_t group2_master_charge_coef;
    uint16_t group2_slave_charge_coef;
} MV_Power_Distribution;

/* 无功计算结果，方案A/B共用 */
typedef struct
{
    uint16_t group1_q_cmd;
    uint16_t group2_q_cmd;

    uint16_t group1_q_max;
    uint16_t group2_q_max;
    uint16_t balance_q_max;

    uint16_t group1_q_available_num;
    uint16_t group2_q_available_num;
} MV_Q_Result;

/*
 * 方案A：
 * 当某Group仅剩1台PCS可用时，该Group无功设置值×2，
 * 并限制在 2 × 剩余PCS Qmax 范围内。
 */
void MV_ReactivePowerControl_A(
        uint16_t total_q_cmd,
        uint16_t master1_p,
        uint16_t slave1_p,
        uint16_t master2_p,
        uint16_t slave2_p,
        MV_Q_Result *result);

void MV_PQ_Control_A(
        uint16_t total_q_cmd,
        MV_Power_Distribution *p_result,
        MV_Q_Result *q_result);

/*
 * 方案B：
 * 不进行“单台PCS可用时Group无功设置值×2”补偿，
 * 两个Group保持相同无功设置值。
 */
void MV_ReactivePowerControl_B(
        uint16_t total_q_cmd,
        uint16_t master1_p,
        uint16_t slave1_p,
        uint16_t master2_p,
        uint16_t slave2_p,
        MV_Q_Result *result);

void MV_PQ_Control_B(
        uint16_t total_q_cmd,
        MV_Power_Distribution *p_result,
        MV_Q_Result *q_result);

#ifdef __cplusplus
}
#endif

#endif /* REACTIVE_ALLOCATE_H */

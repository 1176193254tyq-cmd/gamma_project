#include <stdint.h>
#include <math.h>

#include "main.h"
#include "reactive_allocate.h"

/*
 * A/B方案绝大多数计算完全一致：
 *
 * 1. Group总有功 + 充放电系数 -> PCS1~PCS4计划有功
 * 2. PCS支路状态 -> PCS有效容量 2500/1250/0
 * 3. Qmax = sqrt(S^2 - P^2)
 * 4. Group Qmax = 两台可用PCS Qmax之和
 * 5. 两个Group取较小能力作为均衡上限
 *
 * A/B唯一控制差异：
 *
 * A：Group仅剩1台PCS可用时，Group Q设置值×2；
 * B：不做×2，两个Group Q设置值保持相同。
 */


/************************************************************
 * 公共基础函数
 ************************************************************/

static float MV_U16ToSignedFloat(uint16_t raw)
{
    return (float)((int16_t)raw);
}


static uint16_t MV_FloatToU16Signed(float value)
{
    int16_t signed_value;

    if (value > 32767.0f)
    {
        value = 32767.0f;
    }

    if (value < -32768.0f)
    {
        value = -32768.0f;
    }

    /* 向0截断，避免转换后超过理论能力 */
    signed_value = (int16_t)value;

    return (uint16_t)signed_value;
}


static uint16_t MV_PositiveFloatToU16(float value)
{
    if (value <= 0.0f)
    {
        return 0u;
    }

    if (value >= 65535.0f)
    {
        return 65535u;
    }

    return (uint16_t)value;
}


static float MV_Q_Limit(
        float q_cmd,
        float q_max)
{
    if (q_max <= 0.0f)
    {
        return 0.0f;
    }

    if (q_cmd > q_max)
    {
        q_cmd = q_max;
    }

    if (q_cmd < -q_max)
    {
        q_cmd = -q_max;
    }

    return q_cmd;
}


/************************************************************
 * 公共有功分配
 ************************************************************/

static uint16_t MV_LimitPowerCoef(uint32_t coef)
{
    if (coef > PCS_POWER_COEF_BASE)
    {
        return PCS_POWER_COEF_BASE;
    }

    return (uint16_t)coef;
}


/*
 * group_p_set > 0：放电
 * group_p_set < 0：充电
 *
 * Slave直接使用 Group - Master，
 * 保证 Master + Slave == Group，避免整数除法累计误差。
 */
static void MV_CalcGroupActivePower(
        int16_t group_p_set,
        uint16_t master_discharge_coef,
        uint16_t master_charge_coef,
        int16_t *master_p_set,
        int16_t *slave_p_set)
{
    int32_t master_p;

    if ((master_p_set == 0) || (slave_p_set == 0))
    {
        return;
    }

    if (group_p_set == 0)
    {
        *master_p_set = 0;
        *slave_p_set  = 0;
        return;
    }

    if (group_p_set > 0)
    {
        master_p =
            ((int32_t)group_p_set *
             (int32_t)master_discharge_coef)
            / (int32_t)PCS_POWER_COEF_BASE;
    }
    else
    {
        master_p =
            ((int32_t)group_p_set *
             (int32_t)master_charge_coef)
            / (int32_t)PCS_POWER_COEF_BASE;
    }

    *master_p_set = (int16_t)master_p;
    *slave_p_set  = (int16_t)(group_p_set - *master_p_set);
}


/*
 * A/B共用的有功分配：
 *
 * Group1总有功：
 *      GET_HOLD(27000 + 52)
 *
 * Group2总有功：
 *      GET_HOLD(27000 + 300 + 52)
 *
 * 放电：
 *      Group1 Master = 27050 + 27046
 *      Group2 Master = 27350 + 27346
 *
 * 充电：
 *      Group1 Master = 27051 + 27048
 *      Group2 Master = 27351 + 27348
 *
 * Slave系数 = 1000 - Master系数
 */
static void MV_CalcPCSActivePower( MV_Power_Distribution *result)
{
    uint32_t temp_coef;

    if (result == 0)
    {
        return;
    }

    /* ---------------- Group1 ---------------- */

    result->group1_total_p =(int16_t)((uint16_t)GET_HOLD(27000 + 52));

    temp_coef = (uint32_t)GET_HOLD(27000 + 50)+(uint32_t)GET_HOLD(27000 + 46);

    result->group1_master_discharge_coef =MV_LimitPowerCoef(temp_coef);

    result->group1_slave_discharge_coef =PCS_POWER_COEF_BASE-result->group1_master_discharge_coef;

    temp_coef =(uint32_t)GET_HOLD(27000 + 51)+(uint32_t)GET_HOLD(27000 + 48);

    result->group1_master_charge_coef = MV_LimitPowerCoef(temp_coef);

    result->group1_slave_charge_coef = PCS_POWER_COEF_BASE - result->group1_master_charge_coef;

    MV_CalcGroupActivePower(
        result->group1_total_p,
        result->group1_master_discharge_coef,
        result->group1_master_charge_coef,
        &result->pcs1_p_set,
        &result->pcs2_p_set);


    /* ---------------- Group2 ---------------- */

    result->group2_total_p = (int16_t)((uint16_t)GET_HOLD(27000 + 300 + 52));

    temp_coef =(uint32_t)GET_HOLD(27000 + 300 + 50)+(uint32_t)GET_HOLD(27000 + 300 + 46);

    result->group2_master_discharge_coef =MV_LimitPowerCoef(temp_coef);

    result->group2_slave_discharge_coef =PCS_POWER_COEF_BASE-result->group2_master_discharge_coef;

    temp_coef =(uint32_t)GET_HOLD(27000 + 300 + 51)+(uint32_t)GET_HOLD(27000 + 300 + 48);

    result->group2_master_charge_coef = MV_LimitPowerCoef(temp_coef);

    result->group2_slave_charge_coef =PCS_POWER_COEF_BASE-result->group2_master_charge_coef;

    MV_CalcGroupActivePower(
        result->group2_total_p,
        result->group2_master_discharge_coef,
        result->group2_master_charge_coef,
        &result->pcs3_p_set,
        &result->pcs4_p_set);
}


/************************************************************
 * 公共PCS支路状态与容量判断
 ************************************************************/

/*
 * 状态寄存器：
 *
 * bit8  = 支路1运行
 * bit9  = 支路2运行
 * bit10 = 支路1待机
 * bit11 = 支路2待机
 * bit12 = 支路1故障
 * bit13 = 支路2故障
 *
 * 当前容量计算只依据bit8、bit9。
 */
static void PCS_ParseBranchStatus(uint16_t status,PCS_Branch_Status *branch)
{
    if (branch == 0)
    {
        return;
    }

    branch->branch1_run =
        (uint16_t)((status >> 8) & 0x1u);

    branch->branch2_run =
        (uint16_t)((status >> 9) & 0x1u);

    branch->branch1_standby =
        (uint16_t)((status >> 10) & 0x1u);

    branch->branch2_standby =
        (uint16_t)((status >> 11) & 0x1u);

    branch->branch1_fault =
        (uint16_t)((status >> 12) & 0x1u);

    branch->branch2_fault =
        (uint16_t)((status >> 13) & 0x1u);
}


/*
 * 两支路运行  -> 2500kVA
 * 单支路运行  -> 1250kVA
 * 无支路运行  -> 0kVA
 */
static float PCS_GetRatedPowerByBranch(PCS_Branch_Status *branch,uint16_t *run_branch_num)
{
    uint16_t num;

    if ((branch == 0) || (run_branch_num == 0))
    {
        return PCS_RATED_S_ZERO_KVA;
    }

    num =branch->branch1_run+branch->branch2_run;

    *run_branch_num = num;

    if (num >= PCS_BRANCH_DOUBLE)
    {
        return PCS_RATED_S_FULL_KVA;
    }

    if (num == PCS_BRANCH_SINGLE)
    {
        return PCS_RATED_S_HALF_KVA;
    }

    return PCS_RATED_S_ZERO_KVA;
}


/*
 * 当前两份方案代码最终都可以统一为：
 * rated_s_kva > 0 即认为PCS能够参与无功。
 *
 * 原方案B中的run是由bit8/bit9派生，fault固定为0，
 * BMS判断处于注释状态，因此与这里的结果一致。
 */
static uint16_t PCS_Q_IsAvailable(float rated_s_kva)
{
    if (rated_s_kva <= 0.0f)
    {
        return 0u;
    }

    return 1u;
}


/************************************************************
 * 公共无功能力计算
 ************************************************************/

static float PCS_CalcQMax(
        float p_kw,
        float rated_s_kva)
{
    float p_abs;

    if (p_kw < 0.0f)
    {
        p_abs = -p_kw;
    }
    else
    {
        p_abs = p_kw;
    }

    if (rated_s_kva <= 0.0f)
    {
        return 0.0f;
    }

    /*
     * 有功已经达到或超过当前有效视在功率，
     * 无剩余无功能力，同时避免sqrt负数。
     */
    if (p_abs >= rated_s_kva)
    {
        return 0.0f;
    }

    return sqrtf(
        rated_s_kva * rated_s_kva
        -
        p_abs * p_abs);
}


/*
 * A/B共用PCS状态读取：
 *
 * PCS1：17061
 * PCS2：17062
 * PCS3：17361
 * PCS4：17362
 */
static void MV_Q_ReadPCSInfo(
        uint16_t master1_p_raw,
        uint16_t slave1_p_raw,
        uint16_t master2_p_raw,
        uint16_t slave2_p_raw,
        PCS_Q_Info *master1,
        PCS_Q_Info *slave1,
        PCS_Q_Info *master2,
        PCS_Q_Info *slave2)
{
    uint16_t pcs_state;

    if ((master1 == 0) ||
        (slave1 == 0)  ||
        (master2 == 0) ||
        (slave2 == 0))
    {
        return;
    }

    /* PCS1：Group1 Master */
    pcs_state = (uint16_t)GET_INPUT(17061);

    PCS_ParseBranchStatus( pcs_state, &master1->branch);

    master1->rated_s_kva =PCS_GetRatedPowerByBranch( &master1->branch, &master1->run_branch_num);

    master1->p_kw =MV_U16ToSignedFloat(master1_p_raw);
    /* PCS2：Group1 Slave */
    pcs_state = (uint16_t)GET_INPUT(17062);

    PCS_ParseBranchStatus(pcs_state,&slave1->branch);

    slave1->rated_s_kva =PCS_GetRatedPowerByBranch(&slave1->branch,&slave1->run_branch_num);

    slave1->p_kw =MV_U16ToSignedFloat(slave1_p_raw);


    /* PCS3：Group2 Master */
    pcs_state = (uint16_t)GET_INPUT(17361);

    PCS_ParseBranchStatus(pcs_state,&master2->branch);

    master2->rated_s_kva =PCS_GetRatedPowerByBranch(&master2->branch,&master2->run_branch_num);

    master2->p_kw =MV_U16ToSignedFloat(master2_p_raw);


    /* PCS4：Group2 Slave */
    pcs_state = (uint16_t)GET_INPUT(17362);

    PCS_ParseBranchStatus(pcs_state,&slave2->branch);

    slave2->rated_s_kva =PCS_GetRatedPowerByBranch(&slave2->branch,&slave2->run_branch_num);

    slave2->p_kw =MV_U16ToSignedFloat(slave2_p_raw);
}


/*
 * A/B共用Group最大无功能力：
 *
 * Group Qmax =
 *      可用Master Qmax + 可用Slave Qmax
 */
static float MV_Q_CalcGroupMax(
        PCS_Q_Info *master,
        PCS_Q_Info *slave,
        uint16_t *q_available_num)
{
    uint16_t master_ok;
    uint16_t slave_ok;

    float master_qmax = 0.0f;
    float slave_qmax  = 0.0f;

    if ((master == 0) ||
        (slave == 0)  ||
        (q_available_num == 0))
    {
        return 0.0f;
    }

    master_ok =
        PCS_Q_IsAvailable(
            master->rated_s_kva);

    slave_ok =
        PCS_Q_IsAvailable(
            slave->rated_s_kva);

    *q_available_num =
        master_ok + slave_ok;

    if (master_ok == 1u)
    {
        master_qmax =
            PCS_CalcQMax(
                master->p_kw,
                master->rated_s_kva);
    }

    if (slave_ok == 1u)
    {
        slave_qmax =
            PCS_CalcQMax(
                slave->p_kw,
                slave->rated_s_kva);
    }

    return master_qmax + slave_qmax;
}


/************************************************************
 * A/B差异：单PCS运行时Group Q设置值是否×2
 ************************************************************/

/*
 * single_pcs_double_enable = 1：
 *      方案A。仅1台PCS可用时，Group设置值 = target × 2；
 *      最大不超过 2 × 单台PCS Qmax。
 *
 * single_pcs_double_enable = 0：
 *      方案B。不进行×2。
 */
static float MV_Q_CalcGroupCmd(
        float group_q_target,
        float group_q_max,
        uint16_t available_num,
        uint16_t single_pcs_double_enable)
{
    float group_q_cmd;

    if (available_num == 0u)
    {
        return 0.0f;
    }

    if ((single_pcs_double_enable == 0u) ||
        (available_num >= 2u))
    {
        return MV_Q_Limit(
            group_q_target,
            group_q_max);
    }

    group_q_cmd =
        group_q_target * 2.0f;

    return MV_Q_Limit(
        group_q_cmd,
        group_q_max * 2.0f);
}


/************************************************************
 * A/B共用无功控制主体
 ************************************************************/

static void MV_ReactivePowerControl_Common(
        uint16_t total_q_cmd,
        uint16_t master1_p,
        uint16_t slave1_p,
        uint16_t master2_p,
        uint16_t slave2_p,
        uint16_t single_pcs_double_enable,
        MV_Q_Result *result)
{
    PCS_Q_Info master1 = {0};
    PCS_Q_Info slave1  = {0};
    PCS_Q_Info master2 = {0};
    PCS_Q_Info slave2  = {0};

    float group1_q_max;
    float group2_q_max;
    float balance_q_max;

    float total_q_float;
    float group_q_target;

    float group1_q_cmd;
    float group2_q_cmd;

    uint16_t available1;
    uint16_t available2;

    if (result == 0)
    {
        return;
    }

    MV_Q_ReadPCSInfo(
        master1_p,
        slave1_p,
        master2_p,
        slave2_p,
        &master1,
        &slave1,
        &master2,
        &slave2);

    group1_q_max = MV_Q_CalcGroupMax(&master1,&slave1,&available1);

    group2_q_max =MV_Q_CalcGroupMax(&master2,&slave2,&available2);

    result->group1_q_max =MV_PositiveFloatToU16(group1_q_max);

    result->group2_q_max =MV_PositiveFloatToU16(group2_q_max);

    result->group1_q_available_num = available1;
    result->group2_q_available_num = available2;

    result->group1_q_cmd = 0u;
    result->group2_q_cmd = 0u;
    result->balance_q_max = 0u;

    /*
     * 任意一个Group无可用PCS：
     * 为保持两个绕组平衡，两个Group全部不发无功。
     */
    if ((available1 == 0u) ||
        (available2 == 0u))
    {
        SET_INPUT(17000 + 300 * 0 + 31, 0u);
        SET_INPUT(17000 + 300 * 1 + 31, 0u);
        SET_INPUT(117, 0u);
        return;
    }

    if (group1_q_max < group2_q_max)
    {
        balance_q_max = group1_q_max;
    }
    else
    {
        balance_q_max = group2_q_max;
    }

    SET_INPUT(
        17000 + 300 * 0 + 31,
        MV_PositiveFloatToU16(balance_q_max));

    SET_INPUT(
        17000 + 300 * 1 + 31,
        MV_PositiveFloatToU16(balance_q_max));

    SET_INPUT(
        117,
        MV_PositiveFloatToU16(balance_q_max * 2.0f));

    result->balance_q_max =
        MV_PositiveFloatToU16(balance_q_max);

    /*
     * 用户总无功按int16_t补码解析，
     * 两个Group实际目标各承担一半。
     */
    total_q_float =
        MV_U16ToSignedFloat(total_q_cmd);

    group_q_target =
        total_q_float / 2.0f;

    group_q_target =
        MV_Q_Limit(
            group_q_target,
            balance_q_max);

    /*
     * 两个方案在这里才产生差异。
     */
    group1_q_cmd =
        MV_Q_CalcGroupCmd(
            group_q_target,
            group1_q_max,
            available1,
            single_pcs_double_enable);

    group2_q_cmd =
        MV_Q_CalcGroupCmd(
            group_q_target,
            group2_q_max,
            available2,
            single_pcs_double_enable);

    result->group1_q_cmd =
        MV_FloatToU16Signed(group1_q_cmd);

    result->group2_q_cmd =
        MV_FloatToU16Signed(group2_q_cmd);
}


/************************************************************
 * 方案A
 ************************************************************/

void MV_ReactivePowerControl_A(
        uint16_t total_q_cmd,
        uint16_t master1_p,
        uint16_t slave1_p,
        uint16_t master2_p,
        uint16_t slave2_p,
        MV_Q_Result *result)
{
    MV_ReactivePowerControl_Common(
        total_q_cmd,
        master1_p,
        slave1_p,
        master2_p,
        slave2_p,
        1u,
        result);
}

/*A方案，台达PCS一组PCS，任意一个故障，另外一个PCS无功*2*/
void MV_PQ_Control_A(
        uint16_t total_q_cmd,
        MV_Power_Distribution *p_result,
        MV_Q_Result *q_result)
{
    if ((p_result == 0) || (q_result == 0))
    {
        return;
    }

    MV_CalcPCSActivePower(p_result);

    MV_ReactivePowerControl_A(
        total_q_cmd,
        (uint16_t)p_result->pcs1_p_set,
        (uint16_t)p_result->pcs2_p_set,
        (uint16_t)p_result->pcs3_p_set,
        (uint16_t)p_result->pcs4_p_set,
        q_result);
}


/************************************************************
 * 方案B  台达PCS 一组group中 一个PCS故障，另外一个承担无功
 ************************************************************/

void MV_ReactivePowerControl_B(
        uint16_t total_q_cmd,
        uint16_t master1_p,
        uint16_t slave1_p,
        uint16_t master2_p,
        uint16_t slave2_p,
        MV_Q_Result *result)
{
    MV_ReactivePowerControl_Common(
        total_q_cmd,
        master1_p,
        slave1_p,
        master2_p,
        slave2_p,
        0u,
        result);
}


void MV_PQ_Control_B(
        uint16_t total_q_cmd,
        MV_Power_Distribution *p_result,
        MV_Q_Result *q_result)
{
    if ((p_result == 0) || (q_result == 0))
    {
        return;
    }

    MV_CalcPCSActivePower(p_result);

    MV_ReactivePowerControl_B(
        total_q_cmd,
        (uint16_t)p_result->pcs1_p_set,
        (uint16_t)p_result->pcs2_p_set,
        (uint16_t)p_result->pcs3_p_set,
        (uint16_t)p_result->pcs4_p_set,
        q_result);
}

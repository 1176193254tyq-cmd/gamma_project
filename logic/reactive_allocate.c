#include <stdint.h>
#include <math.h>

#include "main.h"

static float MV_U16ToSignedFloat(uint16_t raw)
{
    int16_t signed_value;

    signed_value = (int16_t)raw;

    return (float)signed_value;
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


    /********************************************************
     * 向0截断
     ********************************************************/
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


static float PCS_GetRatedPower(
        uint16_t reg1,
        uint16_t reg2,
        uint16_t *power_level)
{
    uint16_t status1;
    uint16_t status2;


    /********************************************************
     * 只保留bit0和bit1
     ********************************************************/
    status1 = reg1 & 0x0003u;
    status2 = reg2 & 0x0003u;


    /********************************************************
     * 第一优先级：
     *
     * 任意一个寄存器bit0和bit1同时为1
     *
     * 11
     *
     * PCS可用容量直接降为0
     ********************************************************/
    if ((status1 == 0x0003u) ||
        (status2 == 0x0003u))
    {
        *power_level =
            PCS_POWER_LEVEL_ZERO;

        return PCS_RATED_S_ZERO_KVA;
    }


    /********************************************************
     * 第二优先级：
     * PCS额定能力减半
     *
     * 2500 -> 1250
     ********************************************************/
    if ((status1 != 0u) ||
        (status2 != 0u))
    {
        *power_level =
            PCS_POWER_LEVEL_HALF;

        return PCS_RATED_S_HALF_KVA;
    }


    /********************************************************
     * 两个寄存器bit0、bit1全部为0
     *
     * 正常2500
     ********************************************************/
    *power_level =
        PCS_POWER_LEVEL_FULL;

    return PCS_RATED_S_NORMAL_KVA;
}


/************************************************************
 * 判断PCS当前是否允许参与无功控制
 *
 * 必须同时满足：
 *
 * 1. PCS Run = 1
 *
 * 2. PCS Fault = 0
 *
 * 5. PCS当前有效额定功率 > 0
 ************************************************************/
static uint16_t PCS_Q_IsAvailable(
        uint16_t run,
        uint16_t fault,
        uint16_t bms1_state,
        uint16_t bms2_state,
        float rated_s_kva)
{
    /********************************************************
     * PCS没有运行
     ********************************************************/
    if (run == 0u)
    {
        return 0u;
    }


    /********************************************************
     * PCS故障
     ********************************************************/
    if (fault != 0u)
    {
        return 0u;
    }


    // /********************************************************
    //  * BMS1状态=3
    //  ********************************************************/
    // if (bms1_state == BMS_STATE_STANDBY)
    // {
    //     //return 0u;
    // }


    // /********************************************************
    //  * BMS2状态=3
    //  ********************************************************/
    // if (bms2_state == BMS_STATE_STANDBY)
    // {
    //     //return 0u;
    // }


    /********************************************************
     * PCS降额状态已经变成0
     *
     * bit0和bit1同时置1
     ********************************************************/
    if (rated_s_kva <= 0.0f)
    {
        return 0u;
    }


    return 1u;
}


/************************************************************
 * 计算单台PCS当前最大允许无功
 *
 *
 *        S² = P² + Q²
 *
 *
 *        Qmax = sqrt(S² - P²)
 *
 ************************************************************/
static float PCS_CalcQMax(
        float p_kw,
        float rated_s_kva)
{
    float p_abs;


    /********************************************************
     * 有功取绝对值
     *
     * 充电和放电都占用视在功率容量
     ********************************************************/
    if (p_kw < 0.0f)
    {
        p_abs = -p_kw;
    }
    else
    {
        p_abs = p_kw;
    }


    /********************************************************
     * 防止rated_s_kva为0
     ********************************************************/
    if (rated_s_kva <= 0.0f)
    {
        return 0.0f;
    }


    /********************************************************
     * 当前有功已经达到或者超过
     * 当前有效额定视在功率
     *
     * 没有剩余无功能力
     ********************************************************/
    if (p_abs >= rated_s_kva)
    {
        return 0.0f;
    }


    return sqrtf(
            rated_s_kva * rated_s_kva
            -
            p_abs * p_abs);
}


/************************************************************
 * 无功正负限幅
 ************************************************************/
static float MV_Q_Limit(
        float q_cmd,
        float q_max)
{
    if (q_max <= 0.0f)
    {
        return 0.0f;
    }


    /********************************************************
     * 正向限幅
     ********************************************************/
    if (q_cmd > q_max)
    {
        q_cmd = q_max;
    }


    /********************************************************
     * 负向限幅
     ********************************************************/
    if (q_cmd < -q_max)
    {
        q_cmd = -q_max;
    }


    return q_cmd;
}


/************************************************************
 * 读取4台PCS所有无功相关信息
 *
 *
 * PCS对应关系：
 *
 * Group1：
 *
 *      Master = PCS1
 *      Slave  = PCS2
 *
 *
 * Group2：
 *
 *      Master = PCS3
 *      Slave  = PCS4
 *
 ************************************************************/
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
    uint16_t reg1;
    uint16_t reg2;

    uint16_t pcs_state;


    /********************************************************
     ********************************************************
     *
     * PCS1
     *
     * Group1 Master
     *
     ********************************************************
     ********************************************************/

    pcs_state =
        (uint16_t)GET_INPUT(
            17000 + 60);


    master1->run =
        (uint16_t)(
            (pcs_state >> 3) & 0x1u);


    master1->fault =
        (uint16_t)(
            (pcs_state >> 4) & 0x1u);


    /********************************************************
     * PCS1有功
     ********************************************************/
    master1->p_kw =
        MV_U16ToSignedFloat(
            master1_p_raw);


    /********************************************************
     * PCS1对应：
     *
     * BMS1
     * BMS2
     ********************************************************/
    master1->bms1_state =
        (uint16_t)GET_INPUT(
            28000 + 71);


    master1->bms2_state =
        (uint16_t)GET_INPUT(
            28000 + 200 + 71);


    /********************************************************
     * PCS1降额状态寄存器
     *
     * GET_INPUT(17000+69)
     * GET_INPUT(17000+72)
     ********************************************************/
    reg1 =
        (uint16_t)GET_INPUT(
            17000 + 69);


    reg2 =
        (uint16_t)GET_INPUT(
            17000 + 72);


    master1->rated_s_kva =
        PCS_GetRatedPower(
            reg1,
            reg2,
            &master1->power_level);



    /********************************************************
     ********************************************************
     *
     * PCS2
     *
     * Group1 Slave
     *
     ********************************************************
     ********************************************************/

    pcs_state =
        (uint16_t)GET_INPUT(
            17000 + 62);


    slave1->run =
        (uint16_t)(
            (pcs_state >> 2) & 0x1u);


    slave1->fault =
        (uint16_t)(
            (pcs_state >> 1) & 0x1u);


    /********************************************************
     * PCS2有功
     ********************************************************/
    slave1->p_kw =
        MV_U16ToSignedFloat(
            slave1_p_raw);


    /********************************************************
     * PCS2对应：
     *
     * BMS3
     * BMS4
     ********************************************************/
    slave1->bms1_state =
        (uint16_t)GET_INPUT(
            28000 + 400 + 71);


    slave1->bms2_state =
        (uint16_t)GET_INPUT(
            28000 + 600 + 71);


    /********************************************************
     * PCS2降额状态
     *
     * GET_INPUT(2600+300+203)
     * GET_INPUT(2600+300+211)
     ********************************************************/
    reg1 =
        (uint16_t)GET_INPUT(
            2600 + 300 + 203);


    reg2 =
        (uint16_t)GET_INPUT(
            2600 + 300 + 211);


    slave1->rated_s_kva =
        PCS_GetRatedPower(
            reg1,
            reg2,
            &slave1->power_level);



    /********************************************************
     ********************************************************
     *
     * PCS3
     *
     * Group2 Master
     *
     ********************************************************
     ********************************************************/

    pcs_state =
        (uint16_t)GET_INPUT(
            17000 + 300 + 60);


    master2->run =
        (uint16_t)(
            (pcs_state >> 3) & 0x1u);


    master2->fault =
        (uint16_t)(
            (pcs_state >> 4) & 0x1u);


    /********************************************************
     * PCS3有功
     ********************************************************/
    master2->p_kw =
        MV_U16ToSignedFloat(
            master2_p_raw);


    /********************************************************
     * PCS3对应：
     *
     * BMS5
     * BMS6
     ********************************************************/
    master2->bms1_state =
        (uint16_t)GET_INPUT(
            28000 + 800 + 71);


    master2->bms2_state =
        (uint16_t)GET_INPUT(
            28000 + 1000 + 71);


    /********************************************************
     * PCS3降额状态
     *
     * GET_INPUT(17000+69+300)
     * GET_INPUT(17000+72+300)
     ********************************************************/
    reg1 =
        (uint16_t)GET_INPUT(
            17000 + 69 + 300);


    reg2 =
        (uint16_t)GET_INPUT(
            17000 + 72 + 300);


    master2->rated_s_kva =
        PCS_GetRatedPower(
            reg1,
            reg2,
            &master2->power_level);



    /********************************************************
     ********************************************************
     *
     * PCS4
     *
     * Group2 Slave
     *
     ********************************************************
     ********************************************************/

    pcs_state =
        (uint16_t)GET_INPUT(
            17000 + 300 + 62);


    slave2->run =
        (uint16_t)(
            (pcs_state >> 2) & 0x1u);


    slave2->fault =
        (uint16_t)(
            (pcs_state >> 1) & 0x1u);


    /********************************************************
     * PCS4有功
     ********************************************************/
    slave2->p_kw =
        MV_U16ToSignedFloat(
            slave2_p_raw);


    /********************************************************
     * PCS4对应：
     *
     * BMS7
     * BMS8
     ********************************************************/
    slave2->bms1_state =
        (uint16_t)GET_INPUT(
            28000 + 1200 + 71);


    slave2->bms2_state =
        (uint16_t)GET_INPUT(
            28000 + 1400 + 71);


    /********************************************************
     * PCS4降额状态
     *
     * GET_INPUT(2600+900+203)
     * GET_INPUT(2600+900+211)
     ********************************************************/
    reg1 =
        (uint16_t)GET_INPUT(
            2600 + 900 + 203);


    reg2 =
        (uint16_t)GET_INPUT(
            2600 + 900 + 211);


    slave2->rated_s_kva =
        PCS_GetRatedPower(
            reg1,
            reg2,
            &slave2->power_level);
}


/************************************************************
 *
 *
 * Group最大无功能力 =
 *
 * 所有当前可用PCS的之和
 ************************************************************/
static float MV_Q_CalcGroupMax_A(
        PCS_Q_Info *master,
        PCS_Q_Info *slave,
        uint16_t *q_available_num)
{
    uint16_t master_ok;
    uint16_t slave_ok;

    float master_qmax = 0.0f;
    float slave_qmax  = 0.0f;


    /********************************************************
     * Master是否允许参与无功
     ********************************************************/
    master_ok =
        PCS_Q_IsAvailable(
            master->run,
            master->fault,
            master->bms1_state,
            master->bms2_state,
            master->rated_s_kva);


    /********************************************************
     * Slave是否允许参与无功
     ********************************************************/
    slave_ok =
        PCS_Q_IsAvailable(
            slave->run,
            slave->fault,
            slave->bms1_state,
            slave->bms2_state,
            slave->rated_s_kva);


    /********************************************************
     * 当前真正可以参与无功的PCS数量
     ********************************************************/
    *q_available_num =
        master_ok + slave_ok;


    /********************************************************
     * Master可以参与无功
     ********************************************************/
    if (master_ok == 1u)
    {
        master_qmax =
            PCS_CalcQMax(
                master->p_kw,
                master->rated_s_kva);
    }


    /********************************************************
     * Slave可以参与无功
     ********************************************************/
    if (slave_ok == 1u)
    {
        slave_qmax =
            PCS_CalcQMax(
                slave->p_kw,
                slave->rated_s_kva);
    }


    /********************************************************
     * 目标A：
     *
     * 最大化利用当前所有PCS无功能力
     ********************************************************/
    return master_qmax + slave_qmax;
}



void MV_ReactivePowerControl_A(
        uint16_t total_q_cmd,

        uint16_t master1_p,
        uint16_t slave1_p,

        uint16_t master2_p,
        uint16_t slave2_p,

        MV_Q_Result *result)
{
    PCS_Q_Info master1;
    PCS_Q_Info slave1;

    PCS_Q_Info master2;
    PCS_Q_Info slave2;

    float group1_q_max;
    float group2_q_max;

    float balance_q_max;

    float total_q_float;
    float group_q_target;

    uint16_t available1;
    uint16_t available2;


    /********************************************************
     * 读取4台PCS全部信息
     ********************************************************/
    MV_Q_ReadPCSInfo(
        master1_p,
        slave1_p,

        master2_p,
        slave2_p,

        &master1,
        &slave1,

        &master2,
        &slave2);


    /********************************************************
     * 计算Group1最大无功能力
     ********************************************************/
    group1_q_max =
        MV_Q_CalcGroupMax_A(
            &master1,
            &slave1,
            &available1);


    /********************************************************
     * 计算Group2最大无功能力
     ********************************************************/
    group2_q_max =
        MV_Q_CalcGroupMax_A(
            &master2,
            &slave2,
            &available2);


    /********************************************************
     * 输出Group能力
     ********************************************************/
    result->group1_q_max =
        MV_PositiveFloatToU16(
            group1_q_max);


    result->group2_q_max =
        MV_PositiveFloatToU16(
            group2_q_max);


    result->group1_q_available_num =
        available1;


    result->group2_q_available_num =
        available2;


    /********************************************************
     * 默认无功输出0
     ********************************************************/
    result->group1_q_cmd = 0u;
    result->group2_q_cmd = 0u;

    result->balance_q_max = 0u;


    /********************************************************
     * 任意一个Group没有可用PCS
     *
     * 两个绕组全部不发无功
     ********************************************************/
    if ((available1 == 0u) ||
        (available2 == 0u))
    {
        SET_INPUT(17000+300*0+31,0);
        SET_INPUT(17000+300*1+31,0);
         SET_INPUT(117,0);
        return;
    }


    /********************************************************
     * 两个绕组共同能力
     *
     * 必须取较小的一边
     ********************************************************/
    if (group1_q_max < group2_q_max)
    {
        balance_q_max =
            group1_q_max;

    }
    else
    {
        balance_q_max =
            group2_q_max;
    }
     SET_INPUT(17000+300*0+31,balance_q_max);
     SET_INPUT(17000+300*1+31,balance_q_max);
     SET_INPUT(117,(balance_q_max)*2);

    result->balance_q_max =
        MV_PositiveFloatToU16(
            balance_q_max);


    /********************************************************
     * 用户总无功
     *
     * uint16_t按照int16_t补码解析
     ********************************************************/
    total_q_float =
        MV_U16ToSignedFloat(
            total_q_cmd);


    /********************************************************
     * 两个绕组平均分配
     ********************************************************/
    group_q_target =
        total_q_float / 2.0f;


    /********************************************************
     * 根据两个绕组共同能力限幅
     ********************************************************/
    group_q_target =
        MV_Q_Limit(
            group_q_target,
            balance_q_max);


    /********************************************************
     * 最终转换为uint16_t
     ********************************************************/
    result->group1_q_cmd =
        MV_FloatToU16Signed(
            group_q_target);


    /********************************************************
     * 两绕组指令严格相同
     ********************************************************/
    result->group2_q_cmd =
        result->group1_q_cmd;

}



/*
 * File: MCU_AI.c
 *
 * Code generated for Simulink model 'MCU_AI'.
 *
 * Model version                  : 16.9
 * Simulink Coder version         : 25.2 (R2025b) 28-Jul-2025
 * C/C++ source code generated on : Mon Mar 16 15:27:59 2026
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Generic->32-bit Embedded Processor
 * Emulation hardware selection:
 *    Differs from embedded hardware (MATLAB Host)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "MCU_AI.h"
#include "rtwtypes.h"
#include <math.h>
#include "global_mcu_ai.h"
#include "dsp_rt.h"  /* DSP System Toolbox general run time support functions */
#include "dspsrt_rt.h"         /* DSP System Toolbox run time support library */
#include "dsp_rt.h"  /* DSP System Toolbox general run time support functions */

/* Block signals (default storage) */
BlockIO_MCU_AI MCU_AI_B;

/* Real-time model */
static RT_MODEL_MCU_AI MCU_AI_M_;
RT_MODEL_MCU_AI *const MCU_AI_M = &MCU_AI_M_;
extern real32_T rt_roundf_snf(real32_T u);
static void MCU_A_SwitchCaseActionSubsystem(real32_T rtu_In1, real32_T rtu_In2,
    real32_T *rty_Out1, real32_T *rty_Out2);

/*
 * Output and update for action system:
 *    '<S59>/Switch Case Action Subsystem'
 *    '<S60>/Switch Case Action Subsystem'
 *    '<S61>/Switch Case Action Subsystem'
 *    '<S62>/Switch Case Action Subsystem'
 *    '<S63>/Switch Case Action Subsystem'
 *    '<S64>/Switch Case Action Subsystem'
 *    '<S65>/Switch Case Action Subsystem'
 *    '<S66>/Switch Case Action Subsystem'
 *    '<S297>/Switch Case Action Subsystem'
 *    '<S298>/Switch Case Action Subsystem'
 *    ...
 */
static void MCU_A_SwitchCaseActionSubsystem(real32_T rtu_In1, real32_T rtu_In2,
    real32_T *rty_Out1, real32_T *rty_Out2)
{
    /* SignalConversion generated from: '<S76>/In1' */
    *rty_Out1 = rtu_In1;

    /* SignalConversion generated from: '<S76>/In2' */
    *rty_Out2 = rtu_In2;
}

real32_T rt_roundf_snf(real32_T u)
{
    real32_T y;
    if (fabsf(u) < 8.388608E+6F)
    {
        if (u >= 0.5F)
        {
            y = floorf(u + 0.5F);
        }
        else if (u > -0.5F)
        {
            y = u * 0.0F;
        }
        else
        {
            y = ceilf(u - 0.5F);
        }
    }
    else
    {
        y = u;
    }

    return y;
}

/* Model step function */
void MCU_AI_step(void)
{
    int32_T tmp;
    int32_T tmp_0;
    int32_T tmp_1;
    int32_T tmp_2;
    int32_T tmp_3;
    int32_T tmp_4;
    int32_T tmp_5;
    int32_T tmp_6;
    int32_T tmp_7;
    int32_T tmp_8;
    int32_T tmp_9;
    int16_T rtb_Switch15_e;
    int16_T rtb_Switch5_f;
    uint16_T rtb_Max;
    uint16_T rtb_Min_ju;
    boolean_T rtb_Compare;
    boolean_T rtb_Compare_bq;
    boolean_T rtb_Compare_ge;
    boolean_T rtb_Compare_h2;
    boolean_T rtb_Compare_hr;
    boolean_T rtb_Compare_j;
    boolean_T rtb_Compare_kvd;
    boolean_T rtb_Compare_lc;
    boolean_T rtb_Compare_m0;
    boolean_T rtb_Compare_mi;
    boolean_T rtb_Compare_ps;

    /* MinMax: '<S1>/Min' incorporates:
     *  Inport: '<Root>/target_h_soc'
     *  Inport: '<Root>/target_l_soc'
     */
    if (target_h_soc <= target_l_soc)
    {
        rtb_Min_ju = target_h_soc;
    }
    else
    {
        rtb_Min_ju = target_l_soc;
    }

    /* End of MinMax: '<S1>/Min' */

    /* Switch: '<S1>/Switch4' incorporates:
     *  Constant: '<S1>/Constant16'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs1_rated_power'
     */
    if (bus2_or_bus1 >= 0)
    {
        MCU_AI_B.Switch4 = pcs1_rated_power;
    }
    else
    {
        MCU_AI_B.Switch4 = 0U;
    }

    /* End of Switch: '<S1>/Switch4' */

    /* MinMax: '<S1>/Max' incorporates:
     *  Inport: '<Root>/target_h_soc'
     *  Inport: '<Root>/target_l_soc'
     */
    if (target_h_soc >= target_l_soc)
    {
        rtb_Max = target_h_soc;
    }
    else
    {
        rtb_Max = target_l_soc;
    }

    /* End of MinMax: '<S1>/Max' */

    /* Switch: '<S11>/Switch12' incorporates:
     *  Constant: '<S11>/Constant37'
     *  Inport: '<Root>/bms1_max_charge_power'
     *  Inport: '<Root>/bms1_soc'
     *  RelationalOperator: '<S11>/GreaterThanOrEqual'
     */
    if (bms1_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms1_max_charge_power;
    }

    /* MinMax: '<S11>/Min' incorporates:
     *  MinMax: '<S12>/Min'
     *  MinMax: '<S19>/Min4'
     *  MinMax: '<S20>/Min4'
     *  Product: '<S1>/Divide'
     *  Switch: '<S11>/Switch12'
     */
    MCU_AI_B.Switch9_f = (real32_T)((int32_T)MCU_AI_B.Switch4 >> 1);
    MCU_AI_B.Min = fminf(MCU_AI_B.Switch9_f, (real32_T)MCU_AI_B.i);

    /* Switch: '<S12>/Switch12' incorporates:
     *  Constant: '<S12>/Constant37'
     *  Inport: '<Root>/bms2_max_charge_power'
     *  Inport: '<Root>/bms2_soc'
     *  RelationalOperator: '<S12>/GreaterThanOrEqual'
     */
    if (bms2_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms2_max_charge_power;
    }

    /* MinMax: '<S12>/Min' incorporates:
     *  Switch: '<S12>/Switch12'
     */
    MCU_AI_B.Min_k = fminf(MCU_AI_B.Switch9_f, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S3>/Compare' incorporates:
     *  Constant: '<S3>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_ps = (module_num >= 2);

    /* Switch: '<S1>/Switch3' incorporates:
     *  Constant: '<S1>/Constant17'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs2_rated_power'
     */
    if (bus2_or_bus1 >=0)
    {
        MCU_AI_B.Switch3 = pcs2_rated_power;
    }
    else
    {
        MCU_AI_B.Switch3 = 0U;
    }

    /* End of Switch: '<S1>/Switch3' */

    /* Switch: '<S13>/Switch12' incorporates:
     *  Constant: '<S13>/Constant37'
     *  Inport: '<Root>/bms3_max_charge_power'
     *  Inport: '<Root>/bms3_soc'
     *  RelationalOperator: '<S13>/GreaterThanOrEqual'
     */
    if (bms3_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms3_max_charge_power;
    }

    /* MinMax: '<S13>/Min' incorporates:
     *  MinMax: '<S14>/Min'
     *  MinMax: '<S21>/Min4'
     *  MinMax: '<S22>/Min4'
     *  Product: '<S1>/Divide2'
     *  Switch: '<S13>/Switch12'
     */
    MCU_AI_B.Switch28 = (real32_T)((int32_T)MCU_AI_B.Switch3 >> 1);
    MCU_AI_B.Min_d = fminf(MCU_AI_B.Switch28, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S4>/Compare' incorporates:
     *  Constant: '<S4>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_m0 = (module_num >= 3);

    /* Switch: '<S14>/Switch12' incorporates:
     *  Constant: '<S14>/Constant37'
     *  Inport: '<Root>/bms4_max_charge_power'
     *  Inport: '<Root>/bms4_soc'
     *  RelationalOperator: '<S14>/GreaterThanOrEqual'
     */
    if (bms4_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms4_max_charge_power;
    }

    /* MinMax: '<S14>/Min' incorporates:
     *  Switch: '<S14>/Switch12'
     */
    MCU_AI_B.Min_h = fminf(MCU_AI_B.Switch28, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S5>/Compare' incorporates:
     *  Constant: '<S5>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_kvd = (module_num >= 4);

    /* Switch: '<S1>/Switch2' incorporates:
     *  Constant: '<S1>/Constant18'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs3_rated_power'
     */
    if (bus2_or_bus1 >= 0)
    {
        MCU_AI_B.Switch2 = pcs3_rated_power;
    }
    else
    {
        MCU_AI_B.Switch2 = 0U;
    }

    /* End of Switch: '<S1>/Switch2' */

    /* Switch: '<S15>/Switch12' incorporates:
     *  Constant: '<S15>/Constant37'
     *  Inport: '<Root>/bms5_max_charge_power'
     *  Inport: '<Root>/bms5_soc'
     *  RelationalOperator: '<S15>/GreaterThanOrEqual'
     */
    if (bms5_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms5_max_charge_power;
    }

    /* MinMax: '<S15>/Min' incorporates:
     *  MinMax: '<S16>/Min'
     *  MinMax: '<S23>/Min4'
     *  MinMax: '<S24>/Min4'
     *  Product: '<S1>/Divide8'
     *  Switch: '<S15>/Switch12'
     */
    MCU_AI_B.Switch17_b = (real32_T)((int32_T)MCU_AI_B.Switch2 >> 1);
    MCU_AI_B.Min_h1 = fminf(MCU_AI_B.Switch17_b, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S6>/Compare' incorporates:
     *  Constant: '<S6>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_ge = (module_num >= 5);

    /* Switch: '<S16>/Switch12' incorporates:
     *  Constant: '<S16>/Constant37'
     *  Inport: '<Root>/bms6_max_charge_power'
     *  Inport: '<Root>/bms6_soc'
     *  RelationalOperator: '<S16>/GreaterThanOrEqual'
     */
    if (bms6_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms6_max_charge_power;
    }

    /* MinMax: '<S16>/Min' incorporates:
     *  Switch: '<S16>/Switch12'
     */
    MCU_AI_B.Min_kv = fminf(MCU_AI_B.Switch17_b, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S7>/Compare' incorporates:
     *  Constant: '<S7>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_hr = (module_num >= 6);

    /* Switch: '<S1>/Switch1' incorporates:
     *  Constant: '<S1>/Constant19'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs4_rated_power'
     */
    if (bus2_or_bus1 >=0)
    {
        MCU_AI_B.Switch1 = pcs4_rated_power;
    }
    else
    {
        MCU_AI_B.Switch1 = 0U;
    }

    /* End of Switch: '<S1>/Switch1' */

    /* Switch: '<S17>/Switch12' incorporates:
     *  Constant: '<S17>/Constant37'
     *  Inport: '<Root>/bms7_max_charge_power'
     *  Inport: '<Root>/bms7_soc'
     *  RelationalOperator: '<S17>/GreaterThanOrEqual'
     */
    if (bms7_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms7_max_charge_power;
    }

    /* MinMax: '<S17>/Min' incorporates:
     *  MinMax: '<S18>/Min'
     *  MinMax: '<S25>/Min4'
     *  MinMax: '<S26>/Min4'
     *  Product: '<S1>/Divide12'
     *  Switch: '<S17>/Switch12'
     */
    MCU_AI_B.Abs7 = (real32_T)((int32_T)MCU_AI_B.Switch1 >> 1);
    MCU_AI_B.Min_l = fminf(MCU_AI_B.Abs7, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S8>/Compare' incorporates:
     *  Constant: '<S8>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare = (module_num >= 7);

    /* Switch: '<S18>/Switch12' incorporates:
     *  Constant: '<S18>/Constant37'
     *  Inport: '<Root>/bms8_max_charge_power'
     *  Inport: '<Root>/bms8_soc'
     *  RelationalOperator: '<S18>/GreaterThanOrEqual'
     */
    if (bms8_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms8_max_charge_power;
    }

    /* MinMax: '<S18>/Min' incorporates:
     *  Switch: '<S18>/Switch12'
     */
    MCU_AI_B.Min_m = fminf(MCU_AI_B.Abs7, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S9>/Compare' incorporates:
     *  Constant: '<S9>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_j = (module_num >= 8);

    /* Switch: '<S1>/Switch6' incorporates:
     *  Constant: '<S1>/Constant20'
     *  Constant: '<S1>/Constant21'
     *  DataTypeConversion: '<S1>/Data Type Conversion4'
     *  DataTypeConversion: '<S1>/Data Type Conversion5'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/mv_max_power'
     *  Inport: '<Root>/mv_power'
     *  Switch: '<S1>/Switch5'
     */
    if (bus2_or_bus1 > 0)
    {
        MCU_AI_B.DataTypeConversion9 = mv_max_power;
        MCU_AI_B.Switch12_e = mv_power;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
        MCU_AI_B.Switch12_e = 0.0F;
    }

    /* End of Switch: '<S1>/Switch6' */

    /* Switch: '<S19>/Switch5' incorporates:
     *  Constant: '<S19>/Constant7'
     *  Inport: '<Root>/bms1_max_discharge_power'
     *  Inport: '<Root>/bms1_soc'
     *  RelationalOperator: '<S19>/LessThanOrEqual'
     */
    if (bms1_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms1_max_discharge_power;
    }

    /* MinMax: '<S19>/Min4' incorporates:
     *  Switch: '<S19>/Switch5'
     */
    MCU_AI_B.Min4 = fminf(MCU_AI_B.Switch9_f, (real32_T)MCU_AI_B.i);

    /* Switch: '<S20>/Switch5' incorporates:
     *  Constant: '<S20>/Constant7'
     *  Inport: '<Root>/bms2_max_discharge_power'
     *  Inport: '<Root>/bms2_soc'
     *  RelationalOperator: '<S20>/LessThanOrEqual'
     */
    if (bms2_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms2_max_discharge_power;
    }

    /* MinMax: '<S20>/Min4' incorporates:
     *  Switch: '<S20>/Switch5'
     */
    MCU_AI_B.Min4_i = fminf(MCU_AI_B.Switch9_f, (real32_T)MCU_AI_B.i);

    /* Switch: '<S22>/Switch5' incorporates:
     *  Constant: '<S22>/Constant7'
     *  Inport: '<Root>/bms4_max_discharge_power'
     *  Inport: '<Root>/bms4_soc'
     *  RelationalOperator: '<S22>/LessThanOrEqual'
     */
    if (bms4_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms4_max_discharge_power;
    }

    /* MinMax: '<S22>/Min4' incorporates:
     *  Switch: '<S22>/Switch5'
     */
    MCU_AI_B.Min4_p = fminf(MCU_AI_B.Switch28, (real32_T)MCU_AI_B.i);

    /* Switch: '<S23>/Switch5' incorporates:
     *  Constant: '<S23>/Constant7'
     *  Inport: '<Root>/bms5_max_discharge_power'
     *  Inport: '<Root>/bms5_soc'
     *  RelationalOperator: '<S23>/LessThanOrEqual'
     */
    if (bms5_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms5_max_discharge_power;
    }

    /* MinMax: '<S23>/Min4' incorporates:
     *  Switch: '<S23>/Switch5'
     */
    MCU_AI_B.Min4_pm = fminf(MCU_AI_B.Switch17_b, (real32_T)MCU_AI_B.i);

    /* Switch: '<S25>/Switch5' incorporates:
     *  Constant: '<S25>/Constant7'
     *  Inport: '<Root>/bms7_max_discharge_power'
     *  Inport: '<Root>/bms7_soc'
     *  RelationalOperator: '<S25>/LessThanOrEqual'
     */
    if (bms7_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms7_max_discharge_power;
    }

    /* MinMax: '<S25>/Min4' incorporates:
     *  Switch: '<S25>/Switch5'
     */
    MCU_AI_B.Min4_f = fminf(MCU_AI_B.Abs7, (real32_T)MCU_AI_B.i);

    /* Switch: '<S26>/Switch5' incorporates:
     *  Constant: '<S26>/Constant7'
     *  Inport: '<Root>/bms8_max_discharge_power'
     *  Inport: '<Root>/bms8_soc'
     *  RelationalOperator: '<S26>/LessThanOrEqual'
     */
    if (bms8_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms8_max_discharge_power;
    }

    /* MinMax: '<S26>/Min4' incorporates:
     *  Switch: '<S26>/Switch5'
     */
    MCU_AI_B.Min4_l = fminf(MCU_AI_B.Abs7, (real32_T)MCU_AI_B.i);

    /* Switch: '<S27>/Switch3' incorporates:
     *  Constant: '<S27>/Constant42'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.i = (int32_T)MCU_AI_B.Min_k;
    }
    else
    {
        MCU_AI_B.i = 0;
    }

    /* Switch: '<S27>/Switch4' incorporates:
     *  Constant: '<S27>/Constant43'
     */
    if (rtb_Compare_m0)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min_d;
    }
    else
    {
        MCU_AI_B.DataTypeConversion15 = 0;
    }

    /* Switch: '<S27>/Switch5' incorporates:
     *  Constant: '<S27>/Constant44'
     */
    if (rtb_Compare_kvd)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min_h;
    }
    else
    {
        MCU_AI_B.DataTypeConversion4 = 0;
    }

    /* Switch: '<S27>/Switch6' incorporates:
     *  Constant: '<S27>/Constant1'
     */
    if (rtb_Compare_ge)
    {
        tmp = (int32_T)MCU_AI_B.Min_h1;
    }
    else
    {
        tmp = 0;
    }

    /* Switch: '<S27>/Switch7' incorporates:
     *  Constant: '<S27>/Constant2'
     */
    if (rtb_Compare_hr)
    {
        tmp_0 = (int32_T)MCU_AI_B.Min_kv;
    }
    else
    {
        tmp_0 = 0;
    }

    /* Switch: '<S27>/Switch8' incorporates:
     *  Constant: '<S27>/Constant3'
     */
    if (rtb_Compare)
    {
        tmp_1 = (int32_T)MCU_AI_B.Min_l;
    }
    else
    {
        tmp_1 = 0;
    }

    /* Switch: '<S27>/Switch9' incorporates:
     *  Constant: '<S27>/Constant4'
     */
    if (rtb_Compare_j)
    {
        tmp_2 = (int32_T)MCU_AI_B.Min_m;
    }
    else
    {
        tmp_2 = 0;
    }

    /* Switch: '<S27>/Switch' incorporates:
     *  Constant: '<S27>/Constant39'
     */
    if (rtb_Compare_ps)
    {
        tmp_3 = (int32_T)MCU_AI_B.Min4_i;
    }
    else
    {
        tmp_3 = 0;
    }

    /* Switch: '<S27>/Switch1' incorporates:
     *  Constant: '<S27>/Constant40'
     */
    if (rtb_Compare_m0)
    {
        tmp_4 = (int32_T)MCU_AI_B.Min_d;
    }
    else
    {
        tmp_4 = 0;
    }

    /* Switch: '<S27>/Switch2' incorporates:
     *  Constant: '<S27>/Constant41'
     */
    if (rtb_Compare_kvd)
    {
        tmp_5 = (int32_T)MCU_AI_B.Min4_p;
    }
    else
    {
        tmp_5 = 0;
    }

    /* Switch: '<S27>/Switch10' incorporates:
     *  Constant: '<S27>/Constant5'
     */
    if (rtb_Compare_ge)
    {
        tmp_6 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        tmp_6 = 0;
    }

    /* Switch: '<S27>/Switch11' incorporates:
     *  Constant: '<S27>/Constant6'
     */
    if (rtb_Compare_hr)
    {
        tmp_7 = (int32_T)MCU_AI_B.Min_kv;
    }
    else
    {
        tmp_7 = 0;
    }

    /* Switch: '<S27>/Switch12' incorporates:
     *  Constant: '<S27>/Constant7'
     */
    if (rtb_Compare)
    {
        tmp_8 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        tmp_8 = 0;
    }

    /* Switch: '<S27>/Switch13' incorporates:
     *  Constant: '<S27>/Constant8'
     */
    if (rtb_Compare_j)
    {
        tmp_9 = (int32_T)MCU_AI_B.Min4_l;
    }
    else
    {
        tmp_9 = 0;
    }

    /* MinMax: '<S27>/Max' incorporates:
     *  MinMax: '<S27>/Min'
     *  Product: '<S27>/Product'
     *  Product: '<S27>/Product1'
     *  Sum: '<S27>/Add1'
     *  Sum: '<S27>/Add2'
     *  Switch: '<S27>/Switch'
     *  Switch: '<S27>/Switch1'
     *  Switch: '<S27>/Switch10'
     *  Switch: '<S27>/Switch11'
     *  Switch: '<S27>/Switch12'
     *  Switch: '<S27>/Switch13'
     *  Switch: '<S27>/Switch2'
     *  Switch: '<S27>/Switch3'
     *  Switch: '<S27>/Switch4'
     *  Switch: '<S27>/Switch5'
     *  Switch: '<S27>/Switch6'
     *  Switch: '<S27>/Switch7'
     *  Switch: '<S27>/Switch8'
     *  Switch: '<S27>/Switch9'
     */
    MCU_AI_B.Product8_n = fmaxf(fmaxf(-(((((((MCU_AI_B.Min + (real32_T)
        MCU_AI_B.i) + (real32_T)MCU_AI_B.DataTypeConversion15) + (real32_T)
        MCU_AI_B.DataTypeConversion4) + (real32_T)tmp) + (real32_T)tmp_0) +
        (real32_T)tmp_1) + (real32_T)tmp_2), -MCU_AI_B.DataTypeConversion9),
        fminf(fminf(MCU_AI_B.Switch12_e, MCU_AI_B.DataTypeConversion9),
              ((((((MCU_AI_B.Min4 + (real32_T)tmp_3) + (real32_T)tmp_4) +
                  (real32_T)tmp_5) + (real32_T)tmp_6) + (real32_T)tmp_7) +
               (real32_T)tmp_8) + (real32_T)tmp_9));

    /* RelationalOperator: '<S10>/Compare' incorporates:
     *  Constant: '<S10>/Constant'
     */
    rtb_Compare_bq = (MCU_AI_B.Product8_n >= 0.0F);

    /* Switch: '<S32>/Switch12' incorporates:
     *  DataTypeConversion: '<S1>/Data Type Conversion10'
     *  Inport: '<Root>/bms1_soc'
     *  Sum: '<S32>/Subtract'
     *  Sum: '<S32>/Subtract4'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.Abs7 = (real32_T)bms1_soc - (real32_T)rtb_Min_ju;
    }
    else
    {
        MCU_AI_B.Abs7 = (real32_T)rtb_Max - (real32_T)bms1_soc;
    }

    /* End of Switch: '<S32>/Switch12' */

    /* Switch: '<S32>/Switch20' incorporates:
     *  Constant: '<S257>/Constant'
     *  Constant: '<S32>/Constant1'
     *  RelationalOperator: '<S257>/Compare'
     */
    if (MCU_AI_B.Abs7 <= 0.0F)
    {
        MCU_AI_B.Switch20 = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Abs7;
    }

    /* End of Switch: '<S32>/Switch20' */

    /* Switch: '<S32>/Switch13' incorporates:
     *  Constant: '<S32>/Constant52'
     */
    if (rtb_Compare_ps)
    {
        /* Switch: '<S32>/Switch9' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion11'
         *  Inport: '<Root>/bms2_soc'
         *  Sum: '<S32>/Subtract1'
         *  Sum: '<S32>/Subtract5'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms2_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms2_soc;
        }

        /* End of Switch: '<S32>/Switch9' */

        /* Switch: '<S32>/Switch17' incorporates:
         *  Constant: '<S258>/Constant'
         *  Constant: '<S32>/Constant2'
         *  RelationalOperator: '<S258>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.Switch13 = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch13 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch17' */
    }
    else
    {
        MCU_AI_B.Switch13 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch13' */

    /* Switch: '<S32>/Switch14' incorporates:
     *  Constant: '<S32>/Constant53'
     */
    if (rtb_Compare_m0)
    {
        /* Switch: '<S32>/Switch10' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion12'
         *  Inport: '<Root>/bms3_soc'
         *  Sum: '<S32>/Subtract2'
         *  Sum: '<S32>/Subtract6'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms3_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms3_soc;
        }

        /* End of Switch: '<S32>/Switch10' */

        /* Switch: '<S32>/Switch18' incorporates:
         *  Constant: '<S259>/Constant'
         *  Constant: '<S32>/Constant3'
         *  RelationalOperator: '<S259>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.Switch14 = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch18' */
    }
    else
    {
        MCU_AI_B.Switch14 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch14' */

    /* Switch: '<S32>/Switch15' incorporates:
     *  Constant: '<S32>/Constant54'
     */
    if (rtb_Compare_kvd)
    {
        /* Switch: '<S32>/Switch11' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion13'
         *  Inport: '<Root>/bms4_soc'
         *  Sum: '<S32>/Subtract3'
         *  Sum: '<S32>/Subtract7'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms4_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms4_soc;
        }

        /* End of Switch: '<S32>/Switch11' */

        /* Switch: '<S32>/Switch19' incorporates:
         *  Constant: '<S260>/Constant'
         *  Constant: '<S32>/Constant4'
         *  RelationalOperator: '<S260>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.Switch15 = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch15 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch19' */
    }
    else
    {
        MCU_AI_B.Switch15 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch15' */

    /* Switch: '<S32>/Switch5' incorporates:
     *  Constant: '<S32>/Constant5'
     */
    if (rtb_Compare_ge)
    {
        /* Switch: '<S32>/Switch3' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion1'
         *  Inport: '<Root>/bms5_soc'
         *  Sum: '<S32>/Subtract12'
         *  Sum: '<S32>/Subtract8'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms5_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms5_soc;
        }

        /* End of Switch: '<S32>/Switch3' */

        /* Switch: '<S32>/Switch24' incorporates:
         *  Constant: '<S261>/Constant'
         *  Constant: '<S32>/Constant10'
         *  RelationalOperator: '<S261>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.Switch5 = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch24' */
    }
    else
    {
        MCU_AI_B.Switch5 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch5' */

    /* Switch: '<S32>/Switch6' incorporates:
     *  Constant: '<S32>/Constant6'
     */
    if (rtb_Compare_hr)
    {
        /* Switch: '<S32>/Switch4' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion2'
         *  Inport: '<Root>/bms6_soc'
         *  Sum: '<S32>/Subtract13'
         *  Sum: '<S32>/Subtract9'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms6_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms6_soc;
        }

        /* End of Switch: '<S32>/Switch4' */

        /* Switch: '<S32>/Switch21' incorporates:
         *  Constant: '<S262>/Constant'
         *  Constant: '<S32>/Constant11'
         *  RelationalOperator: '<S262>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.Switch6 = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch6 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch21' */
    }
    else
    {
        MCU_AI_B.Switch6 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch6' */

    /* Switch: '<S32>/Switch7' incorporates:
     *  Constant: '<S32>/Constant7'
     */
    if (rtb_Compare)
    {
        /* Switch: '<S32>/Switch1' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion3'
         *  Inport: '<Root>/bms7_soc'
         *  Sum: '<S32>/Subtract10'
         *  Sum: '<S32>/Subtract14'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms7_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms7_soc;
        }

        /* End of Switch: '<S32>/Switch1' */

        /* Switch: '<S32>/Switch22' incorporates:
         *  Constant: '<S263>/Constant'
         *  Constant: '<S32>/Constant12'
         *  RelationalOperator: '<S263>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.Switch7 = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch7 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch22' */
    }
    else
    {
        MCU_AI_B.Switch7 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch7' */

    /* Switch: '<S32>/Switch16' incorporates:
     *  Constant: '<S32>/Constant9'
     */
    if (rtb_Compare_j)
    {
        /* Switch: '<S32>/Switch2' incorporates:
         *  DataTypeConversion: '<S1>/Data Type Conversion6'
         *  Inport: '<Root>/bms8_soc'
         *  Sum: '<S32>/Subtract11'
         *  Sum: '<S32>/Subtract15'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch9_f = (real32_T)bms8_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms8_soc;
        }

        /* End of Switch: '<S32>/Switch2' */

        /* Switch: '<S32>/Switch23' incorporates:
         *  Constant: '<S264>/Constant'
         *  Constant: '<S32>/Constant13'
         *  RelationalOperator: '<S264>/Compare'
         */
        if (MCU_AI_B.Switch9_f <= 0.0F)
        {
            MCU_AI_B.DataTypeConversion9 = 0.0F;
        }
        else
        {
            MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S32>/Switch23' */
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }

    /* End of Switch: '<S32>/Switch16' */

    /* SignalConversion generated from: '<S29>/Sort' */
    MCU_AI_B.TmpSignalConversionAtSelect[0] = MCU_AI_B.Switch20;

    /* Switch: '<S29>/Switch6' */
    if (rtb_Compare_ps)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[1] = MCU_AI_B.Switch13;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant45'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[1] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch6' */

    /* Switch: '<S29>/Switch7' */
    if (rtb_Compare_m0)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[2] = MCU_AI_B.Switch14;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant46'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[2] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch7' */

    /* Switch: '<S29>/Switch8' */
    if (rtb_Compare_kvd)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[3] = MCU_AI_B.Switch15;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant47'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[3] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch8' */

    /* Switch: '<S29>/Switch5' */
    if (rtb_Compare_ge)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[4] = MCU_AI_B.Switch5;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant5'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[4] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch5' */

    /* Switch: '<S29>/Switch9' */
    if (rtb_Compare_hr)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[5] = MCU_AI_B.Switch6;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant6'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[5] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch9' */

    /* Switch: '<S29>/Switch10' */
    if (rtb_Compare)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[6] = MCU_AI_B.Switch7;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant7'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[6] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch10' */

    /* Switch: '<S29>/Switch11' */
    if (rtb_Compare_j)
    {
        /* SignalConversion generated from: '<S29>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSelect[7] = MCU_AI_B.DataTypeConversion9;
    }
    else
    {
        /* SignalConversion generated from: '<S29>/Sort' incorporates:
         *  Constant: '<S29>/Constant8'
         */
        MCU_AI_B.TmpSignalConversionAtSelect[7] = -1.0F;
    }

    /* End of Switch: '<S29>/Switch11' */

    /* S-Function (sdspsrt2): '<S29>/Sort' */
    /* DSP System Toolbox Sort (sdspsrt2) - '<S29>/Sort' */
    {
        const real32_T *uQS = MCU_AI_B.TmpSignalConversionAtSelect;
        uint32_T *index = MCU_AI_B.Sort_o2;
        real32_T *y = MCU_AI_B.Sort_o1;
        uint32_T j_SORT;
        for (j_SORT=0; j_SORT<8; j_SORT++)
        {
            index[j_SORT] = j_SORT;
        }

        MWDSP_Sort_Qk_Idx_R(uQS, index, 0, 7);

        {
            uint32_T range = (8 >> 1);
            uint32_T k_SORT;
            for (k_SORT=0; k_SORT<range; k_SORT++)
            {
                uint32_T tmp_IDX = index[k_SORT];
                index[k_SORT] = index[7-k_SORT];
                index[7-k_SORT] = tmp_IDX;
            }
        }

        for (j_SORT=0; j_SORT<8; j_SORT++)
        {
            y[j_SORT] = uQS[index[j_SORT]];
            index[j_SORT] += 1;        /* Convert to MATLAB 1-based indexing */
        }
    }

    /* Switch: '<S28>/Switch36' incorporates:
     *  Constant: '<S227>/Constant25'
     *  Constant: '<S227>/Constant26'
     *  Constant: '<S28>/Constant38'
     *  Constant: '<S28>/Constant39'
     *  Switch: '<S227>/Switch19'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.Abs7 = 1.0F;
        MCU_AI_B.Switch9_f = 1.0F;
    }
    else
    {
        MCU_AI_B.Abs7 = -1.0F;
        MCU_AI_B.Switch9_f = -1.0F;
    }

    /* End of Switch: '<S28>/Switch36' */

    /* Sum: '<S227>/Add9' incorporates:
     *  Sum: '<S31>/Add12'
     *  Switch: '<S227>/Switch17'
     */
    MCU_AI_B.Switch2_kc = MCU_AI_B.Switch7 + MCU_AI_B.DataTypeConversion9;
    MCU_AI_B.Square1 = MCU_AI_B.Switch2_kc;

    /* Switch: '<S227>/Switch1' incorporates:
     *  Constant: '<S227>/Constant2'
     *  Constant: '<S250>/Constant'
     *  RelationalOperator: '<S250>/Compare'
     *  Sum: '<S227>/Add9'
     */
    if (MCU_AI_B.Switch2_kc <= 1.0F)
    {
        MCU_AI_B.Square1 = 1.0F;
    }

    /* End of Switch: '<S227>/Switch1' */

    /* Switch: '<S31>/Switch19' incorporates:
     *  Constant: '<S31>/Constant25'
     *  Constant: '<S31>/Constant26'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.Switch19 = 1.0F;
    }
    else
    {
        MCU_AI_B.Switch19 = -1.0F;
    }

    /* End of Switch: '<S31>/Switch19' */

    /* Sum: '<S32>/Add3' incorporates:
     *  Sum: '<S224>/Add9'
     *  Sum: '<S31>/Add9'
     *  Switch: '<S224>/Switch17'
     */
    MCU_AI_B.Switch12_e = MCU_AI_B.Switch20 + MCU_AI_B.Switch13;
    MCU_AI_B.Product7_d = (((((MCU_AI_B.Switch12_e + MCU_AI_B.Switch14) +
        MCU_AI_B.Switch15) + MCU_AI_B.Switch5) + MCU_AI_B.Switch6) +
                           MCU_AI_B.Switch7) + MCU_AI_B.DataTypeConversion9;

    /* Switch: '<S32>/Switch8' incorporates:
     *  Constant: '<S256>/Constant'
     *  Constant: '<S32>/Constant8'
     *  RelationalOperator: '<S256>/Compare'
     */
    if (MCU_AI_B.Product7_d <= 0.1F)
    {
        MCU_AI_B.Product7_d = 0.4F;
    }

    /* Product: '<S31>/Product8' incorporates:
     *  Constant: '<S31>/Constant22'
     *  Switch: '<S32>/Switch8'
     */
    MCU_AI_B.Product8 = MCU_AI_B.Product7_d * 0.5F;

    /* Switch: '<S31>/Switch17' incorporates:
     *  Constant: '<S213>/Constant'
     *  Constant: '<S31>/Constant55'
     *  RelationalOperator: '<S213>/Compare'
     */
    if (MCU_AI_B.Product8 <= 1.0F)
    {
        MCU_AI_B.Switch17 = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch17 = MCU_AI_B.Product8_n;
    }

    /* End of Switch: '<S31>/Switch17' */

    /* Switch: '<S227>/Switch17' incorporates:
     *  Constant: '<S227>/Constant55'
     *  Constant: '<S251>/Constant'
     *  RelationalOperator: '<S251>/Compare'
     */
    if (MCU_AI_B.Square1 <= 1.0F)
    {
        MCU_AI_B.Product7_d = 0.0F;
    }
    else
    {
        /* Product: '<S31>/Divide38' incorporates:
         *  Constant: '<S31>/Constant5'
         *  Product: '<S31>/Product14'
         */
        MCU_AI_B.Product7_d = MCU_AI_B.Switch2_kc * 0.5F * MCU_AI_B.Switch17 /
            MCU_AI_B.Product8;

        /* Sum: '<S1>/Add7' */
        MCU_AI_B.Switch2_kc = MCU_AI_B.Min4_f + MCU_AI_B.Min4_l;

        /* Switch: '<S231>/Switch2' incorporates:
         *  RelationalOperator: '<S231>/LowerRelop1'
         */
        if (!(MCU_AI_B.Product7_d > MCU_AI_B.Switch2_kc))
        {
            /* Product: '<S31>/Product4' incorporates:
             *  Sum: '<S1>/Add3'
             */
            MCU_AI_B.Switch2_kc = -(MCU_AI_B.Min_l + MCU_AI_B.Min_m);

            /* Switch: '<S231>/Switch' incorporates:
             *  RelationalOperator: '<S231>/UpperRelop'
             */
            if (!(MCU_AI_B.Product7_d < MCU_AI_B.Switch2_kc))
            {
                MCU_AI_B.Switch2_kc = MCU_AI_B.Product7_d;
            }

            /* End of Switch: '<S231>/Switch' */
        }

        /* End of Switch: '<S231>/Switch2' */

        /* Switch: '<S31>/Switch10' incorporates:
         *  Constant: '<S223>/Constant'
         *  Constant: '<S31>/Constant20'
         *  Product: '<S31>/Product13'
         *  RelationalOperator: '<S223>/Compare'
         */
        if (MCU_AI_B.Switch19 * MCU_AI_B.Switch2_kc < 0.0F)
        {
            MCU_AI_B.Product7_d = 0.0F;
        }
        else
        {
            MCU_AI_B.Product7_d = MCU_AI_B.Switch2_kc;
        }

        /* End of Switch: '<S31>/Switch10' */
    }

    /* Product: '<S227>/Divide36' */
    MCU_AI_B.DataTypeConversion9 = MCU_AI_B.DataTypeConversion9 *
        MCU_AI_B.Product7_d / MCU_AI_B.Square1;

    /* Switch: '<S255>/Switch2' incorporates:
     *  Product: '<S227>/Product2'
     *  RelationalOperator: '<S255>/LowerRelop1'
     *  RelationalOperator: '<S255>/UpperRelop'
     *  Switch: '<S255>/Switch'
     */
    if (MCU_AI_B.DataTypeConversion9 > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Min4_l;
    }
    else if (MCU_AI_B.DataTypeConversion9 < -MCU_AI_B.Min_m)
    {
        /* Switch: '<S255>/Switch' incorporates:
         *  Product: '<S227>/Product2'
         */
        MCU_AI_B.DataTypeConversion9 = -MCU_AI_B.Min_m;
    }

    /* End of Switch: '<S255>/Switch2' */

    /* Switch: '<S227>/Switch12' incorporates:
     *  Constant: '<S227>/Constant18'
     *  Constant: '<S253>/Constant'
     *  Product: '<S227>/Product11'
     *  RelationalOperator: '<S253>/Compare'
     */
    if (MCU_AI_B.Switch9_f * MCU_AI_B.DataTypeConversion9 < 0.0F)
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }

    /* End of Switch: '<S227>/Switch12' */

    /* Switch: '<S31>/Switch8' incorporates:
     *  Abs: '<S31>/Abs7'
     *  Constant: '<S219>/Constant'
     *  RelationalOperator: '<S219>/Compare'
     */
    if (fabsf(MCU_AI_B.DataTypeConversion9) <= 0.1F)
    {
        /* DataTypeConversion: '<S31>/Data Type Conversion15' */
        MCU_AI_B.DataTypeConversion15 = 0;
    }
    else
    {
        /* DataTypeConversion: '<S31>/Data Type Conversion15' incorporates:
         *  DataTypeConversion: '<S31>/Data Type Conversion14'
         */
        MCU_AI_B.DataTypeConversion15 = (int16_T)MCU_AI_B.DataTypeConversion9;
    }

    /* End of Switch: '<S31>/Switch8' */

    /* Switch: '<S28>/Switch12' incorporates:
     *  Constant: '<S224>/Constant25'
     *  Constant: '<S224>/Constant26'
     *  Product: '<S28>/Product10'
     *  Switch: '<S224>/Switch19'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Min4;
        MCU_AI_B.Switch2_kc = 1.0F;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = -MCU_AI_B.Min;
        MCU_AI_B.Switch2_kc = -1.0F;
    }

    /* End of Switch: '<S28>/Switch12' */

    /* Switch: '<S224>/Switch1' incorporates:
     *  Constant: '<S224>/Constant2'
     *  Constant: '<S232>/Constant'
     *  RelationalOperator: '<S232>/Compare'
     */
    if (MCU_AI_B.Switch12_e <= 1.0F)
    {
        MCU_AI_B.Switch12_e = 1.0F;

        /* Switch: '<S224>/Switch17' incorporates:
         *  Constant: '<S224>/Constant2'
         *  Constant: '<S224>/Constant55'
         */
        MCU_AI_B.Divide35_o3 = 0.0F;
    }
    else
    {
        /* Product: '<S31>/Divide35' incorporates:
         *  Constant: '<S31>/Constant23'
         *  Product: '<S31>/Product6'
         *  Switch: '<S224>/Switch17'
         */
        MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch12_e * 0.5F * MCU_AI_B.Switch17 /
            MCU_AI_B.Product8;

        /* Sum: '<S1>/Add6' incorporates:
         *  Switch: '<S224>/Switch17'
         */
        MCU_AI_B.Switch2_apa = MCU_AI_B.Min4 + MCU_AI_B.Min4_i;

        /* Switch: '<S228>/Switch2' incorporates:
         *  RelationalOperator: '<S228>/LowerRelop1'
         *  Switch: '<S224>/Switch17'
         */
        if (!(MCU_AI_B.Divide35_o3 > MCU_AI_B.Switch2_apa))
        {
            /* Product: '<S31>/Product1' incorporates:
             *  Sum: '<S1>/Add2'
             */
            MCU_AI_B.Switch2_apa = -(MCU_AI_B.Min + MCU_AI_B.Min_k);

            /* Switch: '<S228>/Switch' incorporates:
             *  RelationalOperator: '<S228>/UpperRelop'
             */
            if (!(MCU_AI_B.Divide35_o3 < MCU_AI_B.Switch2_apa))
            {
                MCU_AI_B.Switch2_apa = MCU_AI_B.Divide35_o3;
            }

            /* End of Switch: '<S228>/Switch' */
        }

        /* End of Switch: '<S228>/Switch2' */

        /* Switch: '<S31>/Switch11' incorporates:
         *  Constant: '<S220>/Constant'
         *  Constant: '<S31>/Constant17'
         *  Product: '<S31>/Product10'
         *  RelationalOperator: '<S220>/Compare'
         *  Switch: '<S224>/Switch17'
         */
        if (MCU_AI_B.Switch19 * MCU_AI_B.Switch2_apa < 0.0F)
        {
            MCU_AI_B.Divide35_o3 = 0.0F;
        }
        else
        {
            MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch2_apa;
        }

        /* End of Switch: '<S31>/Switch11' */
    }

    /* End of Switch: '<S224>/Switch1' */

    /* Product: '<S224>/Divide35' */
    MCU_AI_B.Switch20 = MCU_AI_B.Switch20 * MCU_AI_B.Divide35_o3 /
        MCU_AI_B.Switch12_e;

    /* Switch: '<S236>/Switch2' incorporates:
     *  Product: '<S224>/Product1'
     *  RelationalOperator: '<S236>/LowerRelop1'
     *  RelationalOperator: '<S236>/UpperRelop'
     *  Switch: '<S236>/Switch'
     */
    if (MCU_AI_B.Switch20 > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4;
    }
    else if (MCU_AI_B.Switch20 < -MCU_AI_B.Min)
    {
        /* Switch: '<S236>/Switch' incorporates:
         *  Product: '<S224>/Product1'
         */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min;
    }

    /* End of Switch: '<S236>/Switch2' */

    /* Switch: '<S224>/Switch11' incorporates:
     *  Constant: '<S224>/Constant17'
     *  Constant: '<S234>/Constant'
     *  Product: '<S224>/Product10'
     *  RelationalOperator: '<S234>/Compare'
     */
    if (MCU_AI_B.Switch2_kc * MCU_AI_B.Switch20 < 0.0F)
    {
        MCU_AI_B.Switch20 = 0.0F;
    }

    /* End of Switch: '<S224>/Switch11' */

    /* Switch: '<S31>/Switch1' incorporates:
     *  Abs: '<S31>/Abs'
     *  Constant: '<S215>/Constant'
     *  Constant: '<S31>/Constant8'
     *  DataTypeConversion: '<S31>/Data Type Conversion11'
     *  RelationalOperator: '<S215>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch20) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Switch20;
    }

    /* End of Switch: '<S31>/Switch1' */

    /* DataTypeConversion: '<S31>/Data Type Conversion' */
    MCU_AI_B.Switch20 = rtb_Switch15_e;

    /* Sum: '<S28>/Subtract1' incorporates:
     *  DataTypeConversion: '<S31>/Data Type Conversion'
     */
    MCU_AI_B.Switch2_apa = MCU_AI_B.DataTypeConversion9 - (real32_T)
        rtb_Switch15_e;

    /* Product: '<S224>/Divide36' */
    MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Switch13 * MCU_AI_B.Divide35_o3 /
        MCU_AI_B.Switch12_e;

    /* Switch: '<S237>/Switch2' incorporates:
     *  Product: '<S224>/Product2'
     *  RelationalOperator: '<S237>/LowerRelop1'
     *  RelationalOperator: '<S237>/UpperRelop'
     *  Switch: '<S237>/Switch'
     */
    if (MCU_AI_B.DataTypeConversion9 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Min4_i;
    }
    else if (MCU_AI_B.DataTypeConversion9 < -MCU_AI_B.Min_k)
    {
        /* Switch: '<S237>/Switch' incorporates:
         *  Product: '<S224>/Product2'
         */
        MCU_AI_B.DataTypeConversion9 = -MCU_AI_B.Min_k;
    }

    /* End of Switch: '<S237>/Switch2' */

    /* Switch: '<S224>/Switch12' incorporates:
     *  Constant: '<S224>/Constant18'
     *  Constant: '<S235>/Constant'
     *  Product: '<S224>/Product11'
     *  RelationalOperator: '<S235>/Compare'
     */
    if (MCU_AI_B.Switch2_kc * MCU_AI_B.DataTypeConversion9 < 0.0F)
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }

    /* End of Switch: '<S224>/Switch12' */

    /* Switch: '<S31>/Switch2' incorporates:
     *  Abs: '<S31>/Abs1'
     *  Constant: '<S210>/Constant'
     *  Constant: '<S31>/Constant9'
     *  DataTypeConversion: '<S31>/Data Type Conversion1'
     *  RelationalOperator: '<S210>/Compare'
     */
    if (fabsf(MCU_AI_B.DataTypeConversion9) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.DataTypeConversion9;
    }

    /* End of Switch: '<S31>/Switch2' */

    /* DataTypeConversion: '<S31>/Data Type Conversion4' */
    MCU_AI_B.DataTypeConversion4 = rtb_Switch15_e;

    /* Switch: '<S28>/Switch27' incorporates:
     *  Constant: '<S28>/Constant54'
     *  DataTypeConversion: '<S31>/Data Type Conversion4'
     *  Sum: '<S28>/Subtract3'
     *  Switch: '<S28>/Switch9'
     */
    if (rtb_Compare_ps)
    {
        /* Switch: '<S28>/Switch9' incorporates:
         *  Product: '<S28>/Product11'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Min4_i;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_k;
        }

        MCU_AI_B.Switch27 = (real32_T)MCU_AI_B.i - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Switch27 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch27' */

    /* Switch: '<S21>/Switch5' incorporates:
     *  Constant: '<S21>/Constant7'
     *  Inport: '<Root>/bms3_max_discharge_power'
     *  Inport: '<Root>/bms3_soc'
     *  RelationalOperator: '<S21>/LessThanOrEqual'
     */
    if (bms3_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms3_max_discharge_power;
    }

    /* MinMax: '<S21>/Min4' incorporates:
     *  Switch: '<S21>/Switch5'
     */
    MCU_AI_B.Switch13 = fminf(MCU_AI_B.Switch28, (real32_T)MCU_AI_B.i);

    /* Switch: '<S225>/Switch19' incorporates:
     *  Constant: '<S225>/Constant25'
     *  Constant: '<S225>/Constant26'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.DataTypeConversion9 = 1.0F;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = -1.0F;
    }

    /* End of Switch: '<S225>/Switch19' */

    /* Sum: '<S225>/Add9' */
    MCU_AI_B.Switch2_kc = MCU_AI_B.Switch14 + MCU_AI_B.Switch15;

    /* Switch: '<S225>/Switch1' incorporates:
     *  Constant: '<S225>/Constant2'
     *  Constant: '<S238>/Constant'
     *  RelationalOperator: '<S238>/Compare'
     */
    if (MCU_AI_B.Switch2_kc <= 1.0F)
    {
        MCU_AI_B.Switch2_kc = 1.0F;

        /* Switch: '<S225>/Switch17' incorporates:
         *  Constant: '<S225>/Constant2'
         *  Constant: '<S225>/Constant55'
         */
        MCU_AI_B.Switch12_e = 0.0F;
    }
    else
    {
        /* Product: '<S31>/Divide36' incorporates:
         *  Constant: '<S31>/Constant24'
         *  Product: '<S31>/Product7'
         *  Sum: '<S31>/Add10'
         *  Switch: '<S225>/Switch17'
         */
        MCU_AI_B.Switch12_e = MCU_AI_B.Switch2_kc * 0.5F * MCU_AI_B.Switch17 /
            MCU_AI_B.Product8;

        /* Sum: '<S1>/Add5' incorporates:
         *  Switch: '<S225>/Switch17'
         */
        MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch13 + MCU_AI_B.Min4_p;

        /* Switch: '<S229>/Switch2' incorporates:
         *  RelationalOperator: '<S229>/LowerRelop1'
         *  Switch: '<S225>/Switch17'
         */
        if (!(MCU_AI_B.Switch12_e > MCU_AI_B.Divide35_o3))
        {
            /* Product: '<S31>/Product2' incorporates:
             *  Sum: '<S1>/Add1'
             */
            MCU_AI_B.Divide35_o3 = -(MCU_AI_B.Min_d + MCU_AI_B.Min_h);

            /* Switch: '<S229>/Switch' incorporates:
             *  RelationalOperator: '<S229>/UpperRelop'
             */
            if (!(MCU_AI_B.Switch12_e < MCU_AI_B.Divide35_o3))
            {
                MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch12_e;
            }

            /* End of Switch: '<S229>/Switch' */
        }

        /* End of Switch: '<S229>/Switch2' */

        /* Switch: '<S31>/Switch12' incorporates:
         *  Constant: '<S221>/Constant'
         *  Constant: '<S31>/Constant18'
         *  Product: '<S31>/Product11'
         *  RelationalOperator: '<S221>/Compare'
         *  Switch: '<S225>/Switch17'
         */
        if (MCU_AI_B.Switch19 * MCU_AI_B.Divide35_o3 < 0.0F)
        {
            MCU_AI_B.Switch12_e = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch12_e = MCU_AI_B.Divide35_o3;
        }

        /* End of Switch: '<S31>/Switch12' */
    }

    /* End of Switch: '<S225>/Switch1' */

    /* Product: '<S225>/Divide35' */
    MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch14 * MCU_AI_B.Switch12_e /
        MCU_AI_B.Switch2_kc;

    /* Switch: '<S242>/Switch2' incorporates:
     *  Product: '<S225>/Product1'
     *  RelationalOperator: '<S242>/LowerRelop1'
     *  RelationalOperator: '<S242>/UpperRelop'
     *  Switch: '<S242>/Switch'
     */
    if (MCU_AI_B.Divide35_o3 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch13;
    }
    else if (MCU_AI_B.Divide35_o3 < -MCU_AI_B.Min_d)
    {
        /* Switch: '<S242>/Switch' incorporates:
         *  Product: '<S225>/Product1'
         */
        MCU_AI_B.Divide35_o3 = -MCU_AI_B.Min_d;
    }

    /* End of Switch: '<S242>/Switch2' */

    /* Switch: '<S225>/Switch11' incorporates:
     *  Constant: '<S225>/Constant17'
     *  Constant: '<S240>/Constant'
     *  Product: '<S225>/Product10'
     *  RelationalOperator: '<S240>/Compare'
     */
    if (MCU_AI_B.DataTypeConversion9 * MCU_AI_B.Divide35_o3 < 0.0F)
    {
        MCU_AI_B.Divide35_o3 = 0.0F;
    }

    /* End of Switch: '<S225>/Switch11' */

    /* Switch: '<S31>/Switch3' incorporates:
     *  Abs: '<S31>/Abs2'
     *  Constant: '<S212>/Constant'
     *  Constant: '<S31>/Constant10'
     *  DataTypeConversion: '<S31>/Data Type Conversion2'
     *  RelationalOperator: '<S212>/Compare'
     */
    if (fabsf(MCU_AI_B.Divide35_o3) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Divide35_o3;
    }

    /* End of Switch: '<S31>/Switch3' */

    /* DataTypeConversion: '<S31>/Data Type Conversion5' */
    MCU_AI_B.Switch14 = rtb_Switch15_e;

    /* Switch: '<S28>/Switch28' incorporates:
     *  Constant: '<S28>/Constant27'
     *  DataTypeConversion: '<S31>/Data Type Conversion5'
     *  Sum: '<S28>/Subtract5'
     *  Switch: '<S28>/Switch10'
     */
    if (rtb_Compare_m0)
    {
        /* Switch: '<S28>/Switch10' incorporates:
         *  Product: '<S28>/Product12'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Switch13;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_d;
        }

        MCU_AI_B.Switch28 = (real32_T)MCU_AI_B.i - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Switch28 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch28' */

    /* Product: '<S225>/Divide36' */
    MCU_AI_B.Switch2_kc = MCU_AI_B.Switch15 * MCU_AI_B.Switch12_e /
        MCU_AI_B.Switch2_kc;

    /* Switch: '<S243>/Switch2' incorporates:
     *  Product: '<S225>/Product2'
     *  RelationalOperator: '<S243>/LowerRelop1'
     *  RelationalOperator: '<S243>/UpperRelop'
     *  Switch: '<S243>/Switch'
     */
    if (MCU_AI_B.Switch2_kc > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch2_kc = MCU_AI_B.Min4_p;
    }
    else if (MCU_AI_B.Switch2_kc < -MCU_AI_B.Min_h)
    {
        /* Switch: '<S243>/Switch' incorporates:
         *  Product: '<S225>/Product2'
         */
        MCU_AI_B.Switch2_kc = -MCU_AI_B.Min_h;
    }

    /* End of Switch: '<S243>/Switch2' */

    /* Switch: '<S225>/Switch12' incorporates:
     *  Constant: '<S225>/Constant18'
     *  Constant: '<S241>/Constant'
     *  Product: '<S225>/Product11'
     *  RelationalOperator: '<S241>/Compare'
     */
    if (MCU_AI_B.DataTypeConversion9 * MCU_AI_B.Switch2_kc < 0.0F)
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S225>/Switch12' */

    /* Switch: '<S31>/Switch4' incorporates:
     *  Abs: '<S31>/Abs3'
     *  Constant: '<S214>/Constant'
     *  Constant: '<S31>/Constant11'
     *  DataTypeConversion: '<S31>/Data Type Conversion3'
     *  RelationalOperator: '<S214>/Compare'
     */
    if (fabsf(MCU_AI_B.DataTypeConversion9) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.DataTypeConversion9;
    }

    /* End of Switch: '<S31>/Switch4' */

    /* DataTypeConversion: '<S31>/Data Type Conversion6' */
    MCU_AI_B.DataTypeConversion6_b = rtb_Switch15_e;

    /* Switch: '<S28>/Switch29' incorporates:
     *  Constant: '<S28>/Constant28'
     *  DataTypeConversion: '<S31>/Data Type Conversion6'
     *  Sum: '<S28>/Subtract6'
     *  Switch: '<S28>/Switch11'
     */
    if (rtb_Compare_kvd)
    {
        /* Switch: '<S28>/Switch11' incorporates:
         *  Product: '<S28>/Product13'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Min4_p;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_h;
        }

        MCU_AI_B.Switch29 = (real32_T)MCU_AI_B.i - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Switch29 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch29' */

    /* Switch: '<S226>/Switch19' incorporates:
     *  Constant: '<S226>/Constant25'
     *  Constant: '<S226>/Constant26'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.DataTypeConversion9 = 1.0F;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = -1.0F;
    }

    /* End of Switch: '<S226>/Switch19' */

    /* Sum: '<S226>/Add9' incorporates:
     *  Sum: '<S31>/Add11'
     *  Switch: '<S226>/Switch17'
     */
    MCU_AI_B.Switch12_e = MCU_AI_B.Switch5 + MCU_AI_B.Switch6;
    MCU_AI_B.Switch2_kc = MCU_AI_B.Switch12_e;

    /* Switch: '<S226>/Switch1' incorporates:
     *  Constant: '<S226>/Constant2'
     *  Constant: '<S244>/Constant'
     *  RelationalOperator: '<S244>/Compare'
     *  Sum: '<S226>/Add9'
     */
    if (MCU_AI_B.Switch12_e <= 1.0F)
    {
        MCU_AI_B.Switch2_kc = 1.0F;
    }

    /* End of Switch: '<S226>/Switch1' */

    /* Switch: '<S24>/Switch5' incorporates:
     *  Constant: '<S24>/Constant7'
     *  Inport: '<Root>/bms6_max_discharge_power'
     *  Inport: '<Root>/bms6_soc'
     *  RelationalOperator: '<S24>/LessThanOrEqual'
     */
    if (bms6_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms6_max_discharge_power;
    }

    /* MinMax: '<S24>/Min4' incorporates:
     *  Switch: '<S24>/Switch5'
     */
    MCU_AI_B.Switch15 = fminf(MCU_AI_B.Switch17_b, (real32_T)MCU_AI_B.i);

    /* Switch: '<S226>/Switch17' incorporates:
     *  Constant: '<S226>/Constant55'
     *  Constant: '<S245>/Constant'
     *  RelationalOperator: '<S245>/Compare'
     */
    if (MCU_AI_B.Switch2_kc <= 1.0F)
    {
        MCU_AI_B.Switch12_e = 0.0F;
    }
    else
    {
        /* Product: '<S31>/Divide37' incorporates:
         *  Constant: '<S31>/Constant4'
         *  Product: '<S31>/Product9'
         */
        MCU_AI_B.Switch12_e = MCU_AI_B.Switch12_e * 0.5F * MCU_AI_B.Switch17 /
            MCU_AI_B.Product8;

        /* Sum: '<S1>/Add8' */
        MCU_AI_B.Divide35_o3 = MCU_AI_B.Min4_pm + MCU_AI_B.Switch15;

        /* Switch: '<S230>/Switch2' incorporates:
         *  RelationalOperator: '<S230>/LowerRelop1'
         */
        if (!(MCU_AI_B.Switch12_e > MCU_AI_B.Divide35_o3))
        {
            /* Product: '<S31>/Product3' incorporates:
             *  Sum: '<S1>/Add4'
             */
            MCU_AI_B.Divide35_o3 = -(MCU_AI_B.Min_h1 + MCU_AI_B.Min_kv);

            /* Switch: '<S230>/Switch' incorporates:
             *  RelationalOperator: '<S230>/UpperRelop'
             */
            if (!(MCU_AI_B.Switch12_e < MCU_AI_B.Divide35_o3))
            {
                MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch12_e;
            }

            /* End of Switch: '<S230>/Switch' */
        }

        /* End of Switch: '<S230>/Switch2' */

        /* Switch: '<S31>/Switch13' incorporates:
         *  Constant: '<S222>/Constant'
         *  Constant: '<S31>/Constant19'
         *  Product: '<S31>/Product12'
         *  RelationalOperator: '<S222>/Compare'
         */
        if (MCU_AI_B.Switch19 * MCU_AI_B.Divide35_o3 < 0.0F)
        {
            MCU_AI_B.Switch12_e = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch12_e = MCU_AI_B.Divide35_o3;
        }

        /* End of Switch: '<S31>/Switch13' */
    }

    /* Product: '<S226>/Divide35' */
    MCU_AI_B.Divide35_o3 = MCU_AI_B.Switch5 * MCU_AI_B.Switch12_e /
        MCU_AI_B.Switch2_kc;

    /* Switch: '<S248>/Switch2' incorporates:
     *  Product: '<S226>/Product1'
     *  RelationalOperator: '<S248>/LowerRelop1'
     *  RelationalOperator: '<S248>/UpperRelop'
     *  Switch: '<S248>/Switch'
     */
    if (MCU_AI_B.Divide35_o3 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.Divide35_o3 = MCU_AI_B.Min4_pm;
    }
    else if (MCU_AI_B.Divide35_o3 < -MCU_AI_B.Min_h1)
    {
        /* Switch: '<S248>/Switch' incorporates:
         *  Product: '<S226>/Product1'
         */
        MCU_AI_B.Divide35_o3 = -MCU_AI_B.Min_h1;
    }

    /* End of Switch: '<S248>/Switch2' */

    /* Switch: '<S226>/Switch11' incorporates:
     *  Constant: '<S226>/Constant17'
     *  Constant: '<S246>/Constant'
     *  Product: '<S226>/Product10'
     *  RelationalOperator: '<S246>/Compare'
     */
    if (MCU_AI_B.DataTypeConversion9 * MCU_AI_B.Divide35_o3 < 0.0F)
    {
        MCU_AI_B.Divide35_o3 = 0.0F;
    }

    /* End of Switch: '<S226>/Switch11' */

    /* Switch: '<S31>/Switch5' incorporates:
     *  Abs: '<S31>/Abs4'
     *  Constant: '<S216>/Constant'
     *  Constant: '<S31>/Constant12'
     *  DataTypeConversion: '<S31>/Data Type Conversion7'
     *  RelationalOperator: '<S216>/Compare'
     */
    if (fabsf(MCU_AI_B.Divide35_o3) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Divide35_o3;
    }

    /* End of Switch: '<S31>/Switch5' */

    /* DataTypeConversion: '<S31>/Data Type Conversion8' */
    MCU_AI_B.Divide35_o3 = rtb_Switch15_e;

    /* Switch: '<S28>/Switch30' incorporates:
     *  Constant: '<S28>/Constant29'
     *  DataTypeConversion: '<S31>/Data Type Conversion8'
     *  Sum: '<S28>/Subtract7'
     *  Switch: '<S28>/Switch3'
     */
    if (rtb_Compare_ge)
    {
        /* Switch: '<S28>/Switch3' incorporates:
         *  Product: '<S28>/Product14'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Min4_pm;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_h1;
        }

        MCU_AI_B.Switch5 = (real32_T)MCU_AI_B.i - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Switch5 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch30' */

    /* Product: '<S226>/Divide36' */
    MCU_AI_B.Switch2_kc = MCU_AI_B.Switch6 * MCU_AI_B.Switch12_e /
        MCU_AI_B.Switch2_kc;

    /* Switch: '<S249>/Switch2' incorporates:
     *  Product: '<S226>/Product2'
     *  RelationalOperator: '<S249>/LowerRelop1'
     *  RelationalOperator: '<S249>/UpperRelop'
     *  Switch: '<S249>/Switch'
     */
    if (MCU_AI_B.Switch2_kc > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_kc = MCU_AI_B.Switch15;
    }
    else if (MCU_AI_B.Switch2_kc < -MCU_AI_B.Min_kv)
    {
        /* Switch: '<S249>/Switch' incorporates:
         *  Product: '<S226>/Product2'
         */
        MCU_AI_B.Switch2_kc = -MCU_AI_B.Min_kv;
    }

    /* End of Switch: '<S249>/Switch2' */

    /* Switch: '<S226>/Switch12' incorporates:
     *  Constant: '<S226>/Constant18'
     *  Constant: '<S247>/Constant'
     *  Product: '<S226>/Product11'
     *  RelationalOperator: '<S247>/Compare'
     */
    if (MCU_AI_B.DataTypeConversion9 * MCU_AI_B.Switch2_kc < 0.0F)
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S226>/Switch12' */

    /* Switch: '<S31>/Switch6' incorporates:
     *  Abs: '<S31>/Abs5'
     *  Constant: '<S217>/Constant'
     *  Constant: '<S31>/Constant13'
     *  DataTypeConversion: '<S31>/Data Type Conversion9'
     *  RelationalOperator: '<S217>/Compare'
     */
    if (fabsf(MCU_AI_B.DataTypeConversion9) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.DataTypeConversion9;
    }

    /* End of Switch: '<S31>/Switch6' */

    /* DataTypeConversion: '<S31>/Data Type Conversion10' */
    MCU_AI_B.Switch6 = rtb_Switch15_e;

    /* Switch: '<S28>/Switch31' incorporates:
     *  Constant: '<S28>/Constant30'
     *  DataTypeConversion: '<S31>/Data Type Conversion10'
     *  Sum: '<S28>/Subtract8'
     *  Switch: '<S28>/Switch4'
     */
    if (rtb_Compare_hr)
    {
        /* Switch: '<S28>/Switch4' incorporates:
         *  Product: '<S28>/Product15'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Switch15;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_kv;
        }

        MCU_AI_B.Switch19 = (real32_T)MCU_AI_B.i - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Switch19 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch31' */

    /* Product: '<S227>/Divide35' */
    MCU_AI_B.Product7_d = MCU_AI_B.Switch7 * MCU_AI_B.Product7_d /
        MCU_AI_B.Square1;

    /* Switch: '<S254>/Switch2' incorporates:
     *  RelationalOperator: '<S254>/LowerRelop1'
     */
    if (MCU_AI_B.Product7_d > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.Square1 = MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S227>/Product1' */
        MCU_AI_B.Square1 = -MCU_AI_B.Min_l;

        /* Switch: '<S254>/Switch' incorporates:
         *  Product: '<S227>/Product1'
         *  RelationalOperator: '<S254>/UpperRelop'
         */
        if (!(MCU_AI_B.Product7_d < -MCU_AI_B.Min_l))
        {
            MCU_AI_B.Square1 = MCU_AI_B.Product7_d;
        }

        /* End of Switch: '<S254>/Switch' */
    }

    /* End of Switch: '<S254>/Switch2' */

    /* Switch: '<S227>/Switch11' incorporates:
     *  Constant: '<S227>/Constant17'
     *  Constant: '<S252>/Constant'
     *  Product: '<S227>/Product10'
     *  RelationalOperator: '<S252>/Compare'
     */
    if (MCU_AI_B.Switch9_f * MCU_AI_B.Square1 < 0.0F)
    {
        MCU_AI_B.Switch9_f = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch9_f = MCU_AI_B.Square1;
    }

    /* End of Switch: '<S227>/Switch11' */

    /* Switch: '<S31>/Switch7' incorporates:
     *  Abs: '<S31>/Abs6'
     *  Constant: '<S218>/Constant'
     *  Constant: '<S31>/Constant14'
     *  DataTypeConversion: '<S31>/Data Type Conversion12'
     *  RelationalOperator: '<S218>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch9_f) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S31>/Switch7' */

    /* DataTypeConversion: '<S31>/Data Type Conversion13' */
    MCU_AI_B.Switch7 = rtb_Switch15_e;

    /* Switch: '<S28>/Switch32' incorporates:
     *  Constant: '<S28>/Constant31'
     *  DataTypeConversion: '<S31>/Data Type Conversion13'
     *  Sum: '<S28>/Subtract9'
     *  Switch: '<S28>/Switch1'
     */
    if (rtb_Compare)
    {
        /* Switch: '<S28>/Switch1' incorporates:
         *  Product: '<S28>/Product16'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Min4_f;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_l;
        }

        MCU_AI_B.Product8 = (real32_T)MCU_AI_B.i - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Product8 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch32' */

    /* Switch: '<S28>/Switch33' incorporates:
     *  Constant: '<S28>/Constant32'
     *  Sum: '<S28>/Subtract10'
     *  Switch: '<S28>/Switch2'
     */
    if (rtb_Compare_j)
    {
        /* Switch: '<S28>/Switch2' incorporates:
         *  Product: '<S28>/Product17'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.i = (int32_T)MCU_AI_B.Min4_l;
        }
        else
        {
            MCU_AI_B.i = (int32_T)-MCU_AI_B.Min_m;
        }

        MCU_AI_B.Product7_d = (real32_T)MCU_AI_B.i - (real32_T)
            MCU_AI_B.DataTypeConversion15;
    }
    else
    {
        MCU_AI_B.Product7_d = 0.0F;
    }

    /* End of Switch: '<S28>/Switch33' */

    /* Sum: '<S28>/Add1' */
    MCU_AI_B.Switch9_f = ((((((MCU_AI_B.Switch2_apa + MCU_AI_B.Switch27) +
        MCU_AI_B.Switch28) + MCU_AI_B.Switch29) + MCU_AI_B.Switch5) +
                           MCU_AI_B.Switch19) + MCU_AI_B.Product8) +
        MCU_AI_B.Product7_d;

    /* RelationalOperator: '<S42>/Compare' incorporates:
     *  Abs: '<S28>/Abs8'
     *  Constant: '<S42>/Constant'
     */
    rtb_Compare_ps = (fabsf(MCU_AI_B.Switch9_f) <= 0.4F);

    /* Product: '<S31>/Product5' incorporates:
     *  DataTypeConversion: '<S31>/Data Type Conversion13'
     *  Sum: '<S31>/Add4'
     *  Sum: '<S31>/Subtract4'
     */
    MCU_AI_B.Square1 = MCU_AI_B.Switch17 - (((((((MCU_AI_B.Switch20 + (real32_T)
        MCU_AI_B.DataTypeConversion4) + MCU_AI_B.Switch14) +
        MCU_AI_B.DataTypeConversion6_b) + MCU_AI_B.Divide35_o3) +
        MCU_AI_B.Switch6) + (real32_T)rtb_Switch15_e) + (real32_T)
        MCU_AI_B.DataTypeConversion15);

    /* Switch: '<S31>/Switch9' incorporates:
     *  Abs: '<S31>/Abs8'
     *  Constant: '<S211>/Constant'
     *  Constant: '<S31>/Constant16'
     *  DataTypeConversion: '<S31>/Data Type Conversion16'
     *  RelationalOperator: '<S211>/Compare'
     */
    if (fabsf(MCU_AI_B.Square1) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Square1;
    }

    /* End of Switch: '<S31>/Switch9' */

    /* Switch: '<S28>/Switch17' incorporates:
     *  Constant: '<S28>/Constant26'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide11'
     *  Switch: '<S28>/Switch26'
     */
    if (rtb_Compare_ps)
    {
        /* Switch: '<S28>/Switch18' incorporates:
         *  Constant: '<S28>/Constant5'
         *  Constant: '<S28>/Constant6'
         */
        if (rtb_Compare_bq)
        {
            MCU_AI_B.Switch17_b = 0.4F;
        }
        else
        {
            MCU_AI_B.Switch17_b = -0.4F;
        }

        /* End of Switch: '<S28>/Switch18' */
        MCU_AI_B.Switch9_f = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch17_b = MCU_AI_B.Switch9_f;
        MCU_AI_B.Switch9_f = (real32_T)rtb_Switch15_e * MCU_AI_B.Product7_d /
            MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S28>/Switch17' */

    /* Sum: '<S28>/Add9' */
    MCU_AI_B.Switch9_f += (real32_T)MCU_AI_B.DataTypeConversion15;

    /* Switch: '<S28>/Switch41' incorporates:
     *  Constant: '<S28>/Constant42'
     *  Constant: '<S50>/Constant'
     *  Product: '<S28>/Product5'
     *  RelationalOperator: '<S50>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Switch9_f < 0.0F)
    {
        MCU_AI_B.Switch9_f = 0.0F;
    }

    /* End of Switch: '<S28>/Switch41' */

    /* Switch: '<S28>/Switch16' incorporates:
     *  Abs: '<S28>/Abs7'
     *  Constant: '<S40>/Constant'
     *  RelationalOperator: '<S40>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch9_f) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion15' */
        MCU_AI_B.Switch9_f = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion15' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion7'
         */
        MCU_AI_B.Switch9_f = (int16_T)MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S28>/Switch16' */

    /* Switch: '<S28>/Switch19' incorporates:
     *  Constant: '<S28>/Constant7'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide35'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Square1 = 0.0F;
    }
    else
    {
        MCU_AI_B.Square1 = (real32_T)rtb_Switch15_e * MCU_AI_B.Switch2_apa /
            MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch19' */

    /* Sum: '<S28>/Add2' */
    MCU_AI_B.Switch20 += MCU_AI_B.Square1;

    /* Switch: '<S28>/Switch38' incorporates:
     *  Constant: '<S28>/Constant37'
     *  Constant: '<S43>/Constant'
     *  Product: '<S28>/Product1'
     *  RelationalOperator: '<S43>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Switch20 < 0.0F)
    {
        MCU_AI_B.Product7_d = 0.0F;
    }
    else
    {
        MCU_AI_B.Product7_d = MCU_AI_B.Switch20;
    }

    /* End of Switch: '<S28>/Switch38' */

    /* Switch: '<S28>/Switch5' incorporates:
     *  Abs: '<S28>/Abs'
     *  Constant: '<S37>/Constant'
     *  RelationalOperator: '<S37>/Compare'
     */
    if (fabsf(MCU_AI_B.Product7_d) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion' */
        MCU_AI_B.Square1 = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion11'
         */
        MCU_AI_B.Square1 = (int16_T)MCU_AI_B.Product7_d;
    }

    /* End of Switch: '<S28>/Switch5' */

    /* Switch: '<S28>/Switch20' incorporates:
     *  Constant: '<S28>/Constant20'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide5'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Product7_d = 0.0F;
    }
    else
    {
        MCU_AI_B.Product7_d = (real32_T)rtb_Switch15_e * MCU_AI_B.Switch27 /
            MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch20' */

    /* Sum: '<S28>/Add3' */
    MCU_AI_B.Product7_d += (real32_T)MCU_AI_B.DataTypeConversion4;

    /* Switch: '<S28>/Switch39' incorporates:
     *  Constant: '<S28>/Constant34'
     *  Constant: '<S44>/Constant'
     *  Product: '<S28>/Product6'
     *  RelationalOperator: '<S44>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Product7_d < 0.0F)
    {
        MCU_AI_B.Product7_d = 0.0F;
    }

    /* End of Switch: '<S28>/Switch39' */

    /* Switch: '<S28>/Switch6' incorporates:
     *  Abs: '<S28>/Abs1'
     *  Constant: '<S33>/Constant'
     *  RelationalOperator: '<S33>/Compare'
     */
    if (fabsf(MCU_AI_B.Product7_d) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion8' */
        MCU_AI_B.Product7_d = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion8' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion1'
         */
        MCU_AI_B.Product7_d = (int16_T)MCU_AI_B.Product7_d;
    }

    /* End of Switch: '<S28>/Switch6' */

    /* Switch: '<S28>/Switch21' incorporates:
     *  Constant: '<S28>/Constant21'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide6'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }
    else
    {
        MCU_AI_B.DataTypeConversion9 = (real32_T)rtb_Switch15_e *
            MCU_AI_B.Switch28 / MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch21' */

    /* Sum: '<S28>/Add4' */
    MCU_AI_B.DataTypeConversion9 += MCU_AI_B.Switch14;

    /* Switch: '<S28>/Switch35' incorporates:
     *  Constant: '<S28>/Constant35'
     *  Constant: '<S45>/Constant'
     *  Product: '<S28>/Product7'
     *  RelationalOperator: '<S45>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.DataTypeConversion9 < 0.0F)
    {
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch35' */

    /* Switch: '<S28>/Switch7' incorporates:
     *  Abs: '<S28>/Abs2'
     *  Constant: '<S35>/Constant'
     *  RelationalOperator: '<S35>/Compare'
     */
    if (fabsf(MCU_AI_B.DataTypeConversion9) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion9' */
        MCU_AI_B.DataTypeConversion9 = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion9' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion2'
         */
        MCU_AI_B.DataTypeConversion9 = (int16_T)MCU_AI_B.DataTypeConversion9;
    }

    /* End of Switch: '<S28>/Switch7' */

    /* Switch: '<S28>/Switch22' incorporates:
     *  Constant: '<S28>/Constant22'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide7'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Switch2_kc = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch2_kc = (real32_T)rtb_Switch15_e * MCU_AI_B.Switch29 /
            MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch22' */

    /* Sum: '<S28>/Add5' */
    MCU_AI_B.Switch2_kc += MCU_AI_B.DataTypeConversion6_b;

    /* Switch: '<S28>/Switch37' incorporates:
     *  Constant: '<S28>/Constant36'
     *  Constant: '<S46>/Constant'
     *  Product: '<S28>/Product8'
     *  RelationalOperator: '<S46>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Switch2_kc < 0.0F)
    {
        MCU_AI_B.Switch2_kc = 0.0F;
    }

    /* End of Switch: '<S28>/Switch37' */

    /* Switch: '<S28>/Switch8' incorporates:
     *  Abs: '<S28>/Abs3'
     *  Constant: '<S36>/Constant'
     *  RelationalOperator: '<S36>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch2_kc) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion10' */
        MCU_AI_B.Switch2_kc = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion10' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion3'
         */
        MCU_AI_B.Switch2_kc = (int16_T)MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S28>/Switch8' */

    /* Switch: '<S28>/Switch23' incorporates:
     *  Constant: '<S28>/Constant23'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide8'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Switch12_e = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch12_e = (real32_T)rtb_Switch15_e * MCU_AI_B.Switch5 /
            MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch23' */

    /* Sum: '<S28>/Add6' */
    MCU_AI_B.Switch12_e += MCU_AI_B.Divide35_o3;

    /* Switch: '<S28>/Switch42' incorporates:
     *  Constant: '<S28>/Constant43'
     *  Constant: '<S47>/Constant'
     *  Product: '<S28>/Product2'
     *  RelationalOperator: '<S47>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Switch12_e < 0.0F)
    {
        MCU_AI_B.Switch12_e = 0.0F;
    }

    /* End of Switch: '<S28>/Switch42' */

    /* Switch: '<S28>/Switch13' incorporates:
     *  Abs: '<S28>/Abs4'
     *  Constant: '<S41>/Constant'
     *  RelationalOperator: '<S41>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch12_e) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion14' */
        MCU_AI_B.Switch12_e = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion14' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion5'
         */
        MCU_AI_B.Switch12_e = (int16_T)MCU_AI_B.Switch12_e;
    }

    /* End of Switch: '<S28>/Switch13' */

    /* Switch: '<S28>/Switch24' incorporates:
     *  Constant: '<S28>/Constant24'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide9'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Divide35_o3 = 0.0F;
    }
    else
    {
        MCU_AI_B.Divide35_o3 = (real32_T)rtb_Switch15_e * MCU_AI_B.Switch19 /
            MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch24' */

    /* Sum: '<S28>/Add7' */
    MCU_AI_B.Divide35_o3 += MCU_AI_B.Switch6;

    /* Switch: '<S28>/Switch43' incorporates:
     *  Constant: '<S28>/Constant40'
     *  Constant: '<S48>/Constant'
     *  Product: '<S28>/Product3'
     *  RelationalOperator: '<S48>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Divide35_o3 < 0.0F)
    {
        MCU_AI_B.Divide35_o3 = 0.0F;
    }

    /* End of Switch: '<S28>/Switch43' */

    /* Switch: '<S28>/Switch14' incorporates:
     *  Abs: '<S28>/Abs5'
     *  Constant: '<S38>/Constant'
     *  RelationalOperator: '<S38>/Compare'
     */
    if (fabsf(MCU_AI_B.Divide35_o3) <= 0.1F)
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion16' */
        MCU_AI_B.Divide35_o3 = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S28>/Data Type Conversion16' incorporates:
         *  DataTypeConversion: '<S28>/Data Type Conversion4'
         */
        MCU_AI_B.Divide35_o3 = (int16_T)MCU_AI_B.Divide35_o3;
    }

    /* End of Switch: '<S28>/Switch14' */

    /* Switch: '<S28>/Switch25' incorporates:
     *  Constant: '<S28>/Constant25'
     *  DataTypeConversion: '<S31>/Data Type Conversion17'
     *  Product: '<S28>/Divide10'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Switch20 = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch20 = (real32_T)rtb_Switch15_e * MCU_AI_B.Product8 /
            MCU_AI_B.Switch17_b;
    }

    /* End of Switch: '<S28>/Switch25' */

    /* Sum: '<S28>/Add8' */
    MCU_AI_B.Switch20 += MCU_AI_B.Switch7;

    /* Switch: '<S28>/Switch40' incorporates:
     *  Constant: '<S28>/Constant41'
     *  Constant: '<S49>/Constant'
     *  Product: '<S28>/Product4'
     *  RelationalOperator: '<S49>/Compare'
     */
    if (MCU_AI_B.Abs7 * MCU_AI_B.Switch20 < 0.0F)
    {
        MCU_AI_B.Abs7 = 0.0F;
    }
    else
    {
        MCU_AI_B.Abs7 = MCU_AI_B.Switch20;
    }

    /* End of Switch: '<S28>/Switch40' */

    /* Switch: '<S28>/Switch15' incorporates:
     *  Abs: '<S28>/Abs6'
     *  Constant: '<S28>/Constant3'
     *  Constant: '<S39>/Constant'
     *  DataTypeConversion: '<S28>/Data Type Conversion6'
     *  RelationalOperator: '<S39>/Compare'
     */
    if (fabsf(MCU_AI_B.Abs7) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Abs7;
    }

    /* End of Switch: '<S28>/Switch15' */

    /* DataTypeConversion: '<S28>/Data Type Conversion17' */
    MCU_AI_B.Abs7 = rtb_Switch15_e;

    /* Sum: '<S28>/Subtract4' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion17'
     *  Sum: '<S28>/Add14'
     */
    MCU_AI_B.Product8_n -= ((((((MCU_AI_B.Square1 + MCU_AI_B.Product7_d) +
        MCU_AI_B.DataTypeConversion9) + MCU_AI_B.Switch2_kc) +
        MCU_AI_B.Switch12_e) + MCU_AI_B.Divide35_o3) + (real32_T)rtb_Switch15_e)
        + MCU_AI_B.Switch9_f;

    /* Switch: '<S28>/Switch34' incorporates:
     *  Abs: '<S28>/Abs9'
     *  Constant: '<S28>/Constant33'
     *  Constant: '<S34>/Constant'
     *  DataTypeConversion: '<S28>/Data Type Conversion13'
     *  RelationalOperator: '<S34>/Compare'
     */
    if (fabsf(MCU_AI_B.Product8_n) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Product8_n;
    }

    /* End of Switch: '<S28>/Switch34' */

    /* Sum: '<S59>/Add4' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S72>/Switch2' incorporates:
     *  RelationalOperator: '<S72>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S59>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S72>/Switch' incorporates:
         *  Product: '<S59>/Product8'
         *  RelationalOperator: '<S72>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S72>/Switch' */
    }

    /* End of Switch: '<S72>/Switch2' */

    /* Sum: '<S59>/Add3' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S70>/Switch2' incorporates:
     *  RelationalOperator: '<S70>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S59>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S70>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S70>/Switch' incorporates:
         *  RelationalOperator: '<S70>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S70>/Switch' */
    }

    /* End of Switch: '<S70>/Switch2' */

    /* Sum: '<S59>/Add2' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S69>/Switch2' incorporates:
     *  RelationalOperator: '<S69>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S59>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S69>/Switch' incorporates:
         *  Product: '<S59>/Product6'
         *  RelationalOperator: '<S69>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S69>/Switch' */
    }

    /* End of Switch: '<S69>/Switch2' */

    /* Sum: '<S59>/Add1' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S68>/Switch2' incorporates:
     *  RelationalOperator: '<S68>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S59>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S68>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S68>/Switch' incorporates:
         *  RelationalOperator: '<S68>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S68>/Switch' */
    }

    /* End of Switch: '<S68>/Switch2' */

    /* Sum: '<S59>/Add8' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S75>/Switch2' incorporates:
     *  RelationalOperator: '<S75>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S59>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S75>/Switch' incorporates:
         *  Product: '<S59>/Product4'
         *  RelationalOperator: '<S75>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S75>/Switch' */
    }

    /* End of Switch: '<S75>/Switch2' */

    /* Sum: '<S59>/Add7' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S74>/Switch2' incorporates:
     *  RelationalOperator: '<S74>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S59>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S74>/Switch' incorporates:
         *  Product: '<S59>/Product3'
         *  RelationalOperator: '<S74>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S74>/Switch' */
    }

    /* End of Switch: '<S74>/Switch2' */

    /* Sum: '<S59>/Add6' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S73>/Switch2' incorporates:
     *  RelationalOperator: '<S73>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S59>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S73>/Switch' incorporates:
         *  Product: '<S59>/Product2'
         *  RelationalOperator: '<S73>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S73>/Switch' */
    }

    /* End of Switch: '<S73>/Switch2' */

    /* Sum: '<S59>/Add' incorporates:
     *  DataTypeConversion: '<S28>/Data Type Conversion12'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S71>/Switch2' incorporates:
     *  RelationalOperator: '<S71>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S59>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S71>/Switch' incorporates:
         *  Product: '<S59>/Product1'
         *  RelationalOperator: '<S71>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S71>/Switch' */
    }

    /* End of Switch: '<S71>/Switch2' */

    /* SwitchCase: '<S59>/Switch Case' incorporates:
     *  Sum: '<S59>/Subtract1'
     *  Sum: '<S59>/Subtract2'
     *  Sum: '<S59>/Subtract3'
     *  Sum: '<S59>/Subtract4'
     *  Sum: '<S59>/Subtract5'
     *  Sum: '<S59>/Subtract7'
     *  Sum: '<S59>/Subtract8'
     *  Sum: '<S59>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[0])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S76>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge,
            &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S77>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S78>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S79>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S80>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S81>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S82>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S59>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S83>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge, &MCU_AI_B.Merge2);

        /* End of Outputs for SubSystem: '<S59>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S59>/Switch Case' */

    /* Switch: '<S59>/Switch5' incorporates:
     *  Abs: '<S59>/Abs4'
     *  Constant: '<S59>/Constant8'
     *  Constant: '<S67>/Constant'
     *  DataTypeConversion: '<S59>/Data Type Conversion8'
     *  RelationalOperator: '<S67>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge);
    }

    /* End of Switch: '<S59>/Switch5' */

    /* Sum: '<S60>/Add4' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S89>/Switch2' incorporates:
     *  RelationalOperator: '<S89>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S60>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S89>/Switch' incorporates:
         *  Product: '<S60>/Product8'
         *  RelationalOperator: '<S89>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S89>/Switch' */
    }

    /* End of Switch: '<S89>/Switch2' */

    /* Sum: '<S60>/Add3' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S87>/Switch2' incorporates:
     *  RelationalOperator: '<S87>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S60>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S87>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S87>/Switch' incorporates:
         *  RelationalOperator: '<S87>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S87>/Switch' */
    }

    /* End of Switch: '<S87>/Switch2' */

    /* Sum: '<S60>/Add2' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S86>/Switch2' incorporates:
     *  RelationalOperator: '<S86>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S60>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S86>/Switch' incorporates:
         *  Product: '<S60>/Product6'
         *  RelationalOperator: '<S86>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S86>/Switch' */
    }

    /* End of Switch: '<S86>/Switch2' */

    /* Sum: '<S60>/Add1' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S85>/Switch2' incorporates:
     *  RelationalOperator: '<S85>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S60>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S85>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S85>/Switch' incorporates:
         *  RelationalOperator: '<S85>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S85>/Switch' */
    }

    /* End of Switch: '<S85>/Switch2' */

    /* Sum: '<S60>/Add8' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S92>/Switch2' incorporates:
     *  RelationalOperator: '<S92>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S60>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S92>/Switch' incorporates:
         *  Product: '<S60>/Product4'
         *  RelationalOperator: '<S92>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S92>/Switch' */
    }

    /* End of Switch: '<S92>/Switch2' */

    /* Sum: '<S60>/Add7' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S91>/Switch2' incorporates:
     *  RelationalOperator: '<S91>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S60>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S91>/Switch' incorporates:
         *  Product: '<S60>/Product3'
         *  RelationalOperator: '<S91>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S91>/Switch' */
    }

    /* End of Switch: '<S91>/Switch2' */

    /* Sum: '<S60>/Add6' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S90>/Switch2' incorporates:
     *  RelationalOperator: '<S90>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S60>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S90>/Switch' incorporates:
         *  Product: '<S60>/Product2'
         *  RelationalOperator: '<S90>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S90>/Switch' */
    }

    /* End of Switch: '<S90>/Switch2' */

    /* Sum: '<S60>/Add' incorporates:
     *  DataTypeConversion: '<S59>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S88>/Switch2' incorporates:
     *  RelationalOperator: '<S88>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S60>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S88>/Switch' incorporates:
         *  Product: '<S60>/Product1'
         *  RelationalOperator: '<S88>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S88>/Switch' */
    }

    /* End of Switch: '<S88>/Switch2' */

    /* SwitchCase: '<S60>/Switch Case' incorporates:
     *  Sum: '<S60>/Subtract1'
     *  Sum: '<S60>/Subtract2'
     *  Sum: '<S60>/Subtract3'
     *  Sum: '<S60>/Subtract4'
     *  Sum: '<S60>/Subtract5'
     *  Sum: '<S60>/Subtract7'
     *  Sum: '<S60>/Subtract8'
     *  Sum: '<S60>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[1])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S93>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge_g,
            &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S94>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S95>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S96>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S97>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S98>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S99>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S60>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S100>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge_g, &MCU_AI_B.Merge2_f);

        /* End of Outputs for SubSystem: '<S60>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S60>/Switch Case' */

    /* Switch: '<S60>/Switch5' incorporates:
     *  Abs: '<S60>/Abs4'
     *  Constant: '<S60>/Constant8'
     *  Constant: '<S84>/Constant'
     *  DataTypeConversion: '<S60>/Data Type Conversion8'
     *  RelationalOperator: '<S84>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_g) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_g);
    }

    /* End of Switch: '<S60>/Switch5' */

    /* Sum: '<S61>/Add4' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S106>/Switch2' incorporates:
     *  RelationalOperator: '<S106>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S61>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S106>/Switch' incorporates:
         *  Product: '<S61>/Product8'
         *  RelationalOperator: '<S106>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S106>/Switch' */
    }

    /* End of Switch: '<S106>/Switch2' */

    /* Sum: '<S61>/Add3' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S104>/Switch2' incorporates:
     *  RelationalOperator: '<S104>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S61>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S104>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S104>/Switch' incorporates:
         *  RelationalOperator: '<S104>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S104>/Switch' */
    }

    /* End of Switch: '<S104>/Switch2' */

    /* Sum: '<S61>/Add2' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S103>/Switch2' incorporates:
     *  RelationalOperator: '<S103>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S61>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S103>/Switch' incorporates:
         *  Product: '<S61>/Product6'
         *  RelationalOperator: '<S103>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S103>/Switch' */
    }

    /* End of Switch: '<S103>/Switch2' */

    /* Sum: '<S61>/Add1' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S102>/Switch2' incorporates:
     *  RelationalOperator: '<S102>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S61>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S102>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S102>/Switch' incorporates:
         *  RelationalOperator: '<S102>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S102>/Switch' */
    }

    /* End of Switch: '<S102>/Switch2' */

    /* Sum: '<S61>/Add8' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S109>/Switch2' incorporates:
     *  RelationalOperator: '<S109>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S61>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S109>/Switch' incorporates:
         *  Product: '<S61>/Product4'
         *  RelationalOperator: '<S109>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S109>/Switch' */
    }

    /* End of Switch: '<S109>/Switch2' */

    /* Sum: '<S61>/Add7' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S108>/Switch2' incorporates:
     *  RelationalOperator: '<S108>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S61>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S108>/Switch' incorporates:
         *  Product: '<S61>/Product3'
         *  RelationalOperator: '<S108>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S108>/Switch' */
    }

    /* End of Switch: '<S108>/Switch2' */

    /* Sum: '<S61>/Add6' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S107>/Switch2' incorporates:
     *  RelationalOperator: '<S107>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S61>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S107>/Switch' incorporates:
         *  Product: '<S61>/Product2'
         *  RelationalOperator: '<S107>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S107>/Switch' */
    }

    /* End of Switch: '<S107>/Switch2' */

    /* Sum: '<S61>/Add' incorporates:
     *  DataTypeConversion: '<S60>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S105>/Switch2' incorporates:
     *  RelationalOperator: '<S105>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S61>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S105>/Switch' incorporates:
         *  Product: '<S61>/Product1'
         *  RelationalOperator: '<S105>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S105>/Switch' */
    }

    /* End of Switch: '<S105>/Switch2' */

    /* SwitchCase: '<S61>/Switch Case' incorporates:
     *  Sum: '<S61>/Subtract1'
     *  Sum: '<S61>/Subtract2'
     *  Sum: '<S61>/Subtract3'
     *  Sum: '<S61>/Subtract4'
     *  Sum: '<S61>/Subtract5'
     *  Sum: '<S61>/Subtract7'
     *  Sum: '<S61>/Subtract8'
     *  Sum: '<S61>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[2])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S110>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge_d,
            &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S111>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S112>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S113>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S114>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S115>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S116>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S61>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S117>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge_d, &MCU_AI_B.Merge2_g);

        /* End of Outputs for SubSystem: '<S61>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S61>/Switch Case' */

    /* Switch: '<S61>/Switch5' incorporates:
     *  Abs: '<S61>/Abs4'
     *  Constant: '<S101>/Constant'
     *  Constant: '<S61>/Constant8'
     *  DataTypeConversion: '<S61>/Data Type Conversion8'
     *  RelationalOperator: '<S101>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_d) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_d);
    }

    /* End of Switch: '<S61>/Switch5' */

    /* Sum: '<S62>/Add4' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S123>/Switch2' incorporates:
     *  RelationalOperator: '<S123>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S62>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S123>/Switch' incorporates:
         *  Product: '<S62>/Product8'
         *  RelationalOperator: '<S123>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S123>/Switch' */
    }

    /* End of Switch: '<S123>/Switch2' */

    /* Sum: '<S62>/Add3' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S121>/Switch2' incorporates:
     *  RelationalOperator: '<S121>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S62>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S121>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S121>/Switch' incorporates:
         *  RelationalOperator: '<S121>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S121>/Switch' */
    }

    /* End of Switch: '<S121>/Switch2' */

    /* Sum: '<S62>/Add2' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S120>/Switch2' incorporates:
     *  RelationalOperator: '<S120>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S62>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S120>/Switch' incorporates:
         *  Product: '<S62>/Product6'
         *  RelationalOperator: '<S120>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S120>/Switch' */
    }

    /* End of Switch: '<S120>/Switch2' */

    /* Sum: '<S62>/Add1' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S119>/Switch2' incorporates:
     *  RelationalOperator: '<S119>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S62>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S119>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S119>/Switch' incorporates:
         *  RelationalOperator: '<S119>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S119>/Switch' */
    }

    /* End of Switch: '<S119>/Switch2' */

    /* Sum: '<S62>/Add8' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S126>/Switch2' incorporates:
     *  RelationalOperator: '<S126>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S62>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S126>/Switch' incorporates:
         *  Product: '<S62>/Product4'
         *  RelationalOperator: '<S126>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S126>/Switch' */
    }

    /* End of Switch: '<S126>/Switch2' */

    /* Sum: '<S62>/Add7' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S125>/Switch2' incorporates:
     *  RelationalOperator: '<S125>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S62>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S125>/Switch' incorporates:
         *  Product: '<S62>/Product3'
         *  RelationalOperator: '<S125>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S125>/Switch' */
    }

    /* End of Switch: '<S125>/Switch2' */

    /* Sum: '<S62>/Add6' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S124>/Switch2' incorporates:
     *  RelationalOperator: '<S124>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S62>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S124>/Switch' incorporates:
         *  Product: '<S62>/Product2'
         *  RelationalOperator: '<S124>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S124>/Switch' */
    }

    /* End of Switch: '<S124>/Switch2' */

    /* Sum: '<S62>/Add' incorporates:
     *  DataTypeConversion: '<S61>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S122>/Switch2' incorporates:
     *  RelationalOperator: '<S122>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S62>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S122>/Switch' incorporates:
         *  Product: '<S62>/Product1'
         *  RelationalOperator: '<S122>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S122>/Switch' */
    }

    /* End of Switch: '<S122>/Switch2' */

    /* SwitchCase: '<S62>/Switch Case' incorporates:
     *  Sum: '<S62>/Subtract1'
     *  Sum: '<S62>/Subtract2'
     *  Sum: '<S62>/Subtract3'
     *  Sum: '<S62>/Subtract4'
     *  Sum: '<S62>/Subtract5'
     *  Sum: '<S62>/Subtract7'
     *  Sum: '<S62>/Subtract8'
     *  Sum: '<S62>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[3])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S127>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge_gg,
            &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S128>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge_gg, &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S129>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge_gg, &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S130>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge_gg, &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S131>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge_gg, &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S132>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge_gg, &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S133>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge_gg,
            &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S62>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S134>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge_gg, &MCU_AI_B.Merge2_a);

        /* End of Outputs for SubSystem: '<S62>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S62>/Switch Case' */

    /* Switch: '<S62>/Switch5' incorporates:
     *  Abs: '<S62>/Abs4'
     *  Constant: '<S118>/Constant'
     *  Constant: '<S62>/Constant8'
     *  DataTypeConversion: '<S62>/Data Type Conversion8'
     *  RelationalOperator: '<S118>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_gg) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_gg);
    }

    /* End of Switch: '<S62>/Switch5' */

    /* Sum: '<S63>/Add4' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S140>/Switch2' incorporates:
     *  RelationalOperator: '<S140>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S63>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S140>/Switch' incorporates:
         *  Product: '<S63>/Product8'
         *  RelationalOperator: '<S140>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S140>/Switch' */
    }

    /* End of Switch: '<S140>/Switch2' */

    /* Sum: '<S63>/Add3' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S138>/Switch2' incorporates:
     *  RelationalOperator: '<S138>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S63>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S138>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S138>/Switch' incorporates:
         *  RelationalOperator: '<S138>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S138>/Switch' */
    }

    /* End of Switch: '<S138>/Switch2' */

    /* Sum: '<S63>/Add2' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S137>/Switch2' incorporates:
     *  RelationalOperator: '<S137>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S63>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S137>/Switch' incorporates:
         *  Product: '<S63>/Product6'
         *  RelationalOperator: '<S137>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S137>/Switch' */
    }

    /* End of Switch: '<S137>/Switch2' */

    /* Sum: '<S63>/Add1' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S136>/Switch2' incorporates:
     *  RelationalOperator: '<S136>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S63>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S136>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S136>/Switch' incorporates:
         *  RelationalOperator: '<S136>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S136>/Switch' */
    }

    /* End of Switch: '<S136>/Switch2' */

    /* Sum: '<S63>/Add8' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S143>/Switch2' incorporates:
     *  RelationalOperator: '<S143>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S63>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S143>/Switch' incorporates:
         *  Product: '<S63>/Product4'
         *  RelationalOperator: '<S143>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S143>/Switch' */
    }

    /* End of Switch: '<S143>/Switch2' */

    /* Sum: '<S63>/Add7' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S142>/Switch2' incorporates:
     *  RelationalOperator: '<S142>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S63>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S142>/Switch' incorporates:
         *  Product: '<S63>/Product3'
         *  RelationalOperator: '<S142>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S142>/Switch' */
    }

    /* End of Switch: '<S142>/Switch2' */

    /* Sum: '<S63>/Add6' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S141>/Switch2' incorporates:
     *  RelationalOperator: '<S141>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S63>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S141>/Switch' incorporates:
         *  Product: '<S63>/Product2'
         *  RelationalOperator: '<S141>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S141>/Switch' */
    }

    /* End of Switch: '<S141>/Switch2' */

    /* Sum: '<S63>/Add' incorporates:
     *  DataTypeConversion: '<S62>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S139>/Switch2' incorporates:
     *  RelationalOperator: '<S139>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S63>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S139>/Switch' incorporates:
         *  Product: '<S63>/Product1'
         *  RelationalOperator: '<S139>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S139>/Switch' */
    }

    /* End of Switch: '<S139>/Switch2' */

    /* SwitchCase: '<S63>/Switch Case' incorporates:
     *  Sum: '<S63>/Subtract1'
     *  Sum: '<S63>/Subtract2'
     *  Sum: '<S63>/Subtract3'
     *  Sum: '<S63>/Subtract4'
     *  Sum: '<S63>/Subtract5'
     *  Sum: '<S63>/Subtract7'
     *  Sum: '<S63>/Subtract8'
     *  Sum: '<S63>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[4])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S144>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge_p,
            &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S145>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S146>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S147>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S148>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S149>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S150>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S63>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S151>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge_p, &MCU_AI_B.Merge2_o);

        /* End of Outputs for SubSystem: '<S63>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S63>/Switch Case' */

    /* Switch: '<S63>/Switch5' incorporates:
     *  Abs: '<S63>/Abs4'
     *  Constant: '<S135>/Constant'
     *  Constant: '<S63>/Constant8'
     *  DataTypeConversion: '<S63>/Data Type Conversion8'
     *  RelationalOperator: '<S135>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_p) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_p);
    }

    /* End of Switch: '<S63>/Switch5' */

    /* Sum: '<S64>/Add4' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S157>/Switch2' incorporates:
     *  RelationalOperator: '<S157>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S64>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S157>/Switch' incorporates:
         *  Product: '<S64>/Product8'
         *  RelationalOperator: '<S157>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S157>/Switch' */
    }

    /* End of Switch: '<S157>/Switch2' */

    /* Sum: '<S64>/Add3' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S155>/Switch2' incorporates:
     *  RelationalOperator: '<S155>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S64>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S155>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S155>/Switch' incorporates:
         *  RelationalOperator: '<S155>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S155>/Switch' */
    }

    /* End of Switch: '<S155>/Switch2' */

    /* Sum: '<S64>/Add2' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S154>/Switch2' incorporates:
     *  RelationalOperator: '<S154>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S64>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S154>/Switch' incorporates:
         *  Product: '<S64>/Product6'
         *  RelationalOperator: '<S154>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S154>/Switch' */
    }

    /* End of Switch: '<S154>/Switch2' */

    /* Sum: '<S64>/Add1' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S153>/Switch2' incorporates:
     *  RelationalOperator: '<S153>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S64>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S153>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S153>/Switch' incorporates:
         *  RelationalOperator: '<S153>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S153>/Switch' */
    }

    /* End of Switch: '<S153>/Switch2' */

    /* Sum: '<S64>/Add8' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S160>/Switch2' incorporates:
     *  RelationalOperator: '<S160>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S64>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S160>/Switch' incorporates:
         *  Product: '<S64>/Product4'
         *  RelationalOperator: '<S160>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S160>/Switch' */
    }

    /* End of Switch: '<S160>/Switch2' */

    /* Sum: '<S64>/Add7' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S159>/Switch2' incorporates:
     *  RelationalOperator: '<S159>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S64>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S159>/Switch' incorporates:
         *  Product: '<S64>/Product3'
         *  RelationalOperator: '<S159>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S159>/Switch' */
    }

    /* End of Switch: '<S159>/Switch2' */

    /* Sum: '<S64>/Add6' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S158>/Switch2' incorporates:
     *  RelationalOperator: '<S158>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S64>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S158>/Switch' incorporates:
         *  Product: '<S64>/Product2'
         *  RelationalOperator: '<S158>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S158>/Switch' */
    }

    /* End of Switch: '<S158>/Switch2' */

    /* Sum: '<S64>/Add' incorporates:
     *  DataTypeConversion: '<S63>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S156>/Switch2' incorporates:
     *  RelationalOperator: '<S156>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S64>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S156>/Switch' incorporates:
         *  Product: '<S64>/Product1'
         *  RelationalOperator: '<S156>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S156>/Switch' */
    }

    /* End of Switch: '<S156>/Switch2' */

    /* SwitchCase: '<S64>/Switch Case' incorporates:
     *  Sum: '<S64>/Subtract1'
     *  Sum: '<S64>/Subtract2'
     *  Sum: '<S64>/Subtract3'
     *  Sum: '<S64>/Subtract4'
     *  Sum: '<S64>/Subtract5'
     *  Sum: '<S64>/Subtract7'
     *  Sum: '<S64>/Subtract8'
     *  Sum: '<S64>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[5])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S161>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge_gi,
            &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S162>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge_gi, &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S163>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge_gi, &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S164>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge_gi, &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S165>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge_gi, &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S166>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge_gi, &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S167>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge_gi,
            &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S64>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S168>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge_gi, &MCU_AI_B.Merge2_gk);

        /* End of Outputs for SubSystem: '<S64>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S64>/Switch Case' */

    /* Switch: '<S64>/Switch5' incorporates:
     *  Abs: '<S64>/Abs4'
     *  Constant: '<S152>/Constant'
     *  Constant: '<S64>/Constant8'
     *  DataTypeConversion: '<S64>/Data Type Conversion8'
     *  RelationalOperator: '<S152>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_gi) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_gi);
    }

    /* End of Switch: '<S64>/Switch5' */

    /* Sum: '<S65>/Add4' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Product8_n = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S174>/Switch2' incorporates:
     *  RelationalOperator: '<S174>/LowerRelop1'
     */
    if (MCU_AI_B.Product8_n > MCU_AI_B.Min4_l)
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Min4_l;
    }
    else
    {
        /* Product: '<S65>/Product8' */
        MCU_AI_B.Switch20 = -MCU_AI_B.Min_m;

        /* Switch: '<S174>/Switch' incorporates:
         *  Product: '<S65>/Product8'
         *  RelationalOperator: '<S174>/UpperRelop'
         */
        if (!(MCU_AI_B.Product8_n < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Switch20 = MCU_AI_B.Product8_n;
        }

        /* End of Switch: '<S174>/Switch' */
    }

    /* End of Switch: '<S174>/Switch2' */

    /* Sum: '<S65>/Add3' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Switch17 = MCU_AI_B.Abs7 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S172>/Switch2' incorporates:
     *  RelationalOperator: '<S172>/LowerRelop1'
     */
    if (MCU_AI_B.Switch17 > MCU_AI_B.Min4_f)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        /* Product: '<S65>/Product7' */
        MCU_AI_B.DataTypeConversion15 = (int32_T)-MCU_AI_B.Min_l;

        /* RelationalOperator: '<S172>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch17;

        /* Switch: '<S172>/Switch' incorporates:
         *  RelationalOperator: '<S172>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion15)
        {
            MCU_AI_B.DataTypeConversion15 = MCU_AI_B.i;
        }

        /* End of Switch: '<S172>/Switch' */
    }

    /* End of Switch: '<S172>/Switch2' */

    /* Sum: '<S65>/Add2' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Switch7 = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S171>/Switch2' incorporates:
     *  RelationalOperator: '<S171>/LowerRelop1'
     */
    if (MCU_AI_B.Switch7 > MCU_AI_B.Switch15)
    {
        MCU_AI_B.Switch2_apa = MCU_AI_B.Switch15;
    }
    else
    {
        /* Product: '<S65>/Product6' */
        MCU_AI_B.Switch2_apa = -MCU_AI_B.Min_kv;

        /* Switch: '<S171>/Switch' incorporates:
         *  Product: '<S65>/Product6'
         *  RelationalOperator: '<S171>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch7 < -MCU_AI_B.Min_kv))
        {
            MCU_AI_B.Switch2_apa = MCU_AI_B.Switch7;
        }

        /* End of Switch: '<S171>/Switch' */
    }

    /* End of Switch: '<S171>/Switch2' */

    /* Sum: '<S65>/Add1' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Switch6 = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S170>/Switch2' incorporates:
     *  RelationalOperator: '<S170>/LowerRelop1'
     */
    if (MCU_AI_B.Switch6 > MCU_AI_B.Min4_pm)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        /* Product: '<S65>/Product5' */
        MCU_AI_B.DataTypeConversion4 = (int32_T)-MCU_AI_B.Min_h1;

        /* RelationalOperator: '<S170>/UpperRelop' */
        MCU_AI_B.i = (int32_T)MCU_AI_B.Switch6;

        /* Switch: '<S170>/Switch' incorporates:
         *  RelationalOperator: '<S170>/UpperRelop'
         */
        if (MCU_AI_B.i >= MCU_AI_B.DataTypeConversion4)
        {
            MCU_AI_B.DataTypeConversion4 = MCU_AI_B.i;
        }

        /* End of Switch: '<S170>/Switch' */
    }

    /* End of Switch: '<S170>/Switch2' */

    /* Sum: '<S65>/Add8' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Switch27 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S177>/Switch2' incorporates:
     *  RelationalOperator: '<S177>/LowerRelop1'
     */
    if (MCU_AI_B.Switch27 > MCU_AI_B.Min4_p)
    {
        MCU_AI_B.Switch5 = MCU_AI_B.Min4_p;
    }
    else
    {
        /* Product: '<S65>/Product4' */
        MCU_AI_B.Switch5 = -MCU_AI_B.Min_h;

        /* Switch: '<S177>/Switch' incorporates:
         *  Product: '<S65>/Product4'
         *  RelationalOperator: '<S177>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch27 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Switch5 = MCU_AI_B.Switch27;
        }

        /* End of Switch: '<S177>/Switch' */
    }

    /* End of Switch: '<S177>/Switch2' */

    /* Sum: '<S65>/Add7' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Switch19 = MCU_AI_B.DataTypeConversion9 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S176>/Switch2' incorporates:
     *  RelationalOperator: '<S176>/LowerRelop1'
     */
    if (MCU_AI_B.Switch19 > MCU_AI_B.Switch13)
    {
        MCU_AI_B.Switch14 = MCU_AI_B.Switch13;
    }
    else
    {
        /* Product: '<S65>/Product3' */
        MCU_AI_B.Switch14 = -MCU_AI_B.Min_d;

        /* Switch: '<S176>/Switch' incorporates:
         *  Product: '<S65>/Product3'
         *  RelationalOperator: '<S176>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch19 < -MCU_AI_B.Min_d))
        {
            MCU_AI_B.Switch14 = MCU_AI_B.Switch19;
        }

        /* End of Switch: '<S176>/Switch' */
    }

    /* End of Switch: '<S176>/Switch2' */

    /* Sum: '<S65>/Add6' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.Switch28 = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S175>/Switch2' incorporates:
     *  RelationalOperator: '<S175>/LowerRelop1'
     */
    if (MCU_AI_B.Switch28 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Product8 = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S65>/Product2' */
        MCU_AI_B.Product8 = -MCU_AI_B.Min_k;

        /* Switch: '<S175>/Switch' incorporates:
         *  Product: '<S65>/Product2'
         *  RelationalOperator: '<S175>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch28 < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Product8 = MCU_AI_B.Switch28;
        }

        /* End of Switch: '<S175>/Switch' */
    }

    /* End of Switch: '<S175>/Switch2' */

    /* Sum: '<S65>/Add' incorporates:
     *  DataTypeConversion: '<S64>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion6_b = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S173>/Switch2' incorporates:
     *  RelationalOperator: '<S173>/LowerRelop1'
     */
    if (MCU_AI_B.DataTypeConversion6_b > MCU_AI_B.Min4)
    {
        MCU_AI_B.Switch29 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S65>/Product1' */
        MCU_AI_B.Switch29 = -MCU_AI_B.Min;

        /* Switch: '<S173>/Switch' incorporates:
         *  Product: '<S65>/Product1'
         *  RelationalOperator: '<S173>/UpperRelop'
         */
        if (!(MCU_AI_B.DataTypeConversion6_b < -MCU_AI_B.Min))
        {
            MCU_AI_B.Switch29 = MCU_AI_B.DataTypeConversion6_b;
        }

        /* End of Switch: '<S173>/Switch' */
    }

    /* End of Switch: '<S173>/Switch2' */

    /* SwitchCase: '<S65>/Switch Case' incorporates:
     *  Sum: '<S65>/Subtract1'
     *  Sum: '<S65>/Subtract2'
     *  Sum: '<S65>/Subtract3'
     *  Sum: '<S65>/Subtract4'
     *  Sum: '<S65>/Subtract5'
     *  Sum: '<S65>/Subtract7'
     *  Sum: '<S65>/Subtract8'
     *  Sum: '<S65>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[6])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S178>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion6_b -
            MCU_AI_B.Switch29, MCU_AI_B.Switch29, &MCU_AI_B.Merge_i,
            &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S179>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch28 - MCU_AI_B.Product8,
            MCU_AI_B.Product8, &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S180>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch19 - MCU_AI_B.Switch14,
            MCU_AI_B.Switch14, &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S181>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch27 - MCU_AI_B.Switch5,
            MCU_AI_B.Switch5, &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S182>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch6 - (real32_T)
            MCU_AI_B.DataTypeConversion4, (real32_T)MCU_AI_B.DataTypeConversion4,
            &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S183>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch7 - MCU_AI_B.Switch2_apa,
            MCU_AI_B.Switch2_apa, &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S184>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch17 - (real32_T)
            MCU_AI_B.DataTypeConversion15, (real32_T)
            MCU_AI_B.DataTypeConversion15, &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S65>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S185>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product8_n - MCU_AI_B.Switch20,
            MCU_AI_B.Switch20, &MCU_AI_B.Merge_i, &MCU_AI_B.Merge2_e);

        /* End of Outputs for SubSystem: '<S65>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S65>/Switch Case' */

    /* Switch: '<S65>/Switch5' incorporates:
     *  Abs: '<S65>/Abs4'
     *  Constant: '<S169>/Constant'
     *  Constant: '<S65>/Constant8'
     *  DataTypeConversion: '<S65>/Data Type Conversion8'
     *  RelationalOperator: '<S169>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_i) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_i);
    }

    /* End of Switch: '<S65>/Switch5' */

    /* Sum: '<S66>/Add4' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Switch9_f += (real32_T)rtb_Switch15_e;

    /* Switch: '<S191>/Switch2' incorporates:
     *  RelationalOperator: '<S191>/LowerRelop1'
     */
    if (!(MCU_AI_B.Switch9_f > MCU_AI_B.Min4_l))
    {
        /* Product: '<S66>/Product8' */
        MCU_AI_B.Min4_l = -MCU_AI_B.Min_m;

        /* Switch: '<S191>/Switch' incorporates:
         *  Product: '<S66>/Product8'
         *  RelationalOperator: '<S191>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch9_f < -MCU_AI_B.Min_m))
        {
            MCU_AI_B.Min4_l = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S191>/Switch' */
    }

    /* End of Switch: '<S191>/Switch2' */

    /* Sum: '<S66>/Add3' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Abs7 += (real32_T)rtb_Switch15_e;

    /* Switch: '<S189>/Switch2' incorporates:
     *  RelationalOperator: '<S189>/LowerRelop1'
     */
    if (!(MCU_AI_B.Abs7 > MCU_AI_B.Min4_f))
    {
        /* Product: '<S66>/Product7' */
        MCU_AI_B.Min4_f = -MCU_AI_B.Min_l;

        /* Switch: '<S189>/Switch' incorporates:
         *  Product: '<S66>/Product7'
         *  RelationalOperator: '<S189>/UpperRelop'
         */
        if (!(MCU_AI_B.Abs7 < -MCU_AI_B.Min_l))
        {
            MCU_AI_B.Min4_f = MCU_AI_B.Abs7;
        }

        /* End of Switch: '<S189>/Switch' */
    }

    /* End of Switch: '<S189>/Switch2' */

    /* Sum: '<S66>/Add2' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Min_l = MCU_AI_B.Divide35_o3 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S188>/Switch2' incorporates:
     *  RelationalOperator: '<S188>/LowerRelop1'
     */
    if (!(MCU_AI_B.Min_l > MCU_AI_B.Switch15))
    {
        /* Switch: '<S188>/Switch' incorporates:
         *  Product: '<S66>/Product6'
         *  RelationalOperator: '<S188>/UpperRelop'
         */
        if (MCU_AI_B.Min_l < -MCU_AI_B.Min_kv)
        {
            MCU_AI_B.Switch15 = -MCU_AI_B.Min_kv;
        }
        else
        {
            MCU_AI_B.Switch15 = MCU_AI_B.Min_l;
        }

        /* End of Switch: '<S188>/Switch' */
    }

    /* End of Switch: '<S188>/Switch2' */

    /* Sum: '<S66>/Add1' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Min_kv = MCU_AI_B.Switch12_e + (real32_T)rtb_Switch15_e;

    /* Switch: '<S187>/Switch2' incorporates:
     *  RelationalOperator: '<S187>/LowerRelop1'
     */
    if (!(MCU_AI_B.Min_kv > MCU_AI_B.Min4_pm))
    {
        /* Product: '<S66>/Product5' */
        MCU_AI_B.Min4_pm = -MCU_AI_B.Min_h1;

        /* Switch: '<S187>/Switch' incorporates:
         *  Product: '<S66>/Product5'
         *  RelationalOperator: '<S187>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_kv < -MCU_AI_B.Min_h1))
        {
            MCU_AI_B.Min4_pm = MCU_AI_B.Min_kv;
        }

        /* End of Switch: '<S187>/Switch' */
    }

    /* End of Switch: '<S187>/Switch2' */

    /* Sum: '<S66>/Add8' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Min_h1 = MCU_AI_B.Switch2_kc + (real32_T)rtb_Switch15_e;

    /* Switch: '<S194>/Switch2' incorporates:
     *  RelationalOperator: '<S194>/LowerRelop1'
     */
    if (!(MCU_AI_B.Min_h1 > MCU_AI_B.Min4_p))
    {
        /* Product: '<S66>/Product4' */
        MCU_AI_B.Min4_p = -MCU_AI_B.Min_h;

        /* Switch: '<S194>/Switch' incorporates:
         *  Product: '<S66>/Product4'
         *  RelationalOperator: '<S194>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_h1 < -MCU_AI_B.Min_h))
        {
            MCU_AI_B.Min4_p = MCU_AI_B.Min_h1;
        }

        /* End of Switch: '<S194>/Switch' */
    }

    /* End of Switch: '<S194>/Switch2' */

    /* Sum: '<S66>/Add7' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.DataTypeConversion9 += (real32_T)rtb_Switch15_e;

    /* Switch: '<S193>/Switch2' incorporates:
     *  RelationalOperator: '<S193>/LowerRelop1'
     */
    if (!(MCU_AI_B.DataTypeConversion9 > MCU_AI_B.Switch13))
    {
        /* Switch: '<S193>/Switch' incorporates:
         *  Product: '<S66>/Product3'
         *  RelationalOperator: '<S193>/UpperRelop'
         */
        if (MCU_AI_B.DataTypeConversion9 < -MCU_AI_B.Min_d)
        {
            MCU_AI_B.Switch13 = -MCU_AI_B.Min_d;
        }
        else
        {
            MCU_AI_B.Switch13 = MCU_AI_B.DataTypeConversion9;
        }

        /* End of Switch: '<S193>/Switch' */
    }

    /* End of Switch: '<S193>/Switch2' */

    /* Sum: '<S66>/Add6' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Product7_d += (real32_T)rtb_Switch15_e;

    /* Switch: '<S192>/Switch2' incorporates:
     *  RelationalOperator: '<S192>/LowerRelop1'
     */
    if (!(MCU_AI_B.Product7_d > MCU_AI_B.Min4_i))
    {
        /* Product: '<S66>/Product2' */
        MCU_AI_B.Min4_i = -MCU_AI_B.Min_k;

        /* Switch: '<S192>/Switch' incorporates:
         *  Product: '<S66>/Product2'
         *  RelationalOperator: '<S192>/UpperRelop'
         */
        if (!(MCU_AI_B.Product7_d < -MCU_AI_B.Min_k))
        {
            MCU_AI_B.Min4_i = MCU_AI_B.Product7_d;
        }

        /* End of Switch: '<S192>/Switch' */
    }

    /* End of Switch: '<S192>/Switch2' */

    /* Sum: '<S66>/Add' incorporates:
     *  DataTypeConversion: '<S65>/Data Type Conversion7'
     */
    MCU_AI_B.Square1 += (real32_T)rtb_Switch15_e;

    /* Switch: '<S190>/Switch2' incorporates:
     *  RelationalOperator: '<S190>/LowerRelop1'
     */
    if (!(MCU_AI_B.Square1 > MCU_AI_B.Min4))
    {
        /* Product: '<S66>/Product1' */
        MCU_AI_B.Min4 = -MCU_AI_B.Min;

        /* Switch: '<S190>/Switch' incorporates:
         *  Product: '<S66>/Product1'
         *  RelationalOperator: '<S190>/UpperRelop'
         */
        if (!(MCU_AI_B.Square1 < -MCU_AI_B.Min))
        {
            MCU_AI_B.Min4 = MCU_AI_B.Square1;
        }

        /* End of Switch: '<S190>/Switch' */
    }

    /* End of Switch: '<S190>/Switch2' */

    /* SwitchCase: '<S66>/Switch Case' incorporates:
     *  Sum: '<S66>/Subtract1'
     *  Sum: '<S66>/Subtract2'
     *  Sum: '<S66>/Subtract3'
     *  Sum: '<S66>/Subtract4'
     *  Sum: '<S66>/Subtract5'
     *  Sum: '<S66>/Subtract7'
     *  Sum: '<S66>/Subtract8'
     *  Sum: '<S66>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2[7])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S195>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Square1 - MCU_AI_B.Min4,
            MCU_AI_B.Min4, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S196>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product7_d - MCU_AI_B.Min4_i,
            MCU_AI_B.Min4_i, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S197>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.DataTypeConversion9 -
            MCU_AI_B.Switch13, MCU_AI_B.Switch13, &MCU_AI_B.Min,
            &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S198>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_h1 - MCU_AI_B.Min4_p,
            MCU_AI_B.Min4_p, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem3' */
        break;

      case 5:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem4' incorporates:
         *  ActionPort: '<S199>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_kv - MCU_AI_B.Min4_pm,
            MCU_AI_B.Min4_pm, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem4' */
        break;

      case 6:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem5' incorporates:
         *  ActionPort: '<S200>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_l - MCU_AI_B.Switch15,
            MCU_AI_B.Switch15, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem5' */
        break;

      case 7:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem6' incorporates:
         *  ActionPort: '<S201>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Abs7 - MCU_AI_B.Min4_f,
            MCU_AI_B.Min4_f, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem6' */
        break;

      case 8:
        /* Outputs for IfAction SubSystem: '<S66>/Switch Case Action Subsystem7' incorporates:
         *  ActionPort: '<S202>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch9_f - MCU_AI_B.Min4_l,
            MCU_AI_B.Min4_l, &MCU_AI_B.Min, &MCU_AI_B.Merge2_gm);

        /* End of Outputs for SubSystem: '<S66>/Switch Case Action Subsystem7' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S66>/Switch Case' */

    /* SignalConversion generated from: '<S29>/Selector' */
    MCU_AI_B.TmpSignalConversionAtSelect[0] = MCU_AI_B.Merge2;
    MCU_AI_B.TmpSignalConversionAtSelect[1] = MCU_AI_B.Merge2_f;
    MCU_AI_B.TmpSignalConversionAtSelect[2] = MCU_AI_B.Merge2_g;
    MCU_AI_B.TmpSignalConversionAtSelect[3] = MCU_AI_B.Merge2_a;
    MCU_AI_B.TmpSignalConversionAtSelect[4] = MCU_AI_B.Merge2_o;
    MCU_AI_B.TmpSignalConversionAtSelect[5] = MCU_AI_B.Merge2_gk;
    MCU_AI_B.TmpSignalConversionAtSelect[6] = MCU_AI_B.Merge2_e;
    MCU_AI_B.TmpSignalConversionAtSelect[7] = MCU_AI_B.Merge2_gm;

    /* S-Function (sdspsrt2): '<S29>/Sort1' */
    /* DSP System Toolbox Sort (sdspsrt2) - '<S29>/Sort1' */
    {
        const uint32_T *uQS = MCU_AI_B.Sort_o2;
        uint32_T *index = MCU_AI_B.Sort1_o2;
        uint32_T *y = MCU_AI_B.Sort1_o1;
        uint32_T j_SORT;
        for (j_SORT=0; j_SORT<8; j_SORT++)
        {
            index[j_SORT] = j_SORT;
        }

        MWDSP_Sort_Qk_Idx_U32(uQS, index, 0, 7);
        for (j_SORT=0; j_SORT<8; j_SORT++)
        {
            y[j_SORT] = uQS[index[j_SORT]];
            index[j_SORT] += 1;        /* Convert to MATLAB 1-based indexing */
        }
    }

    /* SignalConversion generated from: '<S29>/Selector' */
    MCU_AI_B.fv[0] = MCU_AI_B.Merge2;
    MCU_AI_B.fv[1] = MCU_AI_B.Merge2_f;
    MCU_AI_B.fv[2] = MCU_AI_B.Merge2_g;
    MCU_AI_B.fv[3] = MCU_AI_B.Merge2_a;
    MCU_AI_B.fv[4] = MCU_AI_B.Merge2_o;
    MCU_AI_B.fv[5] = MCU_AI_B.Merge2_gk;
    MCU_AI_B.fv[6] = MCU_AI_B.Merge2_e;
    MCU_AI_B.fv[7] = MCU_AI_B.Merge2_gm;

    /* Selector: '<S29>/Selector' */
    for (MCU_AI_B.i = 0; MCU_AI_B.i < 8; MCU_AI_B.i++)
    {
        MCU_AI_B.Selector[MCU_AI_B.i] = MCU_AI_B.fv[(int32_T)
            MCU_AI_B.Sort1_o2[MCU_AI_B.i] - 1];
    }

    /* MinMax: '<S2>/Min' incorporates:
     *  Inport: '<Root>/target_h_soc'
     *  Inport: '<Root>/target_l_soc'
     */
    if (target_h_soc <= target_l_soc)
    {
        rtb_Min_ju = target_h_soc;
    }
    else
    {
        rtb_Min_ju = target_l_soc;
    }

    /* End of MinMax: '<S2>/Min' */

    /* Switch: '<S2>/Switch4' incorporates:
     *  Constant: '<S2>/Constant16'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs1_rated_power'
     */
    if (bus2_or_bus1 > 0)
    {
        MCU_AI_B.Switch4_d = 0U;
    }
    else
    {
        MCU_AI_B.Switch4_d = pcs1_rated_power;
    }

    /* End of Switch: '<S2>/Switch4' */

    /* MinMax: '<S2>/Max' incorporates:
     *  Inport: '<Root>/target_h_soc'
     *  Inport: '<Root>/target_l_soc'
     */
    if (target_h_soc >= target_l_soc)
    {
        rtb_Max = target_h_soc;
    }
    else
    {
        rtb_Max = target_l_soc;
    }

    /* End of MinMax: '<S2>/Max' */

    /* Switch: '<S269>/Switch12' incorporates:
     *  Constant: '<S269>/Constant37'
     *  Inport: '<Root>/bms1_max_charge_power'
     *  Inport: '<Root>/bms1_soc'
     *  RelationalOperator: '<S269>/GreaterThanOrEqual'
     */
    if (bms1_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms1_max_charge_power;
    }

    /* MinMax: '<S269>/Min' incorporates:
     *  Switch: '<S269>/Switch12'
     */
    MCU_AI_B.Min4_p = fminf((real32_T)MCU_AI_B.Switch4_d, (real32_T)MCU_AI_B.i);

    /* Switch: '<S2>/Switch3' incorporates:
     *  Constant: '<S2>/Constant17'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs2_rated_power'
     */
    if (bus2_or_bus1 > 0)
    {
        MCU_AI_B.Switch3_g = 0U;
    }
    else
    {
        MCU_AI_B.Switch3_g = pcs2_rated_power;
    }

    /* End of Switch: '<S2>/Switch3' */

    /* Switch: '<S270>/Switch12' incorporates:
     *  Constant: '<S270>/Constant37'
     *  Inport: '<Root>/bms2_max_charge_power'
     *  Inport: '<Root>/bms2_soc'
     *  RelationalOperator: '<S270>/GreaterThanOrEqual'
     */
    if (bms2_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms2_max_charge_power;
    }

    /* MinMax: '<S270>/Min' incorporates:
     *  Switch: '<S270>/Switch12'
     */
    MCU_AI_B.Min4_pm = fminf((real32_T)MCU_AI_B.Switch3_g, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S265>/Compare' incorporates:
     *  Constant: '<S265>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_bq = (module_num >= 2);

    /* Switch: '<S2>/Switch2' incorporates:
     *  Constant: '<S2>/Constant18'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs3_rated_power'
     */
    if (bus2_or_bus1 > 0)
    {
        MCU_AI_B.Switch2_p = 0U;
    }
    else
    {
        MCU_AI_B.Switch2_p = pcs3_rated_power;
    }

    /* End of Switch: '<S2>/Switch2' */

    /* Switch: '<S271>/Switch12' incorporates:
     *  Constant: '<S271>/Constant37'
     *  Inport: '<Root>/bms3_max_charge_power'
     *  Inport: '<Root>/bms3_soc'
     *  RelationalOperator: '<S271>/GreaterThanOrEqual'
     */
    if (bms3_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms3_max_charge_power;
    }

    /* MinMax: '<S271>/Min' incorporates:
     *  Switch: '<S271>/Switch12'
     */
    MCU_AI_B.Min4_f = fminf((real32_T)MCU_AI_B.Switch2_p, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S266>/Compare' incorporates:
     *  Constant: '<S266>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_lc = (module_num >= 3);

    /* Switch: '<S2>/Switch1' incorporates:
     *  Constant: '<S2>/Constant19'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/pcs4_rated_power'
     */
    if (bus2_or_bus1 > 0)
    {
        MCU_AI_B.Switch1_k = 0U;
    }
    else
    {
        MCU_AI_B.Switch1_k = pcs4_rated_power;
    }

    /* End of Switch: '<S2>/Switch1' */

    /* Switch: '<S272>/Switch12' incorporates:
     *  Constant: '<S272>/Constant37'
     *  Inport: '<Root>/bms4_max_charge_power'
     *  Inport: '<Root>/bms4_soc'
     *  RelationalOperator: '<S272>/GreaterThanOrEqual'
     */
    if (bms4_soc >= rtb_Max)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms4_max_charge_power;
    }

    /* MinMax: '<S272>/Min' incorporates:
     *  Switch: '<S272>/Switch12'
     */
    MCU_AI_B.Min4_l = fminf((real32_T)MCU_AI_B.Switch1_k, (real32_T)MCU_AI_B.i);

    /* RelationalOperator: '<S267>/Compare' incorporates:
     *  Constant: '<S267>/Constant'
     *  Inport: '<Root>/module_num'
     */
    rtb_Compare_h2 = (module_num >= 4);

    /* Switch: '<S2>/Switch5' incorporates:
     *  Constant: '<S2>/Constant1'
     *  Constant: '<S2>/Constant2'
     *  DataTypeConversion: '<S2>/Data Type Conversion4'
     *  DataTypeConversion: '<S2>/Data Type Conversion5'
     *  Inport: '<Root>/bus2_or_bus1'
     *  Inport: '<Root>/mv_max_power'
     *  Inport: '<Root>/mv_power'
     *  Switch: '<S2>/Switch6'
     */
    if (bus2_or_bus1 > 0)
    {
        MCU_AI_B.Switch9_f = 0.0F;
        MCU_AI_B.Product7_d = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch9_f = mv_max_power;
        MCU_AI_B.Product7_d = mv_power;
    }

    /* End of Switch: '<S2>/Switch5' */

    /* Switch: '<S273>/Switch5' incorporates:
     *  Constant: '<S273>/Constant7'
     *  Inport: '<Root>/bms1_max_discharge_power'
     *  Inport: '<Root>/bms1_soc'
     *  RelationalOperator: '<S273>/LessThanOrEqual'
     */
    if (bms1_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms1_max_discharge_power;
    }

    /* MinMax: '<S273>/Min4' incorporates:
     *  Switch: '<S273>/Switch5'
     */
    MCU_AI_B.DataTypeConversion9 = fminf((real32_T)MCU_AI_B.Switch4_d, (real32_T)
        MCU_AI_B.i);

    /* Switch: '<S274>/Switch5' incorporates:
     *  Constant: '<S274>/Constant7'
     *  Inport: '<Root>/bms2_max_discharge_power'
     *  Inport: '<Root>/bms2_soc'
     *  RelationalOperator: '<S274>/LessThanOrEqual'
     */
    if (bms2_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms2_max_discharge_power;
    }

    /* MinMax: '<S274>/Min4' incorporates:
     *  Switch: '<S274>/Switch5'
     */
    MCU_AI_B.Min4 = fminf((real32_T)MCU_AI_B.Switch3_g, (real32_T)MCU_AI_B.i);

    /* Switch: '<S276>/Switch5' incorporates:
     *  Constant: '<S276>/Constant7'
     *  Inport: '<Root>/bms4_max_discharge_power'
     *  Inport: '<Root>/bms4_soc'
     *  RelationalOperator: '<S276>/LessThanOrEqual'
     */
    if (bms4_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms4_max_discharge_power;
    }

    /* MinMax: '<S276>/Min4' incorporates:
     *  Switch: '<S276>/Switch5'
     */
    MCU_AI_B.Min4_i = fminf((real32_T)MCU_AI_B.Switch1_k, (real32_T)MCU_AI_B.i);

    /* Switch: '<S277>/Switch3' incorporates:
     *  Constant: '<S277>/Constant42'
     */
    if (rtb_Compare_bq)
    {
        MCU_AI_B.i = (int32_T)MCU_AI_B.Min4_pm;
    }
    else
    {
        MCU_AI_B.i = 0;
    }

    /* Switch: '<S277>/Switch4' incorporates:
     *  Constant: '<S277>/Constant43'
     */
    if (rtb_Compare_lc)
    {
        MCU_AI_B.DataTypeConversion15 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        MCU_AI_B.DataTypeConversion15 = 0;
    }

    /* Switch: '<S277>/Switch5' incorporates:
     *  Constant: '<S277>/Constant44'
     */
    if (rtb_Compare_h2)
    {
        MCU_AI_B.DataTypeConversion4 = (int32_T)MCU_AI_B.Min4_l;
    }
    else
    {
        MCU_AI_B.DataTypeConversion4 = 0;
    }

    /* Switch: '<S277>/Switch' incorporates:
     *  Constant: '<S277>/Constant39'
     */
    if (rtb_Compare_bq)
    {
        tmp = (int32_T)MCU_AI_B.Min4;
    }
    else
    {
        tmp = 0;
    }

    /* Switch: '<S277>/Switch1' incorporates:
     *  Constant: '<S277>/Constant40'
     */
    if (rtb_Compare_lc)
    {
        tmp_0 = (int32_T)MCU_AI_B.Min4_f;
    }
    else
    {
        tmp_0 = 0;
    }

    /* Switch: '<S277>/Switch2' incorporates:
     *  Constant: '<S277>/Constant41'
     */
    if (rtb_Compare_h2)
    {
        tmp_1 = (int32_T)MCU_AI_B.Min4_i;
    }
    else
    {
        tmp_1 = 0;
    }

    /* MinMax: '<S277>/Max' incorporates:
     *  MinMax: '<S277>/Min'
     *  Product: '<S277>/Product'
     *  Product: '<S277>/Product1'
     *  Sum: '<S277>/Add1'
     *  Sum: '<S277>/Add2'
     *  Switch: '<S277>/Switch'
     *  Switch: '<S277>/Switch1'
     *  Switch: '<S277>/Switch2'
     *  Switch: '<S277>/Switch3'
     *  Switch: '<S277>/Switch4'
     *  Switch: '<S277>/Switch5'
     */
    MCU_AI_B.Abs7 = fmaxf(fmaxf(-(((MCU_AI_B.Min4_p + (real32_T)MCU_AI_B.i) +
        (real32_T)MCU_AI_B.DataTypeConversion15) + (real32_T)
                            MCU_AI_B.DataTypeConversion4), -MCU_AI_B.Switch9_f),
                          fminf(fminf(MCU_AI_B.Product7_d, MCU_AI_B.Switch9_f),
                           ((MCU_AI_B.DataTypeConversion9 + (real32_T)tmp) +
                            (real32_T)tmp_0) + (real32_T)tmp_1));

    /* RelationalOperator: '<S268>/Compare' incorporates:
     *  Constant: '<S268>/Constant'
     */
    rtb_Compare_mi = (MCU_AI_B.Abs7 >= 0.0F);

    /* Switch: '<S282>/Switch12' incorporates:
     *  DataTypeConversion: '<S2>/Data Type Conversion10'
     *  Inport: '<Root>/bms1_soc'
     *  Sum: '<S282>/Subtract'
     *  Sum: '<S282>/Subtract4'
     */
    if (rtb_Compare_mi)
    {
        MCU_AI_B.Switch9_f = (real32_T)bms1_soc - (real32_T)rtb_Min_ju;
    }
    else
    {
        MCU_AI_B.Switch9_f = (real32_T)rtb_Max - (real32_T)bms1_soc;
    }

    /* End of Switch: '<S282>/Switch12' */

    /* Switch: '<S282>/Switch5' incorporates:
     *  Constant: '<S282>/Constant1'
     *  Constant: '<S359>/Constant'
     *  RelationalOperator: '<S359>/Compare'
     */
    if (MCU_AI_B.Switch9_f <= 0.0F)
    {
        MCU_AI_B.Switch9_f = 0.0F;
    }

    /* End of Switch: '<S282>/Switch5' */

    /* Switch: '<S282>/Switch13' incorporates:
     *  Constant: '<S282>/Constant52'
     */
    if (rtb_Compare_bq)
    {
        /* Switch: '<S282>/Switch9' incorporates:
         *  DataTypeConversion: '<S2>/Data Type Conversion11'
         *  Inport: '<Root>/bms2_soc'
         *  Sum: '<S282>/Subtract1'
         *  Sum: '<S282>/Subtract5'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Product7_d = (real32_T)bms2_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Product7_d = (real32_T)rtb_Max - (real32_T)bms2_soc;
        }

        /* End of Switch: '<S282>/Switch9' */

        /* Switch: '<S282>/Switch1' incorporates:
         *  Constant: '<S282>/Constant2'
         *  Constant: '<S360>/Constant'
         *  RelationalOperator: '<S360>/Compare'
         */
        if (MCU_AI_B.Product7_d <= 0.0F)
        {
            MCU_AI_B.Square1 = 0.0F;
        }
        else
        {
            MCU_AI_B.Square1 = MCU_AI_B.Product7_d;
        }

        /* End of Switch: '<S282>/Switch1' */
    }
    else
    {
        MCU_AI_B.Square1 = 0.0F;
    }

    /* End of Switch: '<S282>/Switch13' */

    /* Switch: '<S282>/Switch14' incorporates:
     *  Constant: '<S282>/Constant53'
     */
    if (rtb_Compare_lc)
    {
        /* Switch: '<S282>/Switch10' incorporates:
         *  DataTypeConversion: '<S2>/Data Type Conversion12'
         *  Inport: '<Root>/bms3_soc'
         *  Sum: '<S282>/Subtract2'
         *  Sum: '<S282>/Subtract6'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Product7_d = (real32_T)bms3_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Product7_d = (real32_T)rtb_Max - (real32_T)bms3_soc;
        }

        /* End of Switch: '<S282>/Switch10' */

        /* Switch: '<S282>/Switch2' incorporates:
         *  Constant: '<S282>/Constant3'
         *  Constant: '<S361>/Constant'
         *  RelationalOperator: '<S361>/Compare'
         */
        if (MCU_AI_B.Product7_d <= 0.0F)
        {
            MCU_AI_B.Product8_n = 0.0F;
        }
        else
        {
            MCU_AI_B.Product8_n = MCU_AI_B.Product7_d;
        }

        /* End of Switch: '<S282>/Switch2' */
    }
    else
    {
        MCU_AI_B.Product8_n = 0.0F;
    }

    /* End of Switch: '<S282>/Switch14' */

    /* Switch: '<S282>/Switch15' incorporates:
     *  Constant: '<S282>/Constant54'
     */
    if (rtb_Compare_h2)
    {
        /* Switch: '<S282>/Switch11' incorporates:
         *  DataTypeConversion: '<S2>/Data Type Conversion13'
         *  Inport: '<Root>/bms4_soc'
         *  Sum: '<S282>/Subtract3'
         *  Sum: '<S282>/Subtract7'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Product7_d = (real32_T)bms4_soc - (real32_T)rtb_Min_ju;
        }
        else
        {
            MCU_AI_B.Product7_d = (real32_T)rtb_Max - (real32_T)bms4_soc;
        }

        /* End of Switch: '<S282>/Switch11' */

        /* Switch: '<S282>/Switch3' incorporates:
         *  Constant: '<S282>/Constant4'
         *  Constant: '<S362>/Constant'
         *  RelationalOperator: '<S362>/Compare'
         */
        if (MCU_AI_B.Product7_d <= 0.0F)
        {
            MCU_AI_B.Switch12_e = 0.0F;
        }
        else
        {
            MCU_AI_B.Switch12_e = MCU_AI_B.Product7_d;
        }

        /* End of Switch: '<S282>/Switch3' */
    }
    else
    {
        MCU_AI_B.Switch12_e = 0.0F;
    }

    /* End of Switch: '<S282>/Switch15' */

    /* SignalConversion generated from: '<S279>/Sort' */
    MCU_AI_B.TmpSignalConversionAtSele_j[0] = MCU_AI_B.Switch9_f;

    /* Switch: '<S279>/Switch6' */
    if (rtb_Compare_bq)
    {
        /* SignalConversion generated from: '<S279>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSele_j[1] = MCU_AI_B.Square1;
    }
    else
    {
        /* SignalConversion generated from: '<S279>/Sort' incorporates:
         *  Constant: '<S279>/Constant45'
         */
        MCU_AI_B.TmpSignalConversionAtSele_j[1] = -1.0F;
    }

    /* End of Switch: '<S279>/Switch6' */

    /* Switch: '<S279>/Switch7' */
    if (rtb_Compare_lc)
    {
        /* SignalConversion generated from: '<S279>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSele_j[2] = MCU_AI_B.Product8_n;
    }
    else
    {
        /* SignalConversion generated from: '<S279>/Sort' incorporates:
         *  Constant: '<S279>/Constant46'
         */
        MCU_AI_B.TmpSignalConversionAtSele_j[2] = -1.0F;
    }

    /* End of Switch: '<S279>/Switch7' */

    /* Switch: '<S279>/Switch8' */
    if (rtb_Compare_h2)
    {
        /* SignalConversion generated from: '<S279>/Sort' */
        MCU_AI_B.TmpSignalConversionAtSele_j[3] = MCU_AI_B.Switch12_e;
    }
    else
    {
        /* SignalConversion generated from: '<S279>/Sort' incorporates:
         *  Constant: '<S279>/Constant47'
         */
        MCU_AI_B.TmpSignalConversionAtSele_j[3] = -1.0F;
    }

    /* End of Switch: '<S279>/Switch8' */

    /* S-Function (sdspsrt2): '<S279>/Sort' */
    /* DSP System Toolbox Sort (sdspsrt2) - '<S279>/Sort' */
    {
        const real32_T *uQS = MCU_AI_B.TmpSignalConversionAtSele_j;
        uint32_T *index = MCU_AI_B.Sort_o2_o;
        real32_T *y = MCU_AI_B.Sort_o1_p;
        uint32_T j_SORT;
        for (j_SORT=0; j_SORT<4; j_SORT++)
        {
            index[j_SORT] = j_SORT;
        }

        MWDSP_Sort_Qk_Idx_R(uQS, index, 0, 3);

        {
            uint32_T range = (4 >> 1);
            uint32_T k_SORT;
            for (k_SORT=0; k_SORT<range; k_SORT++)
            {
                uint32_T tmp_IDX = index[k_SORT];
                index[k_SORT] = index[3-k_SORT];
                index[3-k_SORT] = tmp_IDX;
            }
        }

        for (j_SORT=0; j_SORT<4; j_SORT++)
        {
            y[j_SORT] = uQS[index[j_SORT]];
            index[j_SORT] += 1;        /* Convert to MATLAB 1-based indexing */
        }
    }

    /* Switch: '<S278>/Switch14' incorporates:
     *  Constant: '<S278>/Constant3'
     *  Constant: '<S278>/Constant4'
     *  Constant: '<S281>/Constant10'
     *  Constant: '<S281>/Constant9'
     *  Switch: '<S281>/Switch6'
     */
    if (rtb_Compare_mi)
    {
        MCU_AI_B.Product7_d = 1.0F;
        MCU_AI_B.Switch2_kc = 1.0F;
    }
    else
    {
        MCU_AI_B.Product7_d = -1.0F;
        MCU_AI_B.Switch2_kc = -1.0F;
    }

    /* End of Switch: '<S278>/Switch14' */

    /* Sum: '<S282>/Add3' */
    MCU_AI_B.Divide35_o3 = ((MCU_AI_B.Switch9_f + MCU_AI_B.Square1) +
                            MCU_AI_B.Product8_n) + MCU_AI_B.Switch12_e;

    /* Switch: '<S282>/Switch8' incorporates:
     *  Constant: '<S282>/Constant8'
     *  Constant: '<S358>/Constant'
     *  RelationalOperator: '<S358>/Compare'
     */
    if (MCU_AI_B.Divide35_o3 <= 0.1F)
    {
        MCU_AI_B.Divide35_o3 = 0.4F;
    }

    /* End of Switch: '<S282>/Switch8' */

    /* Switch: '<S281>/Switch17' incorporates:
     *  Constant: '<S281>/Constant55'
     *  Constant: '<S346>/Constant'
     *  RelationalOperator: '<S346>/Compare'
     */
    if (MCU_AI_B.Divide35_o3 <= 1.0F)
    {
        MCU_AI_B.Switch20 = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch20 = MCU_AI_B.Abs7;
    }

    /* End of Switch: '<S281>/Switch17' */

    /* Product: '<S281>/Divide38' */
    MCU_AI_B.Min = MCU_AI_B.Switch12_e * MCU_AI_B.Switch20 /
        MCU_AI_B.Divide35_o3;

    /* Switch: '<S357>/Switch2' incorporates:
     *  Product: '<S281>/Product4'
     *  RelationalOperator: '<S357>/LowerRelop1'
     *  RelationalOperator: '<S357>/UpperRelop'
     *  Switch: '<S357>/Switch'
     */
    if (MCU_AI_B.Min > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Switch12_e = MCU_AI_B.Min4_i;
    }
    else if (MCU_AI_B.Min < -MCU_AI_B.Min4_l)
    {
        /* Switch: '<S357>/Switch' incorporates:
         *  Product: '<S281>/Product4'
         */
        MCU_AI_B.Switch12_e = -MCU_AI_B.Min4_l;
    }
    else
    {
        MCU_AI_B.Switch12_e = MCU_AI_B.Min;
    }

    /* End of Switch: '<S357>/Switch2' */

    /* Switch: '<S281>/Switch10' incorporates:
     *  Constant: '<S281>/Constant14'
     *  Constant: '<S353>/Constant'
     *  Product: '<S281>/Product8'
     *  RelationalOperator: '<S353>/Compare'
     */
    if (MCU_AI_B.Switch2_kc * MCU_AI_B.Switch12_e < 0.0F)
    {
        MCU_AI_B.Min = 0.0F;
    }
    else
    {
        MCU_AI_B.Min = MCU_AI_B.Switch12_e;
    }

    /* End of Switch: '<S281>/Switch10' */

    /* Switch: '<S281>/Switch4' incorporates:
     *  Abs: '<S281>/Abs3'
     *  Constant: '<S347>/Constant'
     *  RelationalOperator: '<S347>/Compare'
     */
    if (fabsf(MCU_AI_B.Min) <= 0.1F)
    {
        /* DataTypeConversion: '<S281>/Data Type Conversion6' */
        MCU_AI_B.Min_k = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S281>/Data Type Conversion6' incorporates:
         *  DataTypeConversion: '<S281>/Data Type Conversion3'
         */
        MCU_AI_B.Min_k = (int16_T)MCU_AI_B.Min;
    }

    /* End of Switch: '<S281>/Switch4' */

    /* Product: '<S281>/Divide35' */
    MCU_AI_B.Switch9_f = MCU_AI_B.Switch9_f * MCU_AI_B.Switch20 /
        MCU_AI_B.Divide35_o3;

    /* Switch: '<S354>/Switch2' incorporates:
     *  Product: '<S281>/Product1'
     *  RelationalOperator: '<S354>/LowerRelop1'
     *  RelationalOperator: '<S354>/UpperRelop'
     *  Switch: '<S354>/Switch'
     */
    if (MCU_AI_B.Switch9_f > MCU_AI_B.DataTypeConversion9)
    {
        MCU_AI_B.Switch9_f = MCU_AI_B.DataTypeConversion9;
    }
    else if (MCU_AI_B.Switch9_f < -MCU_AI_B.Min4_p)
    {
        /* Switch: '<S354>/Switch' incorporates:
         *  Product: '<S281>/Product1'
         */
        MCU_AI_B.Switch9_f = -MCU_AI_B.Min4_p;
    }

    /* End of Switch: '<S354>/Switch2' */

    /* Switch: '<S281>/Switch7' incorporates:
     *  Constant: '<S281>/Constant11'
     *  Constant: '<S350>/Constant'
     *  Product: '<S281>/Product10'
     *  RelationalOperator: '<S350>/Compare'
     */
    if (MCU_AI_B.Switch2_kc * MCU_AI_B.Switch9_f < 0.0F)
    {
        MCU_AI_B.Switch9_f = 0.0F;
    }

    /* End of Switch: '<S281>/Switch7' */

    /* Switch: '<S281>/Switch1' incorporates:
     *  Abs: '<S281>/Abs'
     *  Constant: '<S281>/Constant4'
     *  Constant: '<S348>/Constant'
     *  DataTypeConversion: '<S281>/Data Type Conversion11'
     *  RelationalOperator: '<S348>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch9_f) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S281>/Switch1' */

    /* DataTypeConversion: '<S281>/Data Type Conversion' */
    MCU_AI_B.Switch9_f = rtb_Switch15_e;

    /* Switch: '<S278>/Switch12' incorporates:
     *  Product: '<S278>/Product10'
     */
    if (rtb_Compare_mi)
    {
        MCU_AI_B.Min_m = MCU_AI_B.DataTypeConversion9;
    }
    else
    {
        MCU_AI_B.Min_m = -MCU_AI_B.Min4_p;
    }

    /* Sum: '<S278>/Subtract1' incorporates:
     *  DataTypeConversion: '<S281>/Data Type Conversion'
     *  Switch: '<S278>/Switch12'
     */
    MCU_AI_B.Min_d = MCU_AI_B.Min_m - (real32_T)rtb_Switch15_e;

    /* Product: '<S281>/Divide36' */
    MCU_AI_B.Square1 = MCU_AI_B.Square1 * MCU_AI_B.Switch20 /
        MCU_AI_B.Divide35_o3;

    /* Switch: '<S355>/Switch2' incorporates:
     *  Product: '<S281>/Product2'
     *  RelationalOperator: '<S355>/LowerRelop1'
     *  RelationalOperator: '<S355>/UpperRelop'
     *  Switch: '<S355>/Switch'
     */
    if (MCU_AI_B.Square1 > MCU_AI_B.Min4)
    {
        MCU_AI_B.Square1 = MCU_AI_B.Min4;
    }
    else if (MCU_AI_B.Square1 < -MCU_AI_B.Min4_pm)
    {
        /* Switch: '<S355>/Switch' incorporates:
         *  Product: '<S281>/Product2'
         */
        MCU_AI_B.Square1 = -MCU_AI_B.Min4_pm;
    }

    /* End of Switch: '<S355>/Switch2' */

    /* Switch: '<S281>/Switch8' incorporates:
     *  Constant: '<S281>/Constant12'
     *  Constant: '<S351>/Constant'
     *  Product: '<S281>/Product6'
     *  RelationalOperator: '<S351>/Compare'
     */
    if (MCU_AI_B.Switch2_kc * MCU_AI_B.Square1 < 0.0F)
    {
        MCU_AI_B.Square1 = 0.0F;
    }

    /* End of Switch: '<S281>/Switch8' */

    /* Switch: '<S281>/Switch2' incorporates:
     *  Abs: '<S281>/Abs1'
     *  Constant: '<S281>/Constant5'
     *  Constant: '<S344>/Constant'
     *  DataTypeConversion: '<S281>/Data Type Conversion1'
     *  RelationalOperator: '<S344>/Compare'
     */
    if (fabsf(MCU_AI_B.Square1) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Square1;
    }

    /* End of Switch: '<S281>/Switch2' */

    /* DataTypeConversion: '<S281>/Data Type Conversion4' */
    MCU_AI_B.Min_h = rtb_Switch15_e;

    /* Switch: '<S278>/Switch27' incorporates:
     *  Constant: '<S278>/Constant54'
     *  DataTypeConversion: '<S281>/Data Type Conversion4'
     *  Sum: '<S278>/Subtract3'
     *  Switch: '<S278>/Switch9'
     */
    if (rtb_Compare_bq)
    {
        /* Switch: '<S278>/Switch9' incorporates:
         *  Product: '<S278>/Product11'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Min_m = MCU_AI_B.Min4;
        }
        else
        {
            MCU_AI_B.Min_m = -MCU_AI_B.Min4_pm;
        }

        MCU_AI_B.Min_h1 = MCU_AI_B.Min_m - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Min_h1 = 0.0F;
    }

    /* End of Switch: '<S278>/Switch27' */

    /* Switch: '<S275>/Switch5' incorporates:
     *  Constant: '<S275>/Constant7'
     *  Inport: '<Root>/bms3_max_discharge_power'
     *  Inport: '<Root>/bms3_soc'
     *  RelationalOperator: '<S275>/LessThanOrEqual'
     */
    if (bms3_soc <= rtb_Min_ju)
    {
        MCU_AI_B.i = 0;
    }
    else
    {
        MCU_AI_B.i = bms3_max_discharge_power;
    }

    /* MinMax: '<S275>/Min4' incorporates:
     *  Switch: '<S275>/Switch5'
     */
    MCU_AI_B.Min = fminf((real32_T)MCU_AI_B.Switch2_p, (real32_T)MCU_AI_B.i);

    /* Product: '<S281>/Divide37' */
    MCU_AI_B.Square1 = MCU_AI_B.Product8_n * MCU_AI_B.Switch20 /
        MCU_AI_B.Divide35_o3;

    /* Switch: '<S356>/Switch2' incorporates:
     *  Product: '<S281>/Product3'
     *  RelationalOperator: '<S356>/LowerRelop1'
     *  RelationalOperator: '<S356>/UpperRelop'
     *  Switch: '<S356>/Switch'
     */
    if (MCU_AI_B.Square1 > MCU_AI_B.Min)
    {
        MCU_AI_B.Square1 = MCU_AI_B.Min;
    }
    else if (MCU_AI_B.Square1 < -MCU_AI_B.Min4_f)
    {
        /* Switch: '<S356>/Switch' incorporates:
         *  Product: '<S281>/Product3'
         */
        MCU_AI_B.Square1 = -MCU_AI_B.Min4_f;
    }

    /* End of Switch: '<S356>/Switch2' */

    /* Switch: '<S281>/Switch9' incorporates:
     *  Constant: '<S281>/Constant13'
     *  Constant: '<S352>/Constant'
     *  Product: '<S281>/Product7'
     *  RelationalOperator: '<S352>/Compare'
     */
    if (MCU_AI_B.Switch2_kc * MCU_AI_B.Square1 < 0.0F)
    {
        MCU_AI_B.Square1 = 0.0F;
    }

    /* End of Switch: '<S281>/Switch9' */

    /* Switch: '<S281>/Switch3' incorporates:
     *  Abs: '<S281>/Abs2'
     *  Constant: '<S281>/Constant6'
     *  Constant: '<S345>/Constant'
     *  DataTypeConversion: '<S281>/Data Type Conversion2'
     *  RelationalOperator: '<S345>/Compare'
     */
    if (fabsf(MCU_AI_B.Square1) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Square1;
    }

    /* End of Switch: '<S281>/Switch3' */

    /* DataTypeConversion: '<S281>/Data Type Conversion5' */
    MCU_AI_B.Min_kv = rtb_Switch15_e;

    /* Switch: '<S278>/Switch28' incorporates:
     *  Constant: '<S278>/Constant27'
     *  DataTypeConversion: '<S281>/Data Type Conversion5'
     *  Sum: '<S278>/Subtract5'
     *  Switch: '<S278>/Switch10'
     */
    if (rtb_Compare_lc)
    {
        /* Switch: '<S278>/Switch10' incorporates:
         *  Product: '<S278>/Product12'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Min_m = MCU_AI_B.Min;
        }
        else
        {
            MCU_AI_B.Min_m = -MCU_AI_B.Min4_f;
        }

        MCU_AI_B.Min_l = MCU_AI_B.Min_m - (real32_T)rtb_Switch15_e;
    }
    else
    {
        MCU_AI_B.Min_l = 0.0F;
    }

    /* End of Switch: '<S278>/Switch28' */

    /* Switch: '<S278>/Switch29' incorporates:
     *  Constant: '<S278>/Constant28'
     *  Sum: '<S278>/Subtract6'
     *  Switch: '<S278>/Switch11'
     */
    if (rtb_Compare_h2)
    {
        /* Switch: '<S278>/Switch11' incorporates:
         *  Product: '<S278>/Product13'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Min_m = MCU_AI_B.Min4_i;
        }
        else
        {
            MCU_AI_B.Min_m = -MCU_AI_B.Min4_l;
        }

        MCU_AI_B.Switch13 = MCU_AI_B.Min_m - MCU_AI_B.Min_k;
    }
    else
    {
        MCU_AI_B.Switch13 = 0.0F;
    }

    /* End of Switch: '<S278>/Switch29' */

    /* Sum: '<S278>/Add1' */
    MCU_AI_B.Square1 = ((MCU_AI_B.Min_d + MCU_AI_B.Min_h1) + MCU_AI_B.Min_l) +
        MCU_AI_B.Switch13;

    /* RelationalOperator: '<S288>/Compare' incorporates:
     *  Abs: '<S278>/Abs8'
     *  Constant: '<S288>/Constant'
     */
    rtb_Compare_ps = (fabsf(MCU_AI_B.Square1) <= 0.4F);

    /* Product: '<S281>/Product5' incorporates:
     *  DataTypeConversion: '<S281>/Data Type Conversion5'
     *  Sum: '<S281>/Add4'
     *  Sum: '<S281>/Subtract4'
     */
    MCU_AI_B.Min_m = MCU_AI_B.Switch20 - (((MCU_AI_B.Switch9_f + MCU_AI_B.Min_h)
        + (real32_T)rtb_Switch15_e) + MCU_AI_B.Min_k);

    /* Switch: '<S281>/Switch5' incorporates:
     *  Abs: '<S281>/Abs4'
     *  Constant: '<S281>/Constant8'
     *  Constant: '<S349>/Constant'
     *  DataTypeConversion: '<S281>/Data Type Conversion8'
     *  RelationalOperator: '<S349>/Compare'
     */
    if (fabsf(MCU_AI_B.Min_m) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Min_m;
    }

    /* End of Switch: '<S281>/Switch5' */

    /* Switch: '<S278>/Switch17' incorporates:
     *  Constant: '<S278>/Constant22'
     *  DataTypeConversion: '<S281>/Data Type Conversion7'
     *  Product: '<S278>/Divide7'
     *  Switch: '<S278>/Switch22'
     */
    if (rtb_Compare_ps)
    {
        /* Switch: '<S278>/Switch18' incorporates:
         *  Constant: '<S278>/Constant5'
         *  Constant: '<S278>/Constant6'
         */
        if (rtb_Compare_mi)
        {
            MCU_AI_B.Switch2_kc = 0.4F;
        }
        else
        {
            MCU_AI_B.Switch2_kc = -0.4F;
        }

        /* End of Switch: '<S278>/Switch18' */
        MCU_AI_B.Square1 = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch2_kc = MCU_AI_B.Square1;
        MCU_AI_B.Square1 = (real32_T)rtb_Switch15_e * MCU_AI_B.Switch13 /
            MCU_AI_B.Square1;
    }

    /* End of Switch: '<S278>/Switch17' */

    /* Sum: '<S278>/Add5' */
    MCU_AI_B.Square1 += MCU_AI_B.Min_k;

    /* Switch: '<S278>/Switch2' incorporates:
     *  Constant: '<S278>/Constant14'
     *  Constant: '<S292>/Constant'
     *  Product: '<S278>/Product8'
     *  RelationalOperator: '<S292>/Compare'
     */
    if (MCU_AI_B.Product7_d * MCU_AI_B.Square1 < 0.0F)
    {
        MCU_AI_B.Square1 = 0.0F;
    }

    /* End of Switch: '<S278>/Switch2' */

    /* Switch: '<S278>/Switch8' incorporates:
     *  Abs: '<S278>/Abs3'
     *  Constant: '<S285>/Constant'
     *  RelationalOperator: '<S285>/Compare'
     */
    if (fabsf(MCU_AI_B.Square1) <= 0.1F)
    {
        /* DataTypeConversion: '<S278>/Data Type Conversion6' */
        MCU_AI_B.Square1 = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S278>/Data Type Conversion6' incorporates:
         *  DataTypeConversion: '<S278>/Data Type Conversion3'
         */
        MCU_AI_B.Square1 = (int16_T)MCU_AI_B.Square1;
    }

    /* End of Switch: '<S278>/Switch8' */

    /* Switch: '<S278>/Switch19' incorporates:
     *  Constant: '<S278>/Constant7'
     *  DataTypeConversion: '<S281>/Data Type Conversion7'
     *  Product: '<S278>/Divide35'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Product8_n = 0.0F;
    }
    else
    {
        MCU_AI_B.Product8_n = (real32_T)rtb_Switch15_e * MCU_AI_B.Min_d /
            MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S278>/Switch19' */

    /* Sum: '<S278>/Add2' */
    MCU_AI_B.Switch9_f += MCU_AI_B.Product8_n;

    /* Switch: '<S278>/Switch3' incorporates:
     *  Constant: '<S278>/Constant2'
     *  Constant: '<S289>/Constant'
     *  Product: '<S278>/Product1'
     *  RelationalOperator: '<S289>/Compare'
     */
    if (MCU_AI_B.Product7_d * MCU_AI_B.Switch9_f < 0.0F)
    {
        MCU_AI_B.Switch9_f = 0.0F;
    }

    /* End of Switch: '<S278>/Switch3' */

    /* Switch: '<S278>/Switch5' incorporates:
     *  Abs: '<S278>/Abs'
     *  Constant: '<S286>/Constant'
     *  RelationalOperator: '<S286>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch9_f) <= 0.1F)
    {
        /* DataTypeConversion: '<S278>/Data Type Conversion' */
        MCU_AI_B.Switch9_f = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S278>/Data Type Conversion' incorporates:
         *  DataTypeConversion: '<S278>/Data Type Conversion11'
         */
        MCU_AI_B.Switch9_f = (int16_T)MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S278>/Switch5' */

    /* Switch: '<S278>/Switch20' incorporates:
     *  Constant: '<S278>/Constant20'
     *  DataTypeConversion: '<S281>/Data Type Conversion7'
     *  Product: '<S278>/Divide5'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Product8_n = 0.0F;
    }
    else
    {
        MCU_AI_B.Product8_n = (real32_T)rtb_Switch15_e * MCU_AI_B.Min_h1 /
            MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S278>/Switch20' */

    /* Sum: '<S278>/Add3' */
    MCU_AI_B.Product8_n += MCU_AI_B.Min_h;

    /* Switch: '<S278>/Switch4' incorporates:
     *  Constant: '<S278>/Constant12'
     *  Constant: '<S290>/Constant'
     *  Product: '<S278>/Product6'
     *  RelationalOperator: '<S290>/Compare'
     */
    if (MCU_AI_B.Product7_d * MCU_AI_B.Product8_n < 0.0F)
    {
        MCU_AI_B.Min_k = 0.0F;
    }
    else
    {
        MCU_AI_B.Min_k = MCU_AI_B.Product8_n;
    }

    /* End of Switch: '<S278>/Switch4' */

    /* Switch: '<S278>/Switch6' incorporates:
     *  Abs: '<S278>/Abs1'
     *  Constant: '<S283>/Constant'
     *  RelationalOperator: '<S283>/Compare'
     */
    if (fabsf(MCU_AI_B.Min_k) <= 0.1F)
    {
        /* DataTypeConversion: '<S278>/Data Type Conversion4' */
        MCU_AI_B.Product8_n = 0.0F;
    }
    else
    {
        /* DataTypeConversion: '<S278>/Data Type Conversion4' incorporates:
         *  DataTypeConversion: '<S278>/Data Type Conversion1'
         */
        MCU_AI_B.Product8_n = (int16_T)MCU_AI_B.Min_k;
    }

    /* End of Switch: '<S278>/Switch6' */

    /* Switch: '<S278>/Switch21' incorporates:
     *  Constant: '<S278>/Constant21'
     *  DataTypeConversion: '<S281>/Data Type Conversion7'
     *  Product: '<S278>/Divide6'
     */
    if (rtb_Compare_ps)
    {
        MCU_AI_B.Switch2_kc = 0.0F;
    }
    else
    {
        MCU_AI_B.Switch2_kc = (real32_T)rtb_Switch15_e * MCU_AI_B.Min_l /
            MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S278>/Switch21' */

    /* Sum: '<S278>/Add4' */
    MCU_AI_B.Switch2_kc += MCU_AI_B.Min_kv;

    /* Switch: '<S278>/Switch13' incorporates:
     *  Constant: '<S278>/Constant13'
     *  Constant: '<S291>/Constant'
     *  Product: '<S278>/Product7'
     *  RelationalOperator: '<S291>/Compare'
     */
    if (MCU_AI_B.Product7_d * MCU_AI_B.Switch2_kc < 0.0F)
    {
        MCU_AI_B.Product7_d = 0.0F;
    }
    else
    {
        MCU_AI_B.Product7_d = MCU_AI_B.Switch2_kc;
    }

    /* End of Switch: '<S278>/Switch13' */

    /* Switch: '<S278>/Switch7' incorporates:
     *  Abs: '<S278>/Abs2'
     *  Constant: '<S278>/Constant18'
     *  Constant: '<S284>/Constant'
     *  DataTypeConversion: '<S278>/Data Type Conversion2'
     *  RelationalOperator: '<S284>/Compare'
     */
    if (fabsf(MCU_AI_B.Product7_d) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Product7_d;
    }

    /* End of Switch: '<S278>/Switch7' */

    /* DataTypeConversion: '<S278>/Data Type Conversion5' */
    MCU_AI_B.Product7_d = rtb_Switch15_e;

    /* Sum: '<S278>/Subtract4' incorporates:
     *  DataTypeConversion: '<S278>/Data Type Conversion5'
     *  Sum: '<S278>/Add6'
     */
    MCU_AI_B.Abs7 -= ((MCU_AI_B.Switch9_f + MCU_AI_B.Product8_n) + (real32_T)
                      rtb_Switch15_e) + MCU_AI_B.Square1;

    /* Switch: '<S278>/Switch1' incorporates:
     *  Abs: '<S278>/Abs4'
     *  Constant: '<S278>/Constant1'
     *  Constant: '<S287>/Constant'
     *  DataTypeConversion: '<S278>/Data Type Conversion8'
     *  RelationalOperator: '<S287>/Compare'
     */
    if (fabsf(MCU_AI_B.Abs7) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)MCU_AI_B.Abs7;
    }

    /* End of Switch: '<S278>/Switch1' */

    /* Sum: '<S297>/Add8' incorporates:
     *  DataTypeConversion: '<S278>/Data Type Conversion7'
     */
    MCU_AI_B.Abs7 = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S305>/Switch2' incorporates:
     *  RelationalOperator: '<S305>/LowerRelop1'
     */
    if (MCU_AI_B.Abs7 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Min_k = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S297>/Product4' */
        MCU_AI_B.Min_k = -MCU_AI_B.Min4_l;

        /* Switch: '<S305>/Switch' incorporates:
         *  Product: '<S297>/Product4'
         *  RelationalOperator: '<S305>/UpperRelop'
         */
        if (!(MCU_AI_B.Abs7 < -MCU_AI_B.Min4_l))
        {
            MCU_AI_B.Min_k = MCU_AI_B.Abs7;
        }

        /* End of Switch: '<S305>/Switch' */
    }

    /* End of Switch: '<S305>/Switch2' */

    /* Sum: '<S297>/Add7' incorporates:
     *  DataTypeConversion: '<S278>/Data Type Conversion7'
     */
    MCU_AI_B.Min_d = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S304>/Switch2' incorporates:
     *  RelationalOperator: '<S304>/LowerRelop1'
     */
    if (MCU_AI_B.Min_d > MCU_AI_B.Min)
    {
        MCU_AI_B.Min_h = MCU_AI_B.Min;
    }
    else
    {
        /* Product: '<S297>/Product3' */
        MCU_AI_B.Min_h = -MCU_AI_B.Min4_f;

        /* Switch: '<S304>/Switch' incorporates:
         *  Product: '<S297>/Product3'
         *  RelationalOperator: '<S304>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_d < -MCU_AI_B.Min4_f))
        {
            MCU_AI_B.Min_h = MCU_AI_B.Min_d;
        }

        /* End of Switch: '<S304>/Switch' */
    }

    /* End of Switch: '<S304>/Switch2' */

    /* Sum: '<S297>/Add6' incorporates:
     *  DataTypeConversion: '<S278>/Data Type Conversion7'
     */
    MCU_AI_B.Min_h1 = MCU_AI_B.Product8_n + (real32_T)rtb_Switch15_e;

    /* Switch: '<S303>/Switch2' incorporates:
     *  RelationalOperator: '<S303>/LowerRelop1'
     */
    if (MCU_AI_B.Min_h1 > MCU_AI_B.Min4)
    {
        MCU_AI_B.Min_kv = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S297>/Product2' */
        MCU_AI_B.Min_kv = -MCU_AI_B.Min4_pm;

        /* Switch: '<S303>/Switch' incorporates:
         *  Product: '<S297>/Product2'
         *  RelationalOperator: '<S303>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_h1 < -MCU_AI_B.Min4_pm))
        {
            MCU_AI_B.Min_kv = MCU_AI_B.Min_h1;
        }

        /* End of Switch: '<S303>/Switch' */
    }

    /* End of Switch: '<S303>/Switch2' */

    /* Sum: '<S297>/Add' incorporates:
     *  DataTypeConversion: '<S278>/Data Type Conversion7'
     */
    MCU_AI_B.Min_l = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S302>/Switch2' incorporates:
     *  RelationalOperator: '<S302>/LowerRelop1'
     */
    if (MCU_AI_B.Min_l > MCU_AI_B.DataTypeConversion9)
    {
        MCU_AI_B.Min_m = MCU_AI_B.DataTypeConversion9;
    }
    else
    {
        /* Product: '<S297>/Product1' */
        MCU_AI_B.Min_m = -MCU_AI_B.Min4_p;

        /* Switch: '<S302>/Switch' incorporates:
         *  Product: '<S297>/Product1'
         *  RelationalOperator: '<S302>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_l < -MCU_AI_B.Min4_p))
        {
            MCU_AI_B.Min_m = MCU_AI_B.Min_l;
        }

        /* End of Switch: '<S302>/Switch' */
    }

    /* End of Switch: '<S302>/Switch2' */

    /* SwitchCase: '<S297>/Switch Case' incorporates:
     *  Sum: '<S297>/Subtract5'
     *  Sum: '<S297>/Subtract7'
     *  Sum: '<S297>/Subtract8'
     *  Sum: '<S297>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2_o[0])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S297>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S306>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_l - MCU_AI_B.Min_m,
            MCU_AI_B.Min_m, &MCU_AI_B.Merge_go, &MCU_AI_B.Merge2_d);

        /* End of Outputs for SubSystem: '<S297>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S297>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S307>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_h1 - MCU_AI_B.Min_kv,
            MCU_AI_B.Min_kv, &MCU_AI_B.Merge_go, &MCU_AI_B.Merge2_d);

        /* End of Outputs for SubSystem: '<S297>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S297>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S308>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_d - MCU_AI_B.Min_h,
            MCU_AI_B.Min_h, &MCU_AI_B.Merge_go, &MCU_AI_B.Merge2_d);

        /* End of Outputs for SubSystem: '<S297>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S297>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S309>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Abs7 - MCU_AI_B.Min_k,
            MCU_AI_B.Min_k, &MCU_AI_B.Merge_go, &MCU_AI_B.Merge2_d);

        /* End of Outputs for SubSystem: '<S297>/Switch Case Action Subsystem3' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S297>/Switch Case' */

    /* Switch: '<S297>/Switch5' incorporates:
     *  Abs: '<S297>/Abs4'
     *  Constant: '<S297>/Constant8'
     *  Constant: '<S301>/Constant'
     *  DataTypeConversion: '<S297>/Data Type Conversion8'
     *  RelationalOperator: '<S301>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_go) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_go);
    }

    /* End of Switch: '<S297>/Switch5' */

    /* Sum: '<S298>/Add8' incorporates:
     *  DataTypeConversion: '<S297>/Data Type Conversion7'
     */
    MCU_AI_B.Abs7 = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S314>/Switch2' incorporates:
     *  RelationalOperator: '<S314>/LowerRelop1'
     */
    if (MCU_AI_B.Abs7 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Min_k = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S298>/Product4' */
        MCU_AI_B.Min_k = -MCU_AI_B.Min4_l;

        /* Switch: '<S314>/Switch' incorporates:
         *  Product: '<S298>/Product4'
         *  RelationalOperator: '<S314>/UpperRelop'
         */
        if (!(MCU_AI_B.Abs7 < -MCU_AI_B.Min4_l))
        {
            MCU_AI_B.Min_k = MCU_AI_B.Abs7;
        }

        /* End of Switch: '<S314>/Switch' */
    }

    /* End of Switch: '<S314>/Switch2' */

    /* Sum: '<S298>/Add7' incorporates:
     *  DataTypeConversion: '<S297>/Data Type Conversion7'
     */
    MCU_AI_B.Min_d = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S313>/Switch2' incorporates:
     *  RelationalOperator: '<S313>/LowerRelop1'
     */
    if (MCU_AI_B.Min_d > MCU_AI_B.Min)
    {
        MCU_AI_B.Min_h = MCU_AI_B.Min;
    }
    else
    {
        /* Product: '<S298>/Product3' */
        MCU_AI_B.Min_h = -MCU_AI_B.Min4_f;

        /* Switch: '<S313>/Switch' incorporates:
         *  Product: '<S298>/Product3'
         *  RelationalOperator: '<S313>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_d < -MCU_AI_B.Min4_f))
        {
            MCU_AI_B.Min_h = MCU_AI_B.Min_d;
        }

        /* End of Switch: '<S313>/Switch' */
    }

    /* End of Switch: '<S313>/Switch2' */

    /* Sum: '<S298>/Add6' incorporates:
     *  DataTypeConversion: '<S297>/Data Type Conversion7'
     */
    MCU_AI_B.Min_h1 = MCU_AI_B.Product8_n + (real32_T)rtb_Switch15_e;

    /* Switch: '<S312>/Switch2' incorporates:
     *  RelationalOperator: '<S312>/LowerRelop1'
     */
    if (MCU_AI_B.Min_h1 > MCU_AI_B.Min4)
    {
        MCU_AI_B.Min_kv = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S298>/Product2' */
        MCU_AI_B.Min_kv = -MCU_AI_B.Min4_pm;

        /* Switch: '<S312>/Switch' incorporates:
         *  Product: '<S298>/Product2'
         *  RelationalOperator: '<S312>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_h1 < -MCU_AI_B.Min4_pm))
        {
            MCU_AI_B.Min_kv = MCU_AI_B.Min_h1;
        }

        /* End of Switch: '<S312>/Switch' */
    }

    /* End of Switch: '<S312>/Switch2' */

    /* Sum: '<S298>/Add' incorporates:
     *  DataTypeConversion: '<S297>/Data Type Conversion7'
     */
    MCU_AI_B.Min_l = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S311>/Switch2' incorporates:
     *  RelationalOperator: '<S311>/LowerRelop1'
     */
    if (MCU_AI_B.Min_l > MCU_AI_B.DataTypeConversion9)
    {
        MCU_AI_B.Min_m = MCU_AI_B.DataTypeConversion9;
    }
    else
    {
        /* Product: '<S298>/Product1' */
        MCU_AI_B.Min_m = -MCU_AI_B.Min4_p;

        /* Switch: '<S311>/Switch' incorporates:
         *  Product: '<S298>/Product1'
         *  RelationalOperator: '<S311>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_l < -MCU_AI_B.Min4_p))
        {
            MCU_AI_B.Min_m = MCU_AI_B.Min_l;
        }

        /* End of Switch: '<S311>/Switch' */
    }

    /* End of Switch: '<S311>/Switch2' */

    /* SwitchCase: '<S298>/Switch Case' incorporates:
     *  Sum: '<S298>/Subtract5'
     *  Sum: '<S298>/Subtract7'
     *  Sum: '<S298>/Subtract8'
     *  Sum: '<S298>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2_o[1])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S298>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S315>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_l - MCU_AI_B.Min_m,
            MCU_AI_B.Min_m, &MCU_AI_B.Merge_j, &MCU_AI_B.Merge2_oe);

        /* End of Outputs for SubSystem: '<S298>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S298>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S316>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_h1 - MCU_AI_B.Min_kv,
            MCU_AI_B.Min_kv, &MCU_AI_B.Merge_j, &MCU_AI_B.Merge2_oe);

        /* End of Outputs for SubSystem: '<S298>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S298>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S317>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_d - MCU_AI_B.Min_h,
            MCU_AI_B.Min_h, &MCU_AI_B.Merge_j, &MCU_AI_B.Merge2_oe);

        /* End of Outputs for SubSystem: '<S298>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S298>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S318>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Abs7 - MCU_AI_B.Min_k,
            MCU_AI_B.Min_k, &MCU_AI_B.Merge_j, &MCU_AI_B.Merge2_oe);

        /* End of Outputs for SubSystem: '<S298>/Switch Case Action Subsystem3' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S298>/Switch Case' */

    /* Switch: '<S298>/Switch5' incorporates:
     *  Abs: '<S298>/Abs4'
     *  Constant: '<S298>/Constant8'
     *  Constant: '<S310>/Constant'
     *  DataTypeConversion: '<S298>/Data Type Conversion8'
     *  RelationalOperator: '<S310>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_j) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_j);
    }

    /* End of Switch: '<S298>/Switch5' */

    /* Sum: '<S299>/Add8' incorporates:
     *  DataTypeConversion: '<S298>/Data Type Conversion7'
     */
    MCU_AI_B.Abs7 = MCU_AI_B.Square1 + (real32_T)rtb_Switch15_e;

    /* Switch: '<S323>/Switch2' incorporates:
     *  RelationalOperator: '<S323>/LowerRelop1'
     */
    if (MCU_AI_B.Abs7 > MCU_AI_B.Min4_i)
    {
        MCU_AI_B.Min_k = MCU_AI_B.Min4_i;
    }
    else
    {
        /* Product: '<S299>/Product4' */
        MCU_AI_B.Min_k = -MCU_AI_B.Min4_l;

        /* Switch: '<S323>/Switch' incorporates:
         *  Product: '<S299>/Product4'
         *  RelationalOperator: '<S323>/UpperRelop'
         */
        if (!(MCU_AI_B.Abs7 < -MCU_AI_B.Min4_l))
        {
            MCU_AI_B.Min_k = MCU_AI_B.Abs7;
        }

        /* End of Switch: '<S323>/Switch' */
    }

    /* End of Switch: '<S323>/Switch2' */

    /* Sum: '<S299>/Add7' incorporates:
     *  DataTypeConversion: '<S298>/Data Type Conversion7'
     */
    MCU_AI_B.Min_d = MCU_AI_B.Product7_d + (real32_T)rtb_Switch15_e;

    /* Switch: '<S322>/Switch2' incorporates:
     *  RelationalOperator: '<S322>/LowerRelop1'
     */
    if (!(MCU_AI_B.Min_d > MCU_AI_B.Min))
    {
        /* Product: '<S299>/Product3' */
        MCU_AI_B.Min = -MCU_AI_B.Min4_f;

        /* Switch: '<S322>/Switch' incorporates:
         *  Product: '<S299>/Product3'
         *  RelationalOperator: '<S322>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_d < -MCU_AI_B.Min4_f))
        {
            MCU_AI_B.Min = MCU_AI_B.Min_d;
        }

        /* End of Switch: '<S322>/Switch' */
    }

    /* End of Switch: '<S322>/Switch2' */

    /* Sum: '<S299>/Add6' incorporates:
     *  DataTypeConversion: '<S298>/Data Type Conversion7'
     */
    MCU_AI_B.Min_h = MCU_AI_B.Product8_n + (real32_T)rtb_Switch15_e;

    /* Switch: '<S321>/Switch2' incorporates:
     *  RelationalOperator: '<S321>/LowerRelop1'
     */
    if (MCU_AI_B.Min_h > MCU_AI_B.Min4)
    {
        MCU_AI_B.Min_h1 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S299>/Product2' */
        MCU_AI_B.Min_h1 = -MCU_AI_B.Min4_pm;

        /* Switch: '<S321>/Switch' incorporates:
         *  Product: '<S299>/Product2'
         *  RelationalOperator: '<S321>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_h < -MCU_AI_B.Min4_pm))
        {
            MCU_AI_B.Min_h1 = MCU_AI_B.Min_h;
        }

        /* End of Switch: '<S321>/Switch' */
    }

    /* End of Switch: '<S321>/Switch2' */

    /* Sum: '<S299>/Add' incorporates:
     *  DataTypeConversion: '<S298>/Data Type Conversion7'
     */
    MCU_AI_B.Min_kv = MCU_AI_B.Switch9_f + (real32_T)rtb_Switch15_e;

    /* Switch: '<S320>/Switch2' incorporates:
     *  RelationalOperator: '<S320>/LowerRelop1'
     */
    if (MCU_AI_B.Min_kv > MCU_AI_B.DataTypeConversion9)
    {
        MCU_AI_B.Min_l = MCU_AI_B.DataTypeConversion9;
    }
    else
    {
        /* Product: '<S299>/Product1' */
        MCU_AI_B.Min_l = -MCU_AI_B.Min4_p;

        /* Switch: '<S320>/Switch' incorporates:
         *  Product: '<S299>/Product1'
         *  RelationalOperator: '<S320>/UpperRelop'
         */
        if (!(MCU_AI_B.Min_kv < -MCU_AI_B.Min4_p))
        {
            MCU_AI_B.Min_l = MCU_AI_B.Min_kv;
        }

        /* End of Switch: '<S320>/Switch' */
    }

    /* End of Switch: '<S320>/Switch2' */

    /* SwitchCase: '<S299>/Switch Case' incorporates:
     *  Sum: '<S299>/Subtract5'
     *  Sum: '<S299>/Subtract7'
     *  Sum: '<S299>/Subtract8'
     *  Sum: '<S299>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2_o[2])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S299>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S324>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_kv - MCU_AI_B.Min_l,
            MCU_AI_B.Min_l, &MCU_AI_B.Merge_dp, &MCU_AI_B.Merge2_c);

        /* End of Outputs for SubSystem: '<S299>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S299>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S325>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_h - MCU_AI_B.Min_h1,
            MCU_AI_B.Min_h1, &MCU_AI_B.Merge_dp, &MCU_AI_B.Merge2_c);

        /* End of Outputs for SubSystem: '<S299>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S299>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S326>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min_d - MCU_AI_B.Min,
            MCU_AI_B.Min, &MCU_AI_B.Merge_dp, &MCU_AI_B.Merge2_c);

        /* End of Outputs for SubSystem: '<S299>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S299>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S327>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Abs7 - MCU_AI_B.Min_k,
            MCU_AI_B.Min_k, &MCU_AI_B.Merge_dp, &MCU_AI_B.Merge2_c);

        /* End of Outputs for SubSystem: '<S299>/Switch Case Action Subsystem3' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S299>/Switch Case' */

    /* Switch: '<S299>/Switch5' incorporates:
     *  Abs: '<S299>/Abs4'
     *  Constant: '<S299>/Constant8'
     *  Constant: '<S319>/Constant'
     *  DataTypeConversion: '<S299>/Data Type Conversion8'
     *  RelationalOperator: '<S319>/Compare'
     */
    if (fabsf(MCU_AI_B.Merge_dp) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Merge_dp);
    }

    /* End of Switch: '<S299>/Switch5' */

    /* Sum: '<S300>/Add8' incorporates:
     *  DataTypeConversion: '<S299>/Data Type Conversion7'
     */
    MCU_AI_B.Square1 += (real32_T)rtb_Switch15_e;

    /* Switch: '<S332>/Switch2' incorporates:
     *  RelationalOperator: '<S332>/LowerRelop1'
     */
    if (!(MCU_AI_B.Square1 > MCU_AI_B.Min4_i))
    {
        /* Switch: '<S332>/Switch' incorporates:
         *  Product: '<S300>/Product4'
         *  RelationalOperator: '<S332>/UpperRelop'
         */
        if (MCU_AI_B.Square1 < -MCU_AI_B.Min4_l)
        {
            MCU_AI_B.Min4_i = -MCU_AI_B.Min4_l;
        }
        else
        {
            MCU_AI_B.Min4_i = MCU_AI_B.Square1;
        }

        /* End of Switch: '<S332>/Switch' */
    }

    /* End of Switch: '<S332>/Switch2' */

    /* Sum: '<S300>/Add7' incorporates:
     *  DataTypeConversion: '<S299>/Data Type Conversion7'
     */
    MCU_AI_B.Product7_d += (real32_T)rtb_Switch15_e;

    /* Switch: '<S331>/Switch2' incorporates:
     *  RelationalOperator: '<S331>/LowerRelop1'
     */
    if (MCU_AI_B.Product7_d > MCU_AI_B.Min4)
    {
        MCU_AI_B.Abs7 = MCU_AI_B.Min4;
    }
    else
    {
        /* Product: '<S300>/Product3' */
        MCU_AI_B.Abs7 = -MCU_AI_B.Min4_f;

        /* Switch: '<S331>/Switch' incorporates:
         *  Product: '<S300>/Product3'
         *  RelationalOperator: '<S331>/UpperRelop'
         */
        if (!(MCU_AI_B.Product7_d < -MCU_AI_B.Min4_f))
        {
            MCU_AI_B.Abs7 = MCU_AI_B.Product7_d;
        }

        /* End of Switch: '<S331>/Switch' */
    }

    /* End of Switch: '<S331>/Switch2' */

    /* Sum: '<S300>/Add6' incorporates:
     *  DataTypeConversion: '<S299>/Data Type Conversion7'
     */
    MCU_AI_B.Min4_f = MCU_AI_B.Product8_n + (real32_T)rtb_Switch15_e;

    /* Switch: '<S330>/Switch2' incorporates:
     *  RelationalOperator: '<S330>/LowerRelop1'
     */
    if (!(MCU_AI_B.Min4_f > MCU_AI_B.Min4))
    {
        /* Product: '<S300>/Product2' */
        MCU_AI_B.Min4 = -MCU_AI_B.Min4_pm;

        /* Switch: '<S330>/Switch' incorporates:
         *  Product: '<S300>/Product2'
         *  RelationalOperator: '<S330>/UpperRelop'
         */
        if (!(MCU_AI_B.Min4_f < -MCU_AI_B.Min4_pm))
        {
            MCU_AI_B.Min4 = MCU_AI_B.Min4_f;
        }

        /* End of Switch: '<S330>/Switch' */
    }

    /* End of Switch: '<S330>/Switch2' */

    /* Sum: '<S300>/Add' incorporates:
     *  DataTypeConversion: '<S299>/Data Type Conversion7'
     */
    MCU_AI_B.Switch9_f += (real32_T)rtb_Switch15_e;

    /* Switch: '<S329>/Switch2' incorporates:
     *  RelationalOperator: '<S329>/LowerRelop1'
     */
    if (!(MCU_AI_B.Switch9_f > MCU_AI_B.DataTypeConversion9))
    {
        /* Product: '<S300>/Product1' */
        MCU_AI_B.DataTypeConversion9 = -MCU_AI_B.Min4_p;

        /* Switch: '<S329>/Switch' incorporates:
         *  Product: '<S300>/Product1'
         *  RelationalOperator: '<S329>/UpperRelop'
         */
        if (!(MCU_AI_B.Switch9_f < -MCU_AI_B.Min4_p))
        {
            MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Switch9_f;
        }

        /* End of Switch: '<S329>/Switch' */
    }

    /* End of Switch: '<S329>/Switch2' */

    /* SwitchCase: '<S300>/Switch Case' incorporates:
     *  Sum: '<S300>/Subtract5'
     *  Sum: '<S300>/Subtract7'
     *  Sum: '<S300>/Subtract8'
     *  Sum: '<S300>/Subtract9'
     */
    switch ((int32_T)MCU_AI_B.Sort_o2_o[3])
    {
      case 1:
        /* Outputs for IfAction SubSystem: '<S300>/Switch Case Action Subsystem' incorporates:
         *  ActionPort: '<S333>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Switch9_f -
            MCU_AI_B.DataTypeConversion9, MCU_AI_B.DataTypeConversion9,
            &MCU_AI_B.Min4_p, &MCU_AI_B.Merge2_b);

        /* End of Outputs for SubSystem: '<S300>/Switch Case Action Subsystem' */
        break;

      case 2:
        /* Outputs for IfAction SubSystem: '<S300>/Switch Case Action Subsystem1' incorporates:
         *  ActionPort: '<S334>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Min4_f - MCU_AI_B.Min4,
            MCU_AI_B.Min4, &MCU_AI_B.Min4_p, &MCU_AI_B.Merge2_b);

        /* End of Outputs for SubSystem: '<S300>/Switch Case Action Subsystem1' */
        break;

      case 3:
        /* Outputs for IfAction SubSystem: '<S300>/Switch Case Action Subsystem2' incorporates:
         *  ActionPort: '<S335>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Product7_d - MCU_AI_B.Abs7,
            MCU_AI_B.Abs7, &MCU_AI_B.Min4_p, &MCU_AI_B.Merge2_b);

        /* End of Outputs for SubSystem: '<S300>/Switch Case Action Subsystem2' */
        break;

      case 4:
        /* Outputs for IfAction SubSystem: '<S300>/Switch Case Action Subsystem3' incorporates:
         *  ActionPort: '<S336>/Action Port'
         */
        MCU_A_SwitchCaseActionSubsystem(MCU_AI_B.Square1 - MCU_AI_B.Min4_i,
            MCU_AI_B.Min4_i, &MCU_AI_B.Min4_p, &MCU_AI_B.Merge2_b);

        /* End of Outputs for SubSystem: '<S300>/Switch Case Action Subsystem3' */
        break;

      default:
        /* no actions */
        break;
    }

    /* End of SwitchCase: '<S300>/Switch Case' */

    /* SignalConversion generated from: '<S279>/Selector' */
    MCU_AI_B.TmpSignalConversionAtSele_j[0] = MCU_AI_B.Merge2_d;
    MCU_AI_B.TmpSignalConversionAtSele_j[1] = MCU_AI_B.Merge2_oe;
    MCU_AI_B.TmpSignalConversionAtSele_j[2] = MCU_AI_B.Merge2_c;
    MCU_AI_B.TmpSignalConversionAtSele_j[3] = MCU_AI_B.Merge2_b;

    /* S-Function (sdspsrt2): '<S279>/Sort1' */
    /* DSP System Toolbox Sort (sdspsrt2) - '<S279>/Sort1' */
    {
        const uint32_T *uQS = MCU_AI_B.Sort_o2_o;
        uint32_T *index = MCU_AI_B.Sort1_o2_f;
        uint32_T *y = MCU_AI_B.Sort1_o1_g;
        uint32_T j_SORT;
        for (j_SORT=0; j_SORT<4; j_SORT++)
        {
            index[j_SORT] = j_SORT;
        }

        MWDSP_Sort_Qk_Idx_U32(uQS, index, 0, 3);
        for (j_SORT=0; j_SORT<4; j_SORT++)
        {
            y[j_SORT] = uQS[index[j_SORT]];
            index[j_SORT] += 1;        /* Convert to MATLAB 1-based indexing */
        }
    }

    /* SignalConversion generated from: '<S279>/Selector' */
    MCU_AI_B.fv1[0] = MCU_AI_B.Merge2_d;
    MCU_AI_B.fv1[1] = MCU_AI_B.Merge2_oe;
    MCU_AI_B.fv1[2] = MCU_AI_B.Merge2_c;
    MCU_AI_B.fv1[3] = MCU_AI_B.Merge2_b;

    /* Selector: '<S279>/Selector' */
    MCU_AI_B.Min = MCU_AI_B.fv1[(int32_T)MCU_AI_B.Sort1_o2_f[0] - 1];
    MCU_AI_B.Min_k = MCU_AI_B.fv1[(int32_T)MCU_AI_B.Sort1_o2_f[1] - 1];
    MCU_AI_B.Min_d = MCU_AI_B.fv1[(int32_T)MCU_AI_B.Sort1_o2_f[2] - 1];
    MCU_AI_B.Min_h = MCU_AI_B.fv1[(int32_T)MCU_AI_B.Sort1_o2_f[3] - 1];

    /* Switch: '<Root>/Switch' incorporates:
     *  Abs: '<S279>/Abs'
     *  Constant: '<S296>/Constant'
     *  Inport: '<Root>/bus2_or_bus1'
     *  RelationalOperator: '<S296>/Compare'
     *  Selector: '<S279>/Selector'
     *  Switch: '<Root>/Switch1'
     *  Switch: '<Root>/Switch2'
     *  Switch: '<Root>/Switch3'
     *  Switch: '<S279>/Switch1'
     */
    if (bus2_or_bus1 > 0)
    {
        /* Switch: '<S29>/Switch1' incorporates:
         *  Abs: '<S29>/Abs'
         *  Constant: '<S54>/Constant'
         *  RelationalOperator: '<S54>/Compare'
         *  Selector: '<S29>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
                  MCU_AI_B.Sort1_o2[0] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs1_power' incorporates:
             *  Constant: '<S29>/Constant1'
             */
            pcs1_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs1_power' incorporates:
             *  DataTypeConversion: '<S29>/Data Type Conversion11'
             */
            pcs1_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[0]);
        }

        /* End of Switch: '<S29>/Switch1' */

        /* Switch: '<S29>/Switch2' incorporates:
         *  Abs: '<S29>/Abs1'
         *  Constant: '<S51>/Constant'
         *  RelationalOperator: '<S51>/Compare'
         *  Selector: '<S29>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
                  MCU_AI_B.Sort1_o2[1] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs2_power' incorporates:
             *  Constant: '<S29>/Constant2'
             */
            pcs2_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs2_power' incorporates:
             *  DataTypeConversion: '<S29>/Data Type Conversion1'
             */
            pcs2_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[1]);
        }

        /* End of Switch: '<S29>/Switch2' */

        /* Switch: '<S29>/Switch3' incorporates:
         *  Abs: '<S29>/Abs2'
         *  Constant: '<S52>/Constant'
         *  RelationalOperator: '<S52>/Compare'
         *  Selector: '<S29>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
                  MCU_AI_B.Sort1_o2[2] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs3_power' incorporates:
             *  Constant: '<S29>/Constant3'
             */
            pcs3_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs3_power' incorporates:
             *  DataTypeConversion: '<S29>/Data Type Conversion2'
             */
            pcs3_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[2]);
        }

        /* End of Switch: '<S29>/Switch3' */

        /* Switch: '<S29>/Switch4' incorporates:
         *  Abs: '<S29>/Abs3'
         *  Constant: '<S53>/Constant'
         *  RelationalOperator: '<S53>/Compare'
         *  Selector: '<S29>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
                  MCU_AI_B.Sort1_o2[3] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs4_power' incorporates:
             *  Constant: '<S29>/Constant4'
             */
            pcs4_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs4_power' incorporates:
             *  DataTypeConversion: '<S29>/Data Type Conversion3'
             */
            pcs4_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[3]);
        }

        /* End of Switch: '<S29>/Switch4' */
    }
    else
    {
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSele_j[(int32_T)
                  MCU_AI_B.Sort1_o2_f[0] - 1]) <= 0.1F)
        {
            /* Switch: '<S279>/Switch1' incorporates:
             *  Constant: '<S279>/Constant1'
             *  Outport: '<Root>/pcs1_power'
             */
            pcs1_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs1_power' incorporates:
             *  DataTypeConversion: '<S279>/Data Type Conversion11'
             *  Switch: '<S279>/Switch1'
             */
            pcs1_power = (int16_T)rt_roundf_snf(MCU_AI_B.Min);
        }

        /* Switch: '<S279>/Switch2' incorporates:
         *  Abs: '<S279>/Abs1'
         *  Constant: '<S293>/Constant'
         *  RelationalOperator: '<S293>/Compare'
         *  Selector: '<S279>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSele_j[(int32_T)
                  MCU_AI_B.Sort1_o2_f[1] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs2_power' incorporates:
             *  Constant: '<S279>/Constant2'
             */
            pcs2_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs2_power' incorporates:
             *  DataTypeConversion: '<S279>/Data Type Conversion1'
             */
            pcs2_power = (int16_T)rt_roundf_snf(MCU_AI_B.Min_k);
        }

        /* End of Switch: '<S279>/Switch2' */

        /* Switch: '<S279>/Switch3' incorporates:
         *  Abs: '<S279>/Abs2'
         *  Constant: '<S294>/Constant'
         *  RelationalOperator: '<S294>/Compare'
         *  Selector: '<S279>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSele_j[(int32_T)
                  MCU_AI_B.Sort1_o2_f[2] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs3_power' incorporates:
             *  Constant: '<S279>/Constant3'
             */
            pcs3_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs3_power' incorporates:
             *  DataTypeConversion: '<S279>/Data Type Conversion2'
             */
            pcs3_power = (int16_T)rt_roundf_snf(MCU_AI_B.Min_d);
        }

        /* End of Switch: '<S279>/Switch3' */

        /* Switch: '<S279>/Switch4' incorporates:
         *  Abs: '<S279>/Abs3'
         *  Constant: '<S295>/Constant'
         *  RelationalOperator: '<S295>/Compare'
         *  Selector: '<S279>/Selector'
         */
        if (fabsf(MCU_AI_B.TmpSignalConversionAtSele_j[(int32_T)
                  MCU_AI_B.Sort1_o2_f[3] - 1]) <= 0.1F)
        {
            /* Outport: '<Root>/pcs4_power' incorporates:
             *  Constant: '<S279>/Constant4'
             */
            pcs4_power = 0;
        }
        else
        {
            /* Outport: '<Root>/pcs4_power' incorporates:
             *  DataTypeConversion: '<S279>/Data Type Conversion3'
             */
            pcs4_power = (int16_T)rt_roundf_snf(MCU_AI_B.Min_h);
        }

        /* End of Switch: '<S279>/Switch4' */
    }

    /* End of Switch: '<Root>/Switch' */

    /* Abs: '<S30>/Abs1' incorporates:
     *  DataTypeConversion: '<S30>/Data Type Conversion6'
     *  Inport: '<Root>/mv_r_power'
     */
    MCU_AI_B.Product7_d = fabsf((real32_T)mv_r_power);

    /* Switch: '<S205>/Switch2' incorporates:
     *  Constant: '<S30>/Constant6'
     *  Inport: '<Root>/reactive_rate'
     *  RelationalOperator: '<S205>/LowerRelop1'
     */
    if (reactive_rate > 100)
    {
        MCU_AI_B.i = 100;
    }
    else
    {
        MCU_AI_B.i = reactive_rate;
    }

    /* Product: '<S30>/Product5' incorporates:
     *  Constant: '<S30>/Constant9'
     *  Switch: '<S205>/Switch2'
     */
    MCU_AI_B.Switch9_f = (real32_T)MCU_AI_B.i * 0.01F;

    /* MinMax: '<S30>/Min8' incorporates:
     *  DataTypeConversion: '<S30>/Data Type Conversion4'
     *  DataTypeConversion: '<S30>/Data Type Conversion5'
     *  DataTypeConversion: '<S30>/Data Type Conversion8'
     *  DataTypeConversion: '<S30>/Data Type Conversion9'
     *  Product: '<S30>/Product6'
     *  Product: '<S30>/Product7'
     *  Product: '<S30>/Product8'
     *  Product: '<S30>/Product9'
     */
    MCU_AI_B.DataTypeConversion9 = fminf(fminf(fminf(MCU_AI_B.Switch9_f *
        (real32_T)MCU_AI_B.Switch4, MCU_AI_B.Switch9_f * (real32_T)
        MCU_AI_B.Switch3), MCU_AI_B.Switch9_f * (real32_T)MCU_AI_B.Switch2),
        MCU_AI_B.Switch9_f * (real32_T)MCU_AI_B.Switch1);

    /* Math: '<S30>/Square' incorporates:
     *  DataTypeConversion: '<S30>/Data Type Conversion'
     */
    MCU_AI_B.Switch9_f = (real32_T)MCU_AI_B.Switch4 * (real32_T)MCU_AI_B.Switch4;

    /* Sum: '<S29>/Add10' incorporates:
     *  Selector: '<S29>/Selector'
     */
    MCU_AI_B.Square1 = MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
        MCU_AI_B.Sort1_o2[0] - 1] + MCU_AI_B.TmpSignalConversionAtSelect
        [(int32_T)MCU_AI_B.Sort1_o2[1] - 1];

    /* Sqrt: '<S30>/Sqrt' incorporates:
     *  Math: '<S30>/Square1'
     *  MinMax: '<S30>/Min4'
     *  Sum: '<S30>/Subtract1'
     */
    MCU_AI_B.Switch9_f = sqrtf(MCU_AI_B.Switch9_f - fminf(MCU_AI_B.Switch9_f,
        MCU_AI_B.Square1 * MCU_AI_B.Square1));

    /* Switch: '<S208>/Switch2' incorporates:
     *  RelationalOperator: '<S208>/LowerRelop1'
     */
    if (MCU_AI_B.Switch9_f > MCU_AI_B.DataTypeConversion9)
    {
        MCU_AI_B.Switch9_f = MCU_AI_B.DataTypeConversion9;
    }

    /* End of Switch: '<S208>/Switch2' */

    /* Switch: '<S30>/Switch6' incorporates:
     *  Constant: '<S30>/Constant1'
     *  Logic: '<S1>/Logical Operator'
     *  MinMax: '<S30>/Min'
     *  Product: '<S30>/Product3'
     *  Switch: '<S30>/Switch2'
     */
    if (rtb_Compare_m0 && rtb_Compare_kvd)
    {
        /* Sum: '<S29>/Add1' incorporates:
         *  Selector: '<S29>/Selector'
         */
        MCU_AI_B.Square1 = MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
            MCU_AI_B.Sort1_o2[2] - 1] + MCU_AI_B.TmpSignalConversionAtSelect
            [(int32_T)MCU_AI_B.Sort1_o2[3] - 1];

        /* Math: '<S30>/Square2' incorporates:
         *  DataTypeConversion: '<S30>/Data Type Conversion1'
         */
        MCU_AI_B.Abs7 = (real32_T)MCU_AI_B.Switch3 * (real32_T)MCU_AI_B.Switch3;

        /* Sqrt: '<S30>/Sqrt1' incorporates:
         *  Math: '<S30>/Square3'
         *  MinMax: '<S30>/Min5'
         *  Sum: '<S30>/Subtract2'
         */
        MCU_AI_B.Square1 = sqrtf(MCU_AI_B.Abs7 - fminf(MCU_AI_B.Abs7,
            MCU_AI_B.Square1 * MCU_AI_B.Square1));

        /* Switch: '<S206>/Switch2' incorporates:
         *  RelationalOperator: '<S206>/LowerRelop1'
         */
        if (MCU_AI_B.Square1 > MCU_AI_B.DataTypeConversion9)
        {
            MCU_AI_B.Square1 = MCU_AI_B.DataTypeConversion9;
        }

        /* End of Switch: '<S206>/Switch2' */
        MCU_AI_B.Switch9_f = 2.0F * fminf(MCU_AI_B.Switch9_f, MCU_AI_B.Square1);
    }

    /* End of Switch: '<S30>/Switch6' */

    /* Logic: '<S1>/Logical Operator1' */
    rtb_Compare_ps = (rtb_Compare_ge && rtb_Compare_hr);

    /* Switch: '<S30>/Switch7' incorporates:
     *  Constant: '<S30>/Constant8'
     *  MinMax: '<S30>/Min2'
     *  MinMax: '<S30>/Min3'
     *  Product: '<S30>/Product2'
     *  Switch: '<S30>/Switch8'
     */
    if (rtb_Compare_ps)
    {
        /* Sum: '<S29>/Add2' incorporates:
         *  Selector: '<S29>/Selector'
         */
        MCU_AI_B.Square1 = MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
            MCU_AI_B.Sort1_o2[4] - 1] + MCU_AI_B.TmpSignalConversionAtSelect
            [(int32_T)MCU_AI_B.Sort1_o2[5] - 1];

        /* Math: '<S30>/Square4' incorporates:
         *  DataTypeConversion: '<S30>/Data Type Conversion2'
         */
        MCU_AI_B.Abs7 = (real32_T)MCU_AI_B.Switch2 * (real32_T)MCU_AI_B.Switch2;

        /* Sqrt: '<S30>/Sqrt2' incorporates:
         *  Math: '<S30>/Square5'
         *  MinMax: '<S30>/Min6'
         *  Sum: '<S30>/Subtract3'
         */
        MCU_AI_B.Square1 = sqrtf(MCU_AI_B.Abs7 - fminf(MCU_AI_B.Abs7,
            MCU_AI_B.Square1 * MCU_AI_B.Square1));

        /* Switch: '<S207>/Switch2' incorporates:
         *  RelationalOperator: '<S207>/LowerRelop1'
         */
        if (MCU_AI_B.Square1 > MCU_AI_B.DataTypeConversion9)
        {
            MCU_AI_B.Square1 = MCU_AI_B.DataTypeConversion9;
        }

        /* End of Switch: '<S207>/Switch2' */

        /* Switch: '<S30>/Switch1' incorporates:
         *  Constant: '<S30>/Constant2'
         *  Logic: '<S1>/Logical Operator2'
         *  MinMax: '<S30>/Min1'
         *  Product: '<S30>/Product1'
         *  Switch: '<S30>/Switch3'
         */
        if (rtb_Compare && rtb_Compare_j)
        {
            /* Sum: '<S29>/Add3' incorporates:
             *  Selector: '<S29>/Selector'
             */
            MCU_AI_B.Abs7 = MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)
                MCU_AI_B.Sort1_o2[6] - 1] +
                MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)MCU_AI_B.Sort1_o2
                [7] - 1];

            /* Math: '<S30>/Square6' incorporates:
             *  DataTypeConversion: '<S30>/Data Type Conversion3'
             */
            MCU_AI_B.Min4 = (real32_T)MCU_AI_B.Switch1 * (real32_T)
                MCU_AI_B.Switch1;

            /* Sqrt: '<S30>/Sqrt3' incorporates:
             *  Math: '<S30>/Square7'
             *  MinMax: '<S30>/Min7'
             *  Sum: '<S30>/Subtract5'
             */
            MCU_AI_B.Abs7 = sqrtf(MCU_AI_B.Min4 - fminf(MCU_AI_B.Min4,
                                   MCU_AI_B.Abs7 * MCU_AI_B.Abs7));

            /* Switch: '<S209>/Switch2' incorporates:
             *  RelationalOperator: '<S209>/LowerRelop1'
             *  Switch: '<S209>/Switch'
             */
            if (!(MCU_AI_B.Abs7 > MCU_AI_B.DataTypeConversion9))
            {
                MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Abs7;
            }

            /* End of Switch: '<S209>/Switch2' */
            MCU_AI_B.Square1 = 2.0F * fminf(MCU_AI_B.Square1,
                MCU_AI_B.DataTypeConversion9);
        }

        /* End of Switch: '<S30>/Switch1' */
        MCU_AI_B.Switch9_f = fminf(fminf(0.5F * MCU_AI_B.Product7_d,
            MCU_AI_B.Switch9_f), MCU_AI_B.Square1);
    }
    else
    {
        MCU_AI_B.Switch9_f = fminf(MCU_AI_B.Switch9_f, MCU_AI_B.Product7_d);
    }

    /* End of Switch: '<S30>/Switch7' */

    /* Switch: '<S30>/Switch4' incorporates:
     *  Constant: '<S204>/Constant'
     *  Inport: '<Root>/mv_r_power'
     *  Product: '<S30>/Product4'
     *  RelationalOperator: '<S204>/Compare'
     */
    if (mv_r_power < 0)
    {
        MCU_AI_B.Switch9_f = -MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S30>/Switch4' */

    /* Switch: '<S30>/Switch5' incorporates:
     *  Abs: '<S30>/Abs2'
     *  Constant: '<S203>/Constant'
     *  Constant: '<S30>/Constant4'
     *  DataTypeConversion: '<S30>/Data Type Conversion7'
     *  RelationalOperator: '<S203>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch9_f) <= 0.1F)
    {
        rtb_Switch15_e = 0;
    }
    else
    {
        rtb_Switch15_e = (int16_T)rt_roundf_snf(MCU_AI_B.Switch9_f);
    }

    /* End of Switch: '<S30>/Switch5' */

    /* Abs: '<S280>/Abs1' incorporates:
     *  DataTypeConversion: '<S280>/Data Type Conversion6'
     *  Inport: '<Root>/mv_r_power'
     */
    MCU_AI_B.Product7_d = fabsf((real32_T)mv_r_power);

    /* Switch: '<S339>/Switch2' incorporates:
     *  Constant: '<S280>/Constant6'
     *  Inport: '<Root>/reactive_rate'
     *  RelationalOperator: '<S339>/LowerRelop1'
     */
    if (reactive_rate > 100)
    {
        MCU_AI_B.i = 100;
    }
    else
    {
        MCU_AI_B.i = reactive_rate;
    }

    /* Product: '<S280>/Product5' incorporates:
     *  Constant: '<S280>/Constant9'
     *  Switch: '<S339>/Switch2'
     */
    MCU_AI_B.Switch9_f = (real32_T)MCU_AI_B.i * 0.01F;

    /* MinMax: '<S280>/Min8' incorporates:
     *  DataTypeConversion: '<S280>/Data Type Conversion4'
     *  DataTypeConversion: '<S280>/Data Type Conversion5'
     *  DataTypeConversion: '<S280>/Data Type Conversion8'
     *  DataTypeConversion: '<S280>/Data Type Conversion9'
     *  Product: '<S280>/Product6'
     *  Product: '<S280>/Product7'
     *  Product: '<S280>/Product8'
     *  Product: '<S280>/Product9'
     */
    MCU_AI_B.Square1 = fminf(fminf(fminf(MCU_AI_B.Switch9_f * (real32_T)
        MCU_AI_B.Switch4, MCU_AI_B.Switch9_f * (real32_T)MCU_AI_B.Switch3),
        MCU_AI_B.Switch9_f * (real32_T)MCU_AI_B.Switch2), MCU_AI_B.Switch9_f *
        (real32_T)MCU_AI_B.Switch1);

    /* Math: '<S280>/Square' incorporates:
     *  DataTypeConversion: '<S280>/Data Type Conversion'
     */
    MCU_AI_B.Switch9_f = (real32_T)MCU_AI_B.Switch4_d * (real32_T)
        MCU_AI_B.Switch4_d;

    /* Sqrt: '<S280>/Sqrt' incorporates:
     *  Math: '<S280>/Square1'
     *  MinMax: '<S280>/Min4'
     *  Sum: '<S280>/Subtract1'
     */
    MCU_AI_B.Switch9_f = sqrtf(MCU_AI_B.Switch9_f - fminf(MCU_AI_B.Switch9_f,
        MCU_AI_B.Min * MCU_AI_B.Min));

    /* Switch: '<S342>/Switch2' incorporates:
     *  RelationalOperator: '<S342>/LowerRelop1'
     */
    if (MCU_AI_B.Switch9_f > MCU_AI_B.Square1)
    {
        MCU_AI_B.Switch9_f = MCU_AI_B.Square1;
    }

    /* End of Switch: '<S342>/Switch2' */

    /* Switch: '<S280>/Switch6' incorporates:
     *  Constant: '<S280>/Constant1'
     *  MinMax: '<S280>/Min'
     *  Product: '<S280>/Product3'
     *  Switch: '<S280>/Switch2'
     */
    if (rtb_Compare_bq)
    {
        /* Math: '<S280>/Square2' incorporates:
         *  DataTypeConversion: '<S280>/Data Type Conversion1'
         */
        MCU_AI_B.DataTypeConversion9 = (real32_T)MCU_AI_B.Switch3_g * (real32_T)
            MCU_AI_B.Switch3_g;

        /* Sqrt: '<S280>/Sqrt1' incorporates:
         *  Math: '<S280>/Square3'
         *  MinMax: '<S280>/Min5'
         *  Sum: '<S280>/Subtract2'
         */
        MCU_AI_B.DataTypeConversion9 = sqrtf(MCU_AI_B.DataTypeConversion9 -
            fminf(MCU_AI_B.DataTypeConversion9, MCU_AI_B.Min_k * MCU_AI_B.Min_k));

        /* Switch: '<S340>/Switch2' incorporates:
         *  RelationalOperator: '<S340>/LowerRelop1'
         */
        if (MCU_AI_B.DataTypeConversion9 > MCU_AI_B.Square1)
        {
            MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Square1;
        }

        /* End of Switch: '<S340>/Switch2' */
        MCU_AI_B.Switch9_f = 2.0F * fminf(MCU_AI_B.Switch9_f,
            MCU_AI_B.DataTypeConversion9);
    }

    /* End of Switch: '<S280>/Switch6' */

    /* Switch: '<S280>/Switch7' incorporates:
     *  Constant: '<S280>/Constant8'
     *  MinMax: '<S280>/Min2'
     *  MinMax: '<S280>/Min3'
     *  Product: '<S280>/Product2'
     *  Switch: '<S280>/Switch8'
     */
    if (rtb_Compare_lc)
    {
        /* Math: '<S280>/Square4' incorporates:
         *  DataTypeConversion: '<S280>/Data Type Conversion2'
         */
        MCU_AI_B.DataTypeConversion9 = (real32_T)MCU_AI_B.Switch2_p * (real32_T)
            MCU_AI_B.Switch2_p;

        /* Sqrt: '<S280>/Sqrt2' incorporates:
         *  Math: '<S280>/Square5'
         *  MinMax: '<S280>/Min6'
         *  Sum: '<S280>/Subtract3'
         */
        MCU_AI_B.DataTypeConversion9 = sqrtf(MCU_AI_B.DataTypeConversion9 -
            fminf(MCU_AI_B.DataTypeConversion9, MCU_AI_B.Min_d * MCU_AI_B.Min_d));

        /* Switch: '<S341>/Switch2' incorporates:
         *  RelationalOperator: '<S341>/LowerRelop1'
         */
        if (MCU_AI_B.DataTypeConversion9 > MCU_AI_B.Square1)
        {
            MCU_AI_B.DataTypeConversion9 = MCU_AI_B.Square1;
        }

        /* End of Switch: '<S341>/Switch2' */

        /* Switch: '<S280>/Switch1' incorporates:
         *  Constant: '<S280>/Constant2'
         *  MinMax: '<S280>/Min1'
         *  Product: '<S280>/Product1'
         *  Switch: '<S280>/Switch3'
         */
        if (rtb_Compare_h2)
        {
            /* Math: '<S280>/Square6' incorporates:
             *  DataTypeConversion: '<S280>/Data Type Conversion3'
             */
            MCU_AI_B.Abs7 = (real32_T)MCU_AI_B.Switch1_k * (real32_T)
                MCU_AI_B.Switch1_k;

            /* Sqrt: '<S280>/Sqrt3' incorporates:
             *  Math: '<S280>/Square7'
             *  MinMax: '<S280>/Min7'
             *  Sum: '<S280>/Subtract5'
             */
            MCU_AI_B.Abs7 = sqrtf(MCU_AI_B.Abs7 - fminf(MCU_AI_B.Abs7,
                                   MCU_AI_B.Min_h * MCU_AI_B.Min_h));

            /* Switch: '<S343>/Switch2' incorporates:
             *  RelationalOperator: '<S343>/LowerRelop1'
             *  Switch: '<S343>/Switch'
             */
            if (!(MCU_AI_B.Abs7 > MCU_AI_B.Square1))
            {
                MCU_AI_B.Square1 = MCU_AI_B.Abs7;
            }

            /* End of Switch: '<S343>/Switch2' */
            MCU_AI_B.DataTypeConversion9 = 2.0F * fminf
                (MCU_AI_B.DataTypeConversion9, MCU_AI_B.Square1);
        }

        /* End of Switch: '<S280>/Switch1' */
        MCU_AI_B.Switch9_f = fminf(fminf(0.5F * MCU_AI_B.Product7_d,
            MCU_AI_B.Switch9_f), MCU_AI_B.DataTypeConversion9);
    }
    else
    {
        MCU_AI_B.Switch9_f = fminf(MCU_AI_B.Switch9_f, MCU_AI_B.Product7_d);
    }

    /* End of Switch: '<S280>/Switch7' */

    /* Switch: '<S280>/Switch4' incorporates:
     *  Constant: '<S338>/Constant'
     *  Inport: '<Root>/mv_r_power'
     *  Product: '<S280>/Product4'
     *  RelationalOperator: '<S338>/Compare'
     */
    if (mv_r_power < 0)
    {
        MCU_AI_B.Switch9_f = -MCU_AI_B.Switch9_f;
    }

    /* End of Switch: '<S280>/Switch4' */

    /* Switch: '<S280>/Switch5' incorporates:
     *  Abs: '<S280>/Abs2'
     *  Constant: '<S280>/Constant4'
     *  Constant: '<S337>/Constant'
     *  DataTypeConversion: '<S280>/Data Type Conversion7'
     *  RelationalOperator: '<S337>/Compare'
     */
    if (fabsf(MCU_AI_B.Switch9_f) <= 0.1F)
    {
        rtb_Switch5_f = 0;
    }
    else
    {
        rtb_Switch5_f = (int16_T)rt_roundf_snf(MCU_AI_B.Switch9_f);
    }

    /* End of Switch: '<S280>/Switch5' */

    /* Switch: '<Root>/Switch4' incorporates:
     *  Inport: '<Root>/bus2_or_bus1'
     *  Switch: '<Root>/Switch5'
     */
    if (bus2_or_bus1 > 0)
    {
        /* Outport: '<Root>/pcs_r_power1_2' */
        pcs_r_power1_2 = rtb_Switch15_e;

        /* Switch: '<S30>/Switch9' */
        if (rtb_Compare_ps)
        {
            /* Outport: '<Root>/pcs_r_power3_4' */
            pcs_r_power3_4 = rtb_Switch15_e;
        }
        else
        {
            /* Outport: '<Root>/pcs_r_power3_4' incorporates:
             *  Constant: '<S30>/Constant5'
             */
            pcs_r_power3_4 = 0;
        }

        /* End of Switch: '<S30>/Switch9' */
    }
    else
    {
        /* Outport: '<Root>/pcs_r_power1_2' */
        pcs_r_power1_2 = rtb_Switch5_f;

        /* Switch: '<S280>/Switch10' */
        if (rtb_Compare_lc)
        {
            /* Outport: '<Root>/pcs_r_power3_4' */
            pcs_r_power3_4 = rtb_Switch5_f;
        }
        else
        {
            /* Outport: '<Root>/pcs_r_power3_4' incorporates:
             *  Constant: '<S280>/Constant5'
             */
            pcs_r_power3_4 = 0;
        }

        /* End of Switch: '<S280>/Switch10' */
    }

    /* End of Switch: '<Root>/Switch4' */

    /* Switch: '<S29>/Switch12' incorporates:
     *  Abs: '<S29>/Abs4'
     *  Constant: '<S58>/Constant'
     *  RelationalOperator: '<S58>/Compare'
     *  Selector: '<S29>/Selector'
     */
    if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)MCU_AI_B.Sort1_o2[4]
              - 1]) <= 0.1F)
    {
        /* Outport: '<Root>/pcs5_power' incorporates:
         *  Constant: '<S29>/Constant9'
         */
        pcs5_power = 0;
    }
    else
    {
        /* Outport: '<Root>/pcs5_power' incorporates:
         *  DataTypeConversion: '<S29>/Data Type Conversion5'
         */
        pcs5_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[4]);
    }

    /* End of Switch: '<S29>/Switch12' */

    /* Switch: '<S29>/Switch13' incorporates:
     *  Abs: '<S29>/Abs5'
     *  Constant: '<S55>/Constant'
     *  RelationalOperator: '<S55>/Compare'
     *  Selector: '<S29>/Selector'
     */
    if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)MCU_AI_B.Sort1_o2[5]
              - 1]) <= 0.1F)
    {
        /* Outport: '<Root>/pcs6_power' incorporates:
         *  Constant: '<S29>/Constant10'
         */
        pcs6_power = 0;
    }
    else
    {
        /* Outport: '<Root>/pcs6_power' incorporates:
         *  DataTypeConversion: '<S29>/Data Type Conversion4'
         */
        pcs6_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[5]);
    }

    /* End of Switch: '<S29>/Switch13' */

    /* Switch: '<S29>/Switch14' incorporates:
     *  Abs: '<S29>/Abs6'
     *  Constant: '<S56>/Constant'
     *  RelationalOperator: '<S56>/Compare'
     *  Selector: '<S29>/Selector'
     */
    if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)MCU_AI_B.Sort1_o2[6]
              - 1]) <= 0.1F)
    {
        /* Outport: '<Root>/pcs7_power' incorporates:
         *  Constant: '<S29>/Constant11'
         */
        pcs7_power = 0;
    }
    else
    {
        /* Outport: '<Root>/pcs7_power' incorporates:
         *  DataTypeConversion: '<S29>/Data Type Conversion6'
         */
        pcs7_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[6]);
    }

    /* End of Switch: '<S29>/Switch14' */

    /* Switch: '<S29>/Switch15' incorporates:
     *  Abs: '<S29>/Abs7'
     *  Constant: '<S57>/Constant'
     *  RelationalOperator: '<S57>/Compare'
     *  Selector: '<S29>/Selector'
     */
    if (fabsf(MCU_AI_B.TmpSignalConversionAtSelect[(int32_T)MCU_AI_B.Sort1_o2[7]
              - 1]) <= 0.1F)
    {
        /* Outport: '<Root>/pcs8_power' incorporates:
         *  Constant: '<S29>/Constant12'
         */
        pcs8_power = 0;
    }
    else
    {
        /* Outport: '<Root>/pcs8_power' incorporates:
         *  DataTypeConversion: '<S29>/Data Type Conversion7'
         */
        pcs8_power = (int16_T)rt_roundf_snf(MCU_AI_B.Selector[7]);
    }

    /* End of Switch: '<S29>/Switch15' */
}

/* Model initialize function */
void MCU_AI_initialize(void)
{
    /* Registration code */

    /* initialize error status */
    rtmSetErrorStatus(MCU_AI_M, (NULL));

    /* block I/O */
    (void) memset(((void *) &MCU_AI_B), 0,
                  sizeof(BlockIO_MCU_AI));
}

/* Model terminate function */
void MCU_AI_terminate(void)
{
    /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

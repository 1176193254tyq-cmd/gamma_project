/*
 * File: MCU_AI.h
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

#ifndef MCU_AI_h_
#define MCU_AI_h_
#ifndef MCU_AI_COMMON_INCLUDES_
#define MCU_AI_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "math.h"
#endif                                 /* MCU_AI_COMMON_INCLUDES_ */

#include <stddef.h>
#include <string.h>
#include "MCU_AI_types.h"

/* Includes for objects with custom storage classes */
#include "global_mcu_ai.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* Block signals (default storage) */
typedef struct
{
    uint32_T Sort_o2[8];               /* '<S29>/Sort' */
    uint32_T Sort1_o2[8];              /* '<S29>/Sort1' */
    uint32_T Sort1_o1[8];              /* '<S29>/Sort1' */
    real32_T TmpSignalConversionAtSelect[8];
    real32_T Selector[8];              /* '<S29>/Selector' */
    real32_T fv[8];
    real32_T Sort_o1[8];               /* '<S29>/Sort' */
    uint32_T Sort_o2_o[4];             /* '<S279>/Sort' */
    uint32_T Sort1_o2_f[4];            /* '<S279>/Sort1' */
    uint32_T Sort1_o1_g[4];            /* '<S279>/Sort1' */
    real32_T TmpSignalConversionAtSele_j[4];
    real32_T fv1[4];
    real32_T Sort_o1_p[4];             /* '<S279>/Sort' */
    real32_T Merge2;                   /* '<S59>/Merge2' */
    real32_T Merge;                    /* '<S59>/Merge' */
    real32_T Merge2_f;                 /* '<S60>/Merge2' */
    real32_T Merge_g;                  /* '<S60>/Merge' */
    real32_T Merge2_g;                 /* '<S61>/Merge2' */
    real32_T Merge_d;                  /* '<S61>/Merge' */
    real32_T Merge2_a;                 /* '<S62>/Merge2' */
    real32_T Merge_gg;                 /* '<S62>/Merge' */
    real32_T Merge2_o;                 /* '<S63>/Merge2' */
    real32_T Merge_p;                  /* '<S63>/Merge' */
    real32_T Merge2_gk;                /* '<S64>/Merge2' */
    real32_T Merge_gi;                 /* '<S64>/Merge' */
    real32_T Merge2_e;                 /* '<S65>/Merge2' */
    real32_T Merge_i;                  /* '<S65>/Merge' */
    real32_T Merge2_gm;                /* '<S66>/Merge2' */
    real32_T Merge2_d;                 /* '<S297>/Merge2' */
    real32_T Merge_go;                 /* '<S297>/Merge' */
    real32_T Merge2_oe;                /* '<S298>/Merge2' */
    real32_T Merge_j;                  /* '<S298>/Merge' */
    real32_T Merge2_c;                 /* '<S299>/Merge2' */
    real32_T Merge_dp;                 /* '<S299>/Merge' */
    real32_T Merge2_b;                 /* '<S300>/Merge2' */
    real32_T Min;                      /* '<S11>/Min' */
    real32_T Min_k;                    /* '<S12>/Min' */
    real32_T Min_d;                    /* '<S13>/Min' */
    real32_T Min_h;                    /* '<S14>/Min' */
    real32_T Switch12_e;               /* '<S278>/Switch12' */
    real32_T Min_h1;                   /* '<S15>/Min' */
    real32_T Min_kv;                   /* '<S16>/Min' */
    real32_T DataTypeConversion9;      /* '<S280>/Data Type Conversion9' */
    real32_T Min_l;                    /* '<S17>/Min' */
    real32_T Product8_n;               /* '<S280>/Product8' */
    real32_T Min_m;                    /* '<S18>/Min' */
    real32_T Min4;                     /* '<S19>/Min4' */
    real32_T Min4_i;                   /* '<S20>/Min4' */
    real32_T Min4_p;                   /* '<S22>/Min4' */
    real32_T Product7_d;               /* '<S280>/Product7' */
    real32_T Min4_pm;                  /* '<S23>/Min4' */
    real32_T Square1;                  /* '<S280>/Square1' */
    real32_T Min4_f;                   /* '<S25>/Min4' */
    real32_T Abs7;                     /* '<S29>/Abs7' */
    real32_T Min4_l;                   /* '<S26>/Min4' */
    real32_T Switch20;                 /* '<S32>/Switch20' */
    real32_T Switch13;                 /* '<S32>/Switch13' */
    real32_T Switch9_f;                /* '<S32>/Switch9' */
    real32_T Switch14;                 /* '<S32>/Switch14' */
    real32_T Switch15;                 /* '<S32>/Switch15' */
    real32_T Switch5;                  /* '<S32>/Switch5' */
    real32_T Switch6;                  /* '<S32>/Switch6' */
    real32_T Switch7;                  /* '<S32>/Switch7' */
    real32_T Switch19;                 /* '<S31>/Switch19' */
    real32_T Product8;                 /* '<S31>/Product8' */
    real32_T Switch17;                 /* '<S31>/Switch17' */
    real32_T Switch2_kc;               /* '<S231>/Switch2' */
    real32_T Divide35_o3;              /* '<S31>/Divide35' */
    real32_T Switch2_apa;              /* '<S228>/Switch2' */
    real32_T Switch27;                 /* '<S28>/Switch27' */
    real32_T Switch28;                 /* '<S28>/Switch28' */
    real32_T DataTypeConversion6_b;    /* '<S31>/Data Type Conversion6' */
    real32_T Switch29;                 /* '<S28>/Switch29' */
    real32_T Switch17_b;               /* '<S28>/Switch17' */
    int32_T DataTypeConversion15;      /* '<S31>/Data Type Conversion15' */
    int32_T DataTypeConversion4;       /* '<S31>/Data Type Conversion4' */
    int32_T i;
    uint32_T Switch4;                  /* '<S1>/Switch4' */
    uint32_T Switch4_d;                /* '<S2>/Switch4' */
    uint32_T Switch3;                  /* '<S1>/Switch3' */
    uint32_T Switch2;                  /* '<S1>/Switch2' */
    uint32_T Switch1;                  /* '<S1>/Switch1' */
    uint32_T Switch3_g;                /* '<S2>/Switch3' */
    uint32_T Switch2_p;                /* '<S2>/Switch2' */
    uint32_T Switch1_k;                /* '<S2>/Switch1' */
}
BlockIO_MCU_AI;

/* Real-time Model Data Structure */
struct tag_RTM_MCU_AI
{
    const char_T * volatile errorStatus;
};

/* Block signals (default storage) */
extern BlockIO_MCU_AI MCU_AI_B;

/* Model entry point functions */
extern void MCU_AI_initialize(void);
extern void MCU_AI_step(void);
extern void MCU_AI_terminate(void);

/* Real-time Model object */
extern RT_MODEL_MCU_AI *const MCU_AI_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'MCU_AI'
 * '<S1>'   : 'MCU_AI/AI_PRO_5ms'
 * '<S2>'   : 'MCU_AI/AI_PRO_5ms1'
 * '<S3>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant'
 * '<S4>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant1'
 * '<S5>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant2'
 * '<S6>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant3'
 * '<S7>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant4'
 * '<S8>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant5'
 * '<S9>'   : 'MCU_AI/AI_PRO_5ms/Compare To Constant6'
 * '<S10>'  : 'MCU_AI/AI_PRO_5ms/Compare To Zero'
 * '<S11>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE1_CALC_PRO'
 * '<S12>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE2_CALC_PRO'
 * '<S13>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE3_CALC_PRO'
 * '<S14>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE4_CALC_PRO'
 * '<S15>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE5_CALC_PRO'
 * '<S16>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE6_CALC_PRO'
 * '<S17>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE7_CALC_PRO'
 * '<S18>'  : 'MCU_AI/AI_PRO_5ms/MAX_CHARGE8_CALC_PRO'
 * '<S19>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE1_CALC_PRO'
 * '<S20>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE2_CALC_PRO'
 * '<S21>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE3_CALC_PRO'
 * '<S22>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE4_CALC_PRO'
 * '<S23>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE5_CALC_PRO'
 * '<S24>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE6_CALC_PRO'
 * '<S25>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE7_CALC_PRO'
 * '<S26>'  : 'MCU_AI/AI_PRO_5ms/MAX_DISCHARGE8_CALC_PRO'
 * '<S27>'  : 'MCU_AI/AI_PRO_5ms/MV_POWER_CALC_PRO'
 * '<S28>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO'
 * '<S29>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO'
 * '<S30>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO'
 * '<S31>'  : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO'
 * '<S32>'  : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO'
 * '<S33>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant1'
 * '<S34>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant10'
 * '<S35>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant2'
 * '<S36>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant3'
 * '<S37>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant4'
 * '<S38>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant5'
 * '<S39>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant6'
 * '<S40>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant7'
 * '<S41>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant8'
 * '<S42>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/Compare To Constant9'
 * '<S43>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative'
 * '<S44>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative1'
 * '<S45>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative2'
 * '<S46>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative3'
 * '<S47>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative4'
 * '<S48>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative5'
 * '<S49>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative6'
 * '<S50>'  : 'MCU_AI/AI_PRO_5ms/PCS_LIM_POWER_CALC_PRO/IsNegative7'
 * '<S51>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant1'
 * '<S52>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant2'
 * '<S53>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant3'
 * '<S54>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant4'
 * '<S55>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant5'
 * '<S56>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant6'
 * '<S57>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant7'
 * '<S58>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/Compare To Constant8'
 * '<S59>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1'
 * '<S60>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2'
 * '<S61>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3'
 * '<S62>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4'
 * '<S63>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5'
 * '<S64>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6'
 * '<S65>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7'
 * '<S66>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8'
 * '<S67>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Compare To Constant6'
 * '<S68>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic1'
 * '<S69>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic2'
 * '<S70>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic3'
 * '<S71>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic4'
 * '<S72>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic5'
 * '<S73>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic6'
 * '<S74>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic7'
 * '<S75>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic8'
 * '<S76>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem'
 * '<S77>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem1'
 * '<S78>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem2'
 * '<S79>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem3'
 * '<S80>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem4'
 * '<S81>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem5'
 * '<S82>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem6'
 * '<S83>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem7'
 * '<S84>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Compare To Constant6'
 * '<S85>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic1'
 * '<S86>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic2'
 * '<S87>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic3'
 * '<S88>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic4'
 * '<S89>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic5'
 * '<S90>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic6'
 * '<S91>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic7'
 * '<S92>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic8'
 * '<S93>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem'
 * '<S94>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem1'
 * '<S95>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem2'
 * '<S96>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem3'
 * '<S97>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem4'
 * '<S98>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem5'
 * '<S99>'  : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem6'
 * '<S100>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem7'
 * '<S101>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Compare To Constant6'
 * '<S102>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic1'
 * '<S103>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic2'
 * '<S104>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic3'
 * '<S105>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic4'
 * '<S106>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic5'
 * '<S107>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic6'
 * '<S108>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic7'
 * '<S109>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic8'
 * '<S110>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem'
 * '<S111>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem1'
 * '<S112>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem2'
 * '<S113>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem3'
 * '<S114>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem4'
 * '<S115>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem5'
 * '<S116>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem6'
 * '<S117>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem7'
 * '<S118>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Compare To Constant6'
 * '<S119>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic1'
 * '<S120>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic2'
 * '<S121>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic3'
 * '<S122>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic4'
 * '<S123>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic5'
 * '<S124>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic6'
 * '<S125>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic7'
 * '<S126>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic8'
 * '<S127>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem'
 * '<S128>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem1'
 * '<S129>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem2'
 * '<S130>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem3'
 * '<S131>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem4'
 * '<S132>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem5'
 * '<S133>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem6'
 * '<S134>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem7'
 * '<S135>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Compare To Constant6'
 * '<S136>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic1'
 * '<S137>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic2'
 * '<S138>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic3'
 * '<S139>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic4'
 * '<S140>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic5'
 * '<S141>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic6'
 * '<S142>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic7'
 * '<S143>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Saturation Dynamic8'
 * '<S144>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem'
 * '<S145>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem1'
 * '<S146>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem2'
 * '<S147>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem3'
 * '<S148>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem4'
 * '<S149>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem5'
 * '<S150>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem6'
 * '<S151>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO5/Switch Case Action Subsystem7'
 * '<S152>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Compare To Constant6'
 * '<S153>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic1'
 * '<S154>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic2'
 * '<S155>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic3'
 * '<S156>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic4'
 * '<S157>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic5'
 * '<S158>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic6'
 * '<S159>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic7'
 * '<S160>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Saturation Dynamic8'
 * '<S161>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem'
 * '<S162>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem1'
 * '<S163>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem2'
 * '<S164>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem3'
 * '<S165>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem4'
 * '<S166>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem5'
 * '<S167>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem6'
 * '<S168>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO6/Switch Case Action Subsystem7'
 * '<S169>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Compare To Constant6'
 * '<S170>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic1'
 * '<S171>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic2'
 * '<S172>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic3'
 * '<S173>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic4'
 * '<S174>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic5'
 * '<S175>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic6'
 * '<S176>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic7'
 * '<S177>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Saturation Dynamic8'
 * '<S178>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem'
 * '<S179>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem1'
 * '<S180>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem2'
 * '<S181>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem3'
 * '<S182>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem4'
 * '<S183>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem5'
 * '<S184>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem6'
 * '<S185>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO7/Switch Case Action Subsystem7'
 * '<S186>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Compare To Constant6'
 * '<S187>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic1'
 * '<S188>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic2'
 * '<S189>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic3'
 * '<S190>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic4'
 * '<S191>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic5'
 * '<S192>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic6'
 * '<S193>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic7'
 * '<S194>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Saturation Dynamic8'
 * '<S195>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem'
 * '<S196>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem1'
 * '<S197>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem2'
 * '<S198>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem3'
 * '<S199>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem4'
 * '<S200>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem5'
 * '<S201>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem6'
 * '<S202>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO8/Switch Case Action Subsystem7'
 * '<S203>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Compare To Constant3'
 * '<S204>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Compare To Zero1'
 * '<S205>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic1'
 * '<S206>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic2'
 * '<S207>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic3'
 * '<S208>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic4'
 * '<S209>' : 'MCU_AI/AI_PRO_5ms/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic5'
 * '<S210>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant1'
 * '<S211>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant10'
 * '<S212>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant2'
 * '<S213>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant3'
 * '<S214>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant4'
 * '<S215>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant5'
 * '<S216>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant6'
 * '<S217>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant7'
 * '<S218>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant8'
 * '<S219>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Compare To Constant9'
 * '<S220>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/IsNegative'
 * '<S221>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/IsNegative1'
 * '<S222>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/IsNegative2'
 * '<S223>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/IsNegative3'
 * '<S224>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO'
 * '<S225>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1'
 * '<S226>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2'
 * '<S227>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3'
 * '<S228>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Saturation Dynamic'
 * '<S229>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Saturation Dynamic1'
 * '<S230>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Saturation Dynamic2'
 * '<S231>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/Saturation Dynamic3'
 * '<S232>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO/Compare To Constant1'
 * '<S233>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO/Compare To Constant3'
 * '<S234>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO/IsNegative'
 * '<S235>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO/IsNegative1'
 * '<S236>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO/Saturation Dynamic'
 * '<S237>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO/Saturation Dynamic1'
 * '<S238>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1/Compare To Constant1'
 * '<S239>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1/Compare To Constant3'
 * '<S240>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1/IsNegative'
 * '<S241>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1/IsNegative1'
 * '<S242>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1/Saturation Dynamic'
 * '<S243>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO1/Saturation Dynamic1'
 * '<S244>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2/Compare To Constant1'
 * '<S245>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2/Compare To Constant3'
 * '<S246>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2/IsNegative'
 * '<S247>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2/IsNegative1'
 * '<S248>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2/Saturation Dynamic'
 * '<S249>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO2/Saturation Dynamic1'
 * '<S250>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3/Compare To Constant1'
 * '<S251>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3/Compare To Constant3'
 * '<S252>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3/IsNegative'
 * '<S253>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3/IsNegative1'
 * '<S254>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3/Saturation Dynamic'
 * '<S255>' : 'MCU_AI/AI_PRO_5ms/PCS_POWER_CALC_PRO/PCS_CHAN_POWER_CALC_PRO3/Saturation Dynamic1'
 * '<S256>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Constant11'
 * '<S257>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero'
 * '<S258>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero1'
 * '<S259>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero2'
 * '<S260>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero3'
 * '<S261>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero4'
 * '<S262>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero5'
 * '<S263>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero6'
 * '<S264>' : 'MCU_AI/AI_PRO_5ms/SOC_CALC_PRO/Compare To Zero7'
 * '<S265>' : 'MCU_AI/AI_PRO_5ms1/Compare To Constant'
 * '<S266>' : 'MCU_AI/AI_PRO_5ms1/Compare To Constant1'
 * '<S267>' : 'MCU_AI/AI_PRO_5ms1/Compare To Constant2'
 * '<S268>' : 'MCU_AI/AI_PRO_5ms1/Compare To Zero'
 * '<S269>' : 'MCU_AI/AI_PRO_5ms1/MAX_CHARGE1_CALC_PRO'
 * '<S270>' : 'MCU_AI/AI_PRO_5ms1/MAX_CHARGE2_CALC_PRO'
 * '<S271>' : 'MCU_AI/AI_PRO_5ms1/MAX_CHARGE3_CALC_PRO'
 * '<S272>' : 'MCU_AI/AI_PRO_5ms1/MAX_CHARGE4_CALC_PRO'
 * '<S273>' : 'MCU_AI/AI_PRO_5ms1/MAX_DISCHARGE1_CALC_PRO'
 * '<S274>' : 'MCU_AI/AI_PRO_5ms1/MAX_DISCHARGE2_CALC_PRO'
 * '<S275>' : 'MCU_AI/AI_PRO_5ms1/MAX_DISCHARGE3_CALC_PRO'
 * '<S276>' : 'MCU_AI/AI_PRO_5ms1/MAX_DISCHARGE4_CALC_PRO'
 * '<S277>' : 'MCU_AI/AI_PRO_5ms1/MV_POWER_CALC_PRO'
 * '<S278>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO'
 * '<S279>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO'
 * '<S280>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO'
 * '<S281>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO'
 * '<S282>' : 'MCU_AI/AI_PRO_5ms1/SOC_CALC_PRO'
 * '<S283>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/Compare To Constant1'
 * '<S284>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/Compare To Constant2'
 * '<S285>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/Compare To Constant3'
 * '<S286>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/Compare To Constant4'
 * '<S287>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/Compare To Constant6'
 * '<S288>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/Compare To Constant9'
 * '<S289>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/IsNegative'
 * '<S290>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/IsNegative1'
 * '<S291>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/IsNegative2'
 * '<S292>' : 'MCU_AI/AI_PRO_5ms1/PCS_LIM_POWER_CALC_PRO/IsNegative3'
 * '<S293>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/Compare To Constant1'
 * '<S294>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/Compare To Constant2'
 * '<S295>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/Compare To Constant3'
 * '<S296>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/Compare To Constant4'
 * '<S297>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1'
 * '<S298>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2'
 * '<S299>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3'
 * '<S300>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4'
 * '<S301>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Compare To Constant6'
 * '<S302>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic4'
 * '<S303>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic6'
 * '<S304>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic7'
 * '<S305>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Saturation Dynamic8'
 * '<S306>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem'
 * '<S307>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem1'
 * '<S308>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem2'
 * '<S309>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO1/Switch Case Action Subsystem3'
 * '<S310>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Compare To Constant6'
 * '<S311>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic4'
 * '<S312>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic6'
 * '<S313>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic7'
 * '<S314>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Saturation Dynamic8'
 * '<S315>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem'
 * '<S316>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem1'
 * '<S317>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem2'
 * '<S318>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO2/Switch Case Action Subsystem3'
 * '<S319>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Compare To Constant6'
 * '<S320>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic4'
 * '<S321>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic6'
 * '<S322>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic7'
 * '<S323>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Saturation Dynamic8'
 * '<S324>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem'
 * '<S325>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem1'
 * '<S326>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem2'
 * '<S327>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO3/Switch Case Action Subsystem3'
 * '<S328>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Compare To Constant6'
 * '<S329>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic4'
 * '<S330>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic6'
 * '<S331>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic7'
 * '<S332>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Saturation Dynamic8'
 * '<S333>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem'
 * '<S334>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem1'
 * '<S335>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem2'
 * '<S336>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_POWER_CALC_PRO/SORT_CALC_PRO4/Switch Case Action Subsystem3'
 * '<S337>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Compare To Constant3'
 * '<S338>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Compare To Zero1'
 * '<S339>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic1'
 * '<S340>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic2'
 * '<S341>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic3'
 * '<S342>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic4'
 * '<S343>' : 'MCU_AI/AI_PRO_5ms1/PCS_OUT_R_POWER_CALC_PRO/Saturation Dynamic5'
 * '<S344>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Compare To Constant1'
 * '<S345>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Compare To Constant2'
 * '<S346>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Compare To Constant3'
 * '<S347>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Compare To Constant4'
 * '<S348>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Compare To Constant5'
 * '<S349>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Compare To Constant6'
 * '<S350>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/IsNegative'
 * '<S351>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/IsNegative1'
 * '<S352>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/IsNegative2'
 * '<S353>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/IsNegative3'
 * '<S354>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Saturation Dynamic'
 * '<S355>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Saturation Dynamic1'
 * '<S356>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Saturation Dynamic2'
 * '<S357>' : 'MCU_AI/AI_PRO_5ms1/PCS_POWER_CALC_PRO/Saturation Dynamic3'
 * '<S358>' : 'MCU_AI/AI_PRO_5ms1/SOC_CALC_PRO/Compare To Constant11'
 * '<S359>' : 'MCU_AI/AI_PRO_5ms1/SOC_CALC_PRO/Compare To Zero'
 * '<S360>' : 'MCU_AI/AI_PRO_5ms1/SOC_CALC_PRO/Compare To Zero1'
 * '<S361>' : 'MCU_AI/AI_PRO_5ms1/SOC_CALC_PRO/Compare To Zero2'
 * '<S362>' : 'MCU_AI/AI_PRO_5ms1/SOC_CALC_PRO/Compare To Zero3'
 */
#endif                                 /* MCU_AI_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

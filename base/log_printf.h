#ifndef LOG_PRINTF_H
#define LOG_PRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include  "log.h"
#include  "app_config.h"
#include "system.h"





void BMS_Warn_Printf(INT16U lv_lbit,INT16U lv_hbit,INT8U bms_num,INT8U level);
void PCS_KEHUA_Warn_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level);
void PCS_KEHUA_Fault_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level);
void PCS_TRINA_Warn_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level);
void PCS_TRINA_Fault_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level);
#endif
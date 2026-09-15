/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : address_map.h
 * Project :
 * Description : 
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified :
 * Modified By :
 * ------------------------------------------------------------
 * HISTORY :
 */

 #ifndef TAIDA_LC_EMS_H
 #define TAIDA_LC_EMS_H
#include "system.h"
#define BASE_ADDR   27000
#define STEP        300
#define INPUT_BASE_ADDR   17000
#define INPUT_GROUP_STEP  300
 int LC_EMS_Convert_Address(int pcsnum, int addr);
void LC_EMS_Calc_Power_Percent(int pcsnum);
void Update_Input_Bit_Status(int system_num);
void Update_Slave_sysstate2(uint16_t Slave_sys_state2_value,uint16_t addr);
#define POWER_DEFAULT_RATIO 250
 #endif 
 
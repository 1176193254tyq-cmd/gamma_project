/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : timed.h
 * Project :
 * Description : 定时器、超时判定
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 增加系统系统断电超时和UPS断电超时两个判断函数
 * ------------------------------------------------------------
 * HISTORY : 2022/10/26 修改文件名和宏定义、调整task函数的结构
 */

#ifndef TIMED_CHECK_H
#define TIMED_CHECK_H

#include "system.h"
#include <stdint.h>


INT8U Get_BMS_Comm(INT8U num);

/**
 * @brief 设置BMS通讯状态
 * @param[in] num 定时器序号 第几个bms,0代表第一台
 * @param[in] status 状态 0-正常，1-通讯故障
 * @param[in] isreset 是否做定时器复位 
 */
void Set_BMS_Comm(INT8U num, INT8U status,INT8U isreset);

/**
 * @brief 返回EMS通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_EMS_Comm(INT8U num);

/**
 * @brief 设置EMS通讯状态
 * @param[in] num 定时器序号 第几个测控装置
 * @param[in] status 状态 0-正常，1-通讯故障
 * @param[in] isreset 是否做定时器复位 
 */
void Set_EMS_Comm(INT8U num, INT8U status,INT8U isreset);

/**
 * @brief 返回DI设备通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_DI_Comm(INT8U num);

/**
 * @brief 设置DI设备通讯状态
 * @param[in] num 定时器序号 第几个di
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_DI_Comm(INT8U num, INT8U status);

/**
 * @brief 返回DIDO设备通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_DIDO_Comm(INT8U num);

/**
 * @brief 设置DI设备通讯状态
 * @param[in] num 定时器序号 第几个dido
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_DIDO_Comm(INT8U num, INT8U status);


/**
 * @brief 返回温湿度传感器通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_TEMP_Comm(INT8U num);

/**
 * @brief 设置温湿度传感器通讯状态
 * @param[in] num 定时器序号 第几个temp
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_TEMP_Comm(INT8U num, INT8U status);

/**
 * @brief 返回测控装置通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_MEASU_Comm(INT8U num);

/**
 * @brief 设置测控装置通讯状态
 * @param[in] num 定时器序号 第几个测控装置
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_MEASU_Comm(INT8U num, INT8U status);

/**
 * @brief 返回气体传感器通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_GAS_Comm(INT8U num);

/**
 * @brief 设置气体传感器装置通讯状态
 * @param[in] num 定时器序号 第几个测控装置
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_GAS_Comm(INT8U num, INT8U status);

/**
 * @brief 返回UPS通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_UPS_Comm(INT8U num);

/**
 * @brief 设置UPS通讯状态
 * @param[in] num 定时器序号 第几个ups
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_UPS_Comm(INT8U num, INT8U status);


/**
 * @brief 定时器线程
 */
void Task_Timed_Check(void);

/**
 * @brief 返回AC_METER通讯状态
 * @param[in] num 定时器序号
 * @return 状态
 */
INT8U Get_AC_METER_Comm(INT8U num);

/**
 * @brief 设置AC_METER通讯状态
 * @param[in] num 定时器序号 第几个ac meter
 * @param[in] status 状态 0-正常，1-通讯故障
 */
void Set_AC_METER_Comm(INT8U num, INT8U status);
void Set_PCS_Comm(INT8U num, INT8U status,INT8U isreset);
INT8U Get_PCS_Comm(INT8U num);
/***************************************超时检查的宏定义************************************/
#define TIMED_CHECK_OUT_BOUNDS    2  //返回定时器状态的数组越界结果


#endif //TIMED_CHECK_H

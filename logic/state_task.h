/* ------------------------------------------------------------
 * Copyright (C) 2024
 * File Name : state_task.h
 * Project : DCDC 程序 DCDC program
 * Description : 储能系统内部子系统运行基础版 Energy storage system internal subsystem operation basic version
 * File Created : 2024/01/29
 * Author : jia.liu
 */

#ifndef STATE_TASK_H
#define STATE_TASK_H

#include "state_sys.h"
#include "state_sub.h"
#define SUBSYSTEM_DEBUG_PRINTF 0 
/**
 * @brief 系统级状态机、子系统级状态、小系统级状态机轮转任务
 * System-level state machine, subsystem-level state, small system-level state machine rotation task
 * @param[in] null
 * @retval null
 */
void State_Task(void);
void Trina_State_Task(void);
//extern volatile SUB_State_ENUM SUB_state[MAX_SUB_NUM];
//lj 20260107 此处需要核对
extern volatile SUB_State_ENUM SUB_state[MAX_SYS_NUM*MAX_SUB_NUM];
extern volatile SYS_State_ENUM SYS_state[MAX_SYS_NUM];
#endif //STATE_TASK_H

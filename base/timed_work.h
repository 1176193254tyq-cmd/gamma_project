/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : timed.h
 * Project :
 * Description : 定时器、超时判定
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified :
 * Modified By :
 * ------------------------------------------------------------
 * HISTORY :
 */

#ifndef TIMED_WORK_H
#define TIMED_WORK_H

/**
 * @brief 定时工作任务集合
 */
void Task_Timed_Work(void);
long GetMSecsSinceEpoch();
const char* get_current_time();
#endif //TIMED_WORK_H

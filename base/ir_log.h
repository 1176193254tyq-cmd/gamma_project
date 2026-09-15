#ifndef IR_LOG_H
#define IR_LOG_H

#include "main.h"

typedef enum
{
    SW_INIT = 0,
    SW_OPEN = 1,
    SW_OPEN_2_CLOSE = 2,
    SW_CLOSE = 3,
    SW_CLOSE_2_OPEN = 4,
}BMS_State_ENUM;


void IR_LOG_Record_Task(void);

#endif
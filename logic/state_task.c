/* ------------------------------------------------------------
 * Copyright (C) 2026
 * File Name : state_task.h
 * Project : 
 * Description : 储能系统内部子系统运行
 * File Created : 2026/06/8
 * Author : tyq
 */

#include "main.h"
#include "log.h"
#include "timed_check.h"
#include "state_task.h"
#include "state_sys_trina.h"
#include <stdarg.h>
#include "blackstart_sys.h"
volatile SUB_State_ENUM SUB_state[MAX_SYS_NUM*MAX_SUB_NUM]={0};
volatile SYS_State_ENUM SYS_state[MAX_SYS_NUM];
extern  INT8U BusType;
uint8_t black_start = 0;
uint8_t black_end   = 0;
void Check_Black_State(void)
{
    static uint8_t last_state = 0;

    uint8_t current_state = 0;

    /* 当前状态 */
    if ((GET_HOLD(27000 + 300 * 0 + 21) == 1) ||
        (GET_HOLD(27000 + 300 * 1 + 21) == 1))
    {
        current_state = 1;
    }
    else
    {
        current_state = 0;
    }

    /* 0 -> 1 */
    if ((last_state == 0) && (current_state == 1))
    {
        black_start = 1;
        LOG_INFO("黑启动开始");
    }

    /* 1 -> 0 */
    if ((last_state == 1) && (current_state == 0))
    {
        black_end = 1;
         LOG_INFO("黑启动关闭");
    }

    /* 更新上一次状态 */
    last_state = current_state;
}
/**
 * @brief 系统级状态机、子系统级状态、小系统级状态机轮转任务
 * System-level state machine, subsystem-level state, small system-level state machine rotation task
 * @param[in] null
 * @retval null
 */

void State_Task(void)
{
    INT8U sub_num,sys_num;
    sysPara *sys_cfg = SysConf_GetInfo();
    while(1)
    {
        //LOG_INFO("黑启动启动！！！");
        Check_Black_State();
        if((GET_HOLD(27000+300*0+21)==0)&&(GET_HOLD(27000+300*1+21)==0))
        {
         for(sys_num=0;(sys_num < sys_cfg->sysNum);sys_num++)
        {
            for(sub_num=0; (sub_num < sys_cfg->subNum) && (sub_num<MAX_SUB_NUM);sub_num++)
            {
              if (!SUB_ENABLED_SYS(sys_num, sub_num)) continue;
                SUB_State_Run(SUB_state,sys_num,sub_num);  
                
            }       
            SYS_State_Run(SYS_state, sys_num);
        }    
        }
        else if ((GET_HOLD(27000+300*0+21)==1)||(GET_HOLD(27000+300*1+21)==1))
        {
            //LOG_INFO("黑启动启动！！！");
           for(sys_num=0;(sys_num < sys_cfg->sysNum);sys_num++)
        {         
            SYS_State_BlackStart_Run(SYS_state, sys_num);
        }  

        }
     
          if(BusType==S_BUS)
            {
                system_value();
                
            }
            else
            {
                system_value1();
          
            }
 
        usleep(2 * 1000);      //轮询周期 Polling cycle
    }
  
}
void Trina_State_Task(void)
{
    INT8U sub_num,sys_num;
    sysPara *sys_cfg = SysConf_GetInfo();
    while(1)
    {        
        for(sys_num=0;(sys_num < sys_cfg->sysNum);sys_num++)
        {
            for(sub_num=0; (sub_num < sys_cfg->subNum) && (sub_num<MAX_SUB_NUM);sub_num++)
            {      
                SUB_State_Run(SUB_state,sys_num,sub_num);                  
            }
         
            SYS_Trina_State_Run(SYS_state, sys_num);
        }
    //   if(BusType==S_BUS)
    //         {
    //             system_value();
                
    //         }
    //         else
    //         {
    //             system_value1();
          
    //         }
        usleep(10 * 1000);      //轮询周期 Polling cycle
    }
  
}

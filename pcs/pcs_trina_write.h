#ifndef PCS_TRINA_WRITE_H
#define PCS_TRINA_WRITE_H
#include "system.h"
#include "main.h"
#define PCS_BUFF_LEN        (300)   //PCS收发缓存长度

#define PCS_Trina_SLAVE_ADDR   (1)
#define PCS_Trina_SLAVE_J1_ADDR   (2)
#define PCS_Trina_SLAVE_J2_ADDR   (3)
#define Master_Discharge_Power_Coef    (3033)//主从机下发功率分配Limitation:0~1000, default:500
#define Slave_Discharge_Power_Coef    (3035)//主从机下发功率分配Limitation:0~1000, default:500



#define Master_Charge_Power_Coef  (3034) //
#define Slave_Charge_Power_Coef  (3036) //

#define Trina_Reactive_Power_Addr  (111) //
#define Trina_Active_Power_Addr  (3004)
#define Trina_Reactive_Addr  (3261)//无功调节
#define Trina_Reactive_Rate_Addr  (3264)//无功比例
#define MVPower                        (1010)
#define LIMIT(x, min, max) \
    (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))
void  PCS_TRina_write_Task(const char *_num);


#define TIMEOUT_MS 0  




#endif
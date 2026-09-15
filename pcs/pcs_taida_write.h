#ifndef PCS_TAIDA_WRITE_H
#define PCS_TAIDA_WRITE_H
#include "system.h"

#define PCS_BUFF_LEN        (300)   //PCS收发缓存长度

#define Master_Bat1_Discharge_Power_Coef    (40105-40001)//主从机下发功率分配Limitation:0~1000, default:500
#define Master_Bat2_Discharge_Power_Coef    (40106-40001)//主从机下发功率分配Limitation:0~1000, default:500

#define Time_Set_Addr                       (40011-40001)//时间设置 

#define Slave_Bat1_Discharge_Power_Coef    (40107-40001)//主从机下发功率分配Limitation:0~1000, default:500

#define Master_Bat1_Charge_Power_Coef  (40108-40001) //

#define Master_Bat2_Charge_Power_Coef  (40109-40001) //
#define Slave_Bat1_Charge_Power_Coef  (40110-40001) //

#define Reactive_Power_Addr  (111) //
#define Active_Power_Addr  (110)
#define MVPower                        (1010)



void  PCS_TAida_write_Task(const char *_num);
int Controlword_Bit(uint16_t address, uint8_t val01,int num);
int PCS_HB_start(int socket, int id, INT16U hb_reg_addr,int num);

typedef struct {
    INT16S P1_out;         // 下发给 PCS1 的功率（放电正、充电负）
    INT16S P2_out;         // 下发给 PCS2 的功率
    INT16S P3_out;         // 下发给 PCS3 的功率（放电正、充电负）
    INT16S P4_out;         // 下发给 PCS4 的功率
    INT16S P5_out;         // 下发给 PCS5 的功率（放电正、充电负）
    INT16S P6_out;         // 下发给 PCS6 的功率
    INT16S P7_out;         // 下发给 PCS7 的功率（放电正、充电负）
    INT16S P8_out;         // 下发给 PCS8 的功率

    INT16S master1bat1_powerate;     // 对主PCS1 的分配比例
    INT16S master1bat2_powerate;     
    INT16S slave1bat1_powerate;  
    INT16S slave1bat2_powerate; 
    INT16S master2bat1_powerate;     // 对主PCS2 的分配比例
    INT16S master2bat2_powerate;     
    INT16S slave2bat1_powerate;  
    INT16S slave2bat2_powerate;  
    INT16S pcs_r_power1_2;
    INT16S pcs_r_power3_4;
    INT16S unmet;   
    INT16S pcs_r_rate1_2;
    INT16S pcs_r_rate3_4;   
    
} Power_Divider_result_t;

typedef struct {

    INT16S S;
    INT16S PF;
        
} PF_CONVERT_t;


typedef struct {
    INT16U address;   /* 逻辑点位号，比如 12012 */
    INT16U reg_addr;   /* 目标保持寄存器地址（显示地址，如 40012） */
    INT8U  bit;        /* 目标 bit 位号：0..15 */
    INT8U  invert;     /* 0: 正常；1: 逻辑取反（点位=1→清零该位）*/
    INT8U  width;      /* 位宽，=1 表示单 bit；>1 可做多位域（可选扩展）*/
} MapEntry;

 struct {
    int socket;
    int id;
    INT16U hb_reg_addr;
    pthread_t th;
    volatile bool running;
    int num;
} PCShb;

typedef struct {
    bool inited;

    INT16S lastP_raw;
    INT16S lastPF_raw;

    INT16S pendP_raw;
    INT16S pendPF_raw;

    INT8U  seen_mask;
    uint64_t t_start_ms;
} PfLatch;

typedef struct
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
}SYS_TIME_BUF;




#endif
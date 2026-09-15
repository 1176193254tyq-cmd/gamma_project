#define _POSIX_C_SOURCE 200809L
#include <stdint.h>           // 
#include <pthread.h>
#include "time.h"
#include "main.h"
#include "pcs_trina_write.h"
#include "global_mcu_ai.h"
#include "MCU_AI.h"
//#include "MCU_AI1.h"
#include "pcs_write_queue.h" 

#include "pf_convert_final.h"
extern INT8U BusType;
int socket_Trina_Pcs[10]={0};//pcs socket
int socket_Trina_Heart_Pcs[10]={0};//pcs heart socket

/*EMS/Web下达指令时的全局变量）*/
extern INT8U  BMSnum;
extern BOOLEAN PCS_Write_Flag[10];
extern INT32S  PCSWrite_Lenth[10];
extern int  PCSaddr[10];
extern int  PCSvalue[10];
extern BOOLEAN MV_Power_Flag;
extern INT32S PCSactivepower[10];
extern INT32S PCSPF[10];
int pcspowerallocate[20]; //记录PCS的功率分配的数值

static char LogStr[100] = {0};

static uint8_t  init_power_flag=0;
extern volatile BOOLEAN Set_Time_Flag;//用于记录是否设置时间标志
/*********************** */

/*MV_Power_allocate 相关外部变量*/
extern uint16_T bms1_max_charge_power; /* '<Root>/bms1_max_charge_power' */
extern uint16_T bms1_max_discharge_power;/* '<Root>/bms1_max_discharge_power' */
extern uint16_T bms1_soc;              /* '<Root>/bms1_soc' */
extern uint16_T bms2_max_charge_power; /* '<Root>/bms2_max_charge_power' */
extern uint16_T bms2_max_discharge_power;/* '<Root>/bms2_max_discharge_power' */
extern uint16_T bms2_soc;              /* '<Root>/bms2_soc' */
extern uint16_T bms3_max_charge_power; /* '<Root>/bms3_max_charge_power ' */
extern uint16_T bms3_max_discharge_power;/* '<Root>/bms3_max_discharge_power' */
extern uint16_T bms3_soc;              /* '<Root>/bms3_soc' */
extern uint16_T bms4_max_charge_power; /* '<Root>/bms4_max_charge_power' */
extern uint16_T bms4_max_discharge_power;/* '<Root>/bms4_max_discharge_power' */
extern uint16_T bms4_soc;              /* '<Root>/bms4_soc' */
extern uint16_T bms5_max_charge_power; /* '<Root>/bms5_max_charge_power' */
extern uint16_T bms5_max_discharge_power;/* '<Root>/bms5_max_discharge_power' */
extern uint16_T bms5_soc;              /* '<Root>/bms5_soc' */
extern uint16_T bms6_max_charge_power; /* '<Root>/bms6_max_charge_power' */
extern uint16_T bms6_max_discharge_power;/* '<Root>/bms6_max_discharge_power' */
extern uint16_T bms6_soc;              /* '<Root>/bms6_soc' */
extern uint16_T bms7_max_charge_power; /* '<Root>/bms7_max_charge_power' */
extern uint16_T bms7_max_discharge_power;/* '<Root>/bms7_max_discharge_power' */
extern uint16_T bms7_soc;              /* '<Root>/bms7_soc' */
extern uint16_T bms8_max_charge_power; /* '<Root>/bms8_max_charge_power' */
extern uint16_T bms8_max_discharge_power;/* '<Root>/bms8_max_discharge_power' */
extern uint16_T bms8_soc;              /* '<Root>/bms8_soc' */
extern uint16_T bus2_or_bus1;          /* '<Root>/bus2_or_bus1' */
extern uint16_T module_num;            /* '<Root>/module_num' */
extern uint16_T mv_max_power;          /* '<Root>/mv_max_power' */
extern int16_T mv_power;               /* '<Root>/mv_power' */
extern int16_T mv_r_power;             /* '<Root>/mv_r_power' */
extern int16_T pcs1_power;             /* '<Root>/pcs1_power' */
extern uint16_T pcs1_rated_power;      /* '<Root>/pcs1_rated_power' */
extern int16_T pcs2_power;             /* '<Root>/pcs2_power' */
extern uint16_T pcs2_rated_power;      /* '<Root>/pcs2_rated_power' */
extern int16_T pcs3_power;             /* '<Root>/pcs3_power' */
extern uint16_T pcs3_rated_power;      /* '<Root>/pcs3_rated_power' */
extern int16_T pcs4_power;             /* '<Root>/pcs4_power' */
extern uint16_T pcs4_rated_power;      /* '<Root>/pcs4_rated_power' */
extern int16_T pcs5_power;             /* '<Root>/pcs5_power' */
extern int16_T pcs6_power;             /* '<Root>/pcs6_power' */
extern int16_T pcs7_power;             /* '<Root>/pcs7_power' */
extern int16_T pcs8_power;             /* '<Root>/pcs8_power' */
extern int16_T pcs_r_power1_2;         /* '<Root>/pcs_r_power1_2' */
extern int16_T pcs_r_power3_4;         /* '<Root>/pcs_r_power3_4' */
extern uint16_T target_h_soc;          /* '<Root>/target_h_soc' */
extern uint16_T target_l_soc;          /* '<Root>/target_l_soc' */

/********************** */

/* 前置声明 */
static void Write_Flags_to_Queue(int num);
static void PCS_Trina_Write(int num);
static void PCS_Trina_Write_DataProcess(unsigned char *ptr, int num);

extern void MCU_AI_step(void);
extern void MCU_AI_step1(void);
void* Pcs_Trina_Write_Task(void *arg);
void  G3_MV_Power_allocate1(void);
void* PCS_Trina_HB_Task(void*arg);

Power_Divider_result_t G3_Power_Divider();
Power_Divider_result_t G3_Power_Result = {0};
Power_Divider_result_t G3_Power_Divider1();
static pthread_mutex_t g_power_mtx = PTHREAD_MUTEX_INITIALIZER;//功率互斥锁,变量资源保护
static pthread_mutex_t g_mv_alloc_call_mtx = PTHREAD_MUTEX_INITIALIZER;//避免MV分配逻辑被双线程并发重入
static pthread_mutex_t g_mv_alloc_pf_mtx = PTHREAD_MUTEX_INITIALIZER;//避免MV分配逻辑被双线程并发重入

/**
 * @brief 将十六进制数据转换为字符串格式并追加到输出缓冲区
 * 
 * 该函数将输入的二进制数据转换为十六进制字符串格式(每个字节转换为"xx "格式)，
 * 并追加到输出缓冲区的现有内容后面。函数会处理特殊字符如\t、\r或\n。
 * 
 * @param out 输出缓冲区，用于存储转换后的字符串
 * @param out_size 输出缓冲区的大小(包括终止符'\0')
 * @param buf 输入二进制数据缓冲区
 * @param len 输入二进制数据的长度
 * 
 * @note 函数会检查输出缓冲区是否已满，并确保不会发生缓冲区溢出
 * @note 每个字节会被转换为3个字符(2个十六进制数字+1个空格)
 * @note 如果输出缓冲区空间不足，函数会提前终止转换
 */
static void Hex_To_String(char *out, size_t out_size, const unsigned char *buf, size_t len)
{
    size_t used = strlen(out);
    if (out_size == 0 || used >= out_size - 1) {
        return;
    }
    for (size_t i = 0; i < len && used < out_size - 1; i++) {
        int n = snprintf(out + used, out_size - used, "%02x ", buf[i]);
        if (n < 0) {
            break;
        }
        if ((size_t)n >= out_size - used) {
            used = out_size - 1;
            break;
        }
        used += (size_t)n;
    }
}
/**
*
*最大充放电
*
*/

static inline int16_t clamp_power(int32_t value)
{
    if (value > 1250) return 1250;
    if (value < -1250) return -1250;
    return (int16_t)value;
}
/**
 * @brief 将PCS写入标志位对应的写入请求加入队列
 * 
 * 该函数根据输入的设备编号(num)，检查对应的PCS写入标志位(PCS_Write_Flag)。
 * 若标志位有效，则根据设备类型构建写入请求，并将其加入写入队列。
 * 
 * @param num 设备编号，取值范围为0-3
 * 
 * @note 函数内部使用互斥锁(g_power_mtx)保护共享数据
 * @note 对于长度大于1的写入请求，会拆分为多条单寄存器写入
 * @note 特殊处理：当value为0且addr非负时，读取保持寄存器值
 * 
 * @warning 函数不处理num超出范围的情况，直接返回
 * @warning 函数会修改全局变量PCS_Write_Flag[num]
 */
static void Write_Flags_to_Queue(int num)
{
    if (num < 0 || num >= 4) return;

    pthread_mutex_lock(&g_power_mtx);
    if (!PCS_Write_Flag[num]) {
        pthread_mutex_unlock(&g_power_mtx);
        return;
    }
    int addr  = PCSaddr[num];
    int len   = (num < 2) ? PCSWrite_Lenth[num] : 1; 
    int value = PCSvalue[num];

    PCS_Write_Flag[num] = false;

    if (len > 1) {
        /* 设备只支持 0x06：拆分为多条单寄存器写入 */
        for (int i = 0; i < len; ++i) {
            INT16U v = GET_HOLD(addr + i);
            PcsWriteReq req10;
            req10.addr = (INT16U)(addr + i),
            req10.value = v,
            req10.len = 1,
            req10.is_multi = false;
        Pcs_Write_Enqueue_Dedup_By_Addr(num, &req10);
    }
    } else {
        INT16U v;
        
        if (value == 0 && (addr >= 0)) v = GET_HOLD(addr);
        else v = (INT16U)value;

        PcsWriteReq req06;
            req06.addr = (INT16U)addr,
            req06.value = v,
            req06.len = 1,
            req06.is_multi = false;
        Pcs_Write_Enqueue_Dedup_By_Addr(num, &req06);
    }
    pthread_mutex_unlock(&g_power_mtx);
}

/* -------------------- 写入执行：用 0x06 -------------------- */
/**
 * @brief 向台达PCS设备写入数据
 * 
 * 该函数负责处理向台达PCS设备的各种写入请求，包括功率写入、控制字写入和常规寄存器写入。
 * 函数会从写入队列中取出请求并逐个处理，支持特殊地址的特殊处理逻辑。
 * 
 * @param num PCS设备编号，用于标识不同的PCS设备
 *            - 0: 第一个PCS设备
 *            - 1: 第二个PCS设备
 *            - 2: 第三个PCS设备
 *            - 3: 第四个PCS设备
 * 
 * @note 特殊处理说明：
 *       - 地址1010：功率写入，需要先发送系数寄存器再写功率
 *       - 地址27000~27045或27300~27345：控制字写入，使用位映射写整字
 *       - 其他地址：常规单寄存器写入(0x06功能码)
 * 
 * @note 功率因数转换：
 *       - 当PFConvert_Write_Flag标志位被设置时，会执行功率因数转换参数的写入
 *       - 写入参数包括视在功率(S)和功率因数(PF)
 * 
 * @return 无返回值
 */
static void PCS_Trina_Write(int num)
{
    /* 先把旧标志转为队列，保证兼容 */
    Write_Flags_to_Queue(num);
    INT8U pcs_num_mask=0; //PCS数量掩码，用于映射PCS实际设备地址
    PcsWriteReq req;
    INT16U send_power_cmd_buf[10] = {0};
    while (pcs_write_try_dequeue(num, &req)) {
        INT16U addr  = req.addr;
        INT16U value = req.value;

        /* 特殊：功率写入（27107 / 27407），先发送系数寄存器再写功率 */
        if (addr == 1010) {
            if(BusType==S_BUS)
            {
                // G3_Power_Divider();
                if (num == 0) {
             

                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, G3_Power_Result.P1_out + G3_Power_Result.P2_out, CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate1_2, CMD_DELAY_5);//无功比例

                }
                else if (num == 1) {
               
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, (INT16U)(G3_Power_Result.P3_out + G3_Power_Result.P4_out), CMD_DELAY_5);
                    // Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                    //     Reactive_Power_Addr, G3_Power_Result.pcs_r_power3_4, CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate1_2, CMD_DELAY_5);//无功比例                 
                }

                else if (num == 2) {
               
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, (INT16U)(G3_Power_Result.P5_out + G3_Power_Result.P6_out), CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate3_4,CMD_DELAY_5);//无功比例
                }

                else if (num == 3) {
               
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, (INT16U)(G3_Power_Result.P7_out + G3_Power_Result.P8_out), CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate3_4,CMD_DELAY_5);//无功比例
                }


            }
            else
            {
                G3_Power_Divider1();
                if (num == 0) 
                {
                    if ((G3_Power_Result.P1_out + G3_Power_Result.P2_out) < 0) 
                    {
    
                        send_power_cmd_buf[0]=500;
                        send_power_cmd_buf[1]=G3_Power_Result.master1bat1_powerate;
                        send_power_cmd_buf[2]=500;
                        send_power_cmd_buf[3]=(INT16U)G3_Power_Result.master1bat2_powerate;                    
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);                        
                            
                    } 
                    else
                    {
   
                        send_power_cmd_buf[0]=(INT16U)G3_Power_Result.master1bat1_powerate;
                        send_power_cmd_buf[1]=500;
                        send_power_cmd_buf[2]=(INT16U)G3_Power_Result.master1bat2_powerate;
                        send_power_cmd_buf[3]=500;
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);

                    }
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, G3_Power_Result.P1_out + G3_Power_Result.P2_out, CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate1_2,CMD_DELAY_5);//无功比例
                }           
                else if (num == 1) 
                {
                   if ((G3_Power_Result.P3_out + G3_Power_Result.P4_out) < 0) 
                    {
    
                        send_power_cmd_buf[0]=500;
                        send_power_cmd_buf[1]=G3_Power_Result.slave1bat1_powerate;
                        send_power_cmd_buf[2]=500;
                        send_power_cmd_buf[3]=(INT16U)G3_Power_Result.slave1bat2_powerate;                    
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);                        
                            
                    } 
                    else
                    {
   
                        send_power_cmd_buf[0]=(INT16U)G3_Power_Result.slave1bat1_powerate;
                        send_power_cmd_buf[1]=500;
                        send_power_cmd_buf[2]=(INT16U)G3_Power_Result.slave1bat2_powerate;
                        send_power_cmd_buf[3]=500;
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);

                    }
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, G3_Power_Result.P3_out + G3_Power_Result.P4_out, CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate1_2,CMD_DELAY_5);//无功比例
                }
                else if (num == 2) 
                {
                   if ((G3_Power_Result.P5_out + G3_Power_Result.P6_out) < 0) 
                    {
    
                        send_power_cmd_buf[0]=500;
                        send_power_cmd_buf[1]=G3_Power_Result.master2bat1_powerate;
                        send_power_cmd_buf[2]=500;
                        send_power_cmd_buf[3]=(INT16U)G3_Power_Result.master2bat2_powerate;                    
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);                        
                            
                    } 
                    else
                    {
   
                        send_power_cmd_buf[0]=(INT16U)G3_Power_Result.master2bat1_powerate;
                        send_power_cmd_buf[1]=500;
                        send_power_cmd_buf[2]=(INT16U)G3_Power_Result.master2bat2_powerate;
                        send_power_cmd_buf[3]=500;
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);

                    }
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, G3_Power_Result.P5_out + G3_Power_Result.P6_out, CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate3_4,CMD_DELAY_5);//无功比例
                }
            else if (num == 3) 
                {
                   if ((G3_Power_Result.P7_out + G3_Power_Result.P8_out) < 0) 
                    {
    
                        send_power_cmd_buf[0]=500;
                        send_power_cmd_buf[1]=G3_Power_Result.slave2bat1_powerate;
                        send_power_cmd_buf[2]=500;
                        send_power_cmd_buf[3]=(INT16U)G3_Power_Result.slave2bat2_powerate;                    
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);                        
                            
                    } 
                    else
                    {
   
                        send_power_cmd_buf[0]=(INT16U)G3_Power_Result.slave2bat1_powerate;
                        send_power_cmd_buf[1]=500;
                        send_power_cmd_buf[2]=(INT16U)G3_Power_Result.slave2bat2_powerate;
                        send_power_cmd_buf[3]=500;
                        Modbus_TCP_Write_Multiple(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR, Master_Discharge_Power_Coef, 4, send_power_cmd_buf, 200);

                    }
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Active_Power_Addr, G3_Power_Result.P7_out + G3_Power_Result.P8_out, CMD_DELAY_5);
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Addr, 5, CMD_DELAY_5);   //无功模式2  + 无功使能                   
                    Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], PCS_Trina_SLAVE_ADDR,
                        Trina_Reactive_Rate_Addr, G3_Power_Result.pcs_r_rate3_4,CMD_DELAY_5);//无功比例
                }
            }          
            continue;
        }

    /* 常规：单寄存器 0x06 根据不同的PCS编号来分配掩码，之后进行地址映射*/

        INT16U idx = Trina_Addr_From_Index(num+1, addr);
        INT32S offset = 700 * num;  
        INT32S base = addr - offset;     
        INT16U slave_addr = 0;
        if ((base >= 12000 + 440 && base <=12000 + 461))
        {
            slave_addr = PCS_Trina_SLAVE_J1_ADDR;
        }
        else if ((base >= 12000 + 480 && base <=12000 + 501))
        {
            slave_addr = PCS_Trina_SLAVE_J2_ADDR;
        }
        else
        {
            slave_addr = PCS_Trina_SLAVE_ADDR;
        }
        Modbus_TCP_Write06_SingleRegist(socket_Trina_Pcs[num], slave_addr, idx, value, CMD_DELAY_5);
        LOG_INFO("PCS[%d] slave addr: %d, 0x06 write ok: hold[%u->idx %d] = %u", num, slave_addr, addr, idx, value);
        
    }


}


/**
 * @brief 处理PCS台达设备的写入数据
 * 
 * 该函数处理来自PCS台达设备的写入数据请求，支持以下功能码：
 * - 0x03: 读保持寄存器
 * - 0x06: 写单个寄存器
 * - 0x10: 写多个寄存器
 * 
 * @param ptr 指向接收数据缓冲区的指针，包含完整的Modbus协议帧
 * @param num 设备编号，用于标识具体的PCS设备
 * 
 * @note 函数内部使用固定长度缓冲区PCS_BUFF_LEN处理数据
 * @note 处理过程中会检查数据有效性，包括数据校验和地址范围
 * @note 对于0x06功能码，会记录日志信息
 * 
 * @warning 调用者需确保ptr指向的缓冲区大小不小于PCS_BUFF_LEN
 * @warning 函数内部会修改ptr指向的数据，建议传入缓冲区的副本
 */
static void PCS_Trina_Write_DataProcess(unsigned char *ptr, int num)
{
    unsigned char pbuf[PCS_BUFF_LEN] = {0};
    u16_conv temp,RegVal;
    int i = 0;
    INT32U sum = 0;
    INT16U startaddr = 0;
    INT16U dataddr = 0;
    bool is_ok = 0;

    memcpy(pbuf, ptr, PCS_BUFF_LEN);

    if (pbuf[7] == 0x03)
    {
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16;
        dataddr = 9;
        is_ok = true;
        sum = 0;

        for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)
        {
            sum += pbuf[i+dataddr];
        }
        if(sum==0) is_ok = false;

        if (startaddr==40100-40001)
        {
            RegVal.D8[1] = pbuf[dataddr++];
            RegVal.D8[0] = pbuf[dataddr++];
        
        }
        else if(startaddr==40102-40001)
        {
            RegVal.D8[1] = pbuf[dataddr++];
            RegVal.D8[0] = pbuf[dataddr++];
        

        }
        else
        {
            is_ok = false;
        }
        if(is_ok == true)
        {
        
        }
    }
    else if (pbuf[7] == 0x06)
    {
        memset(LogStr, 0, sizeof(LogStr));
        temp.D8[1] = pbuf[8]; // 寄存器地址H
        temp.D8[0] = pbuf[9]; // 寄存器地址L
        startaddr = temp.D16; // 寄存器地址
        RegVal.D8[1] = pbuf[10];
        RegVal.D8[0] = pbuf[11];

        // INT16U address=Taida_HOLD_INDEX(num+1,temp.D16);
        // SET_HOLD(address,RegVal.D16);

    Hex_To_String(LogStr, sizeof(LogStr), (const unsigned char *)pbuf, 30);
    LOG_INFO("LC Receive PCS 0x06, address:%d, buffer => %s", temp.D16, LogStr);

    }
    else if (pbuf[7] == 0x10)
    {
    
    }
    else if (pbuf[7] == 0x04)
    {

    }
    else
    {
    //   DebugModbusBuf("PCS Invalid data!\n", pbuf, 30);
    }
}
static void PCS_Trina_Write_Heart_DataProcess(unsigned char *ptr, int num)
{
    unsigned char pbuf[PCS_BUFF_LEN] = {0};
    u16_conv temp,RegVal;
    int i = 0;
    INT32U sum = 0;
    INT16U startaddr = 0;
    INT16U dataddr = 0;
    bool is_ok = 0;

    memcpy(pbuf, ptr, PCS_BUFF_LEN);

    if (pbuf[7] == 0x03)
    {
        temp.D8[1] = pbuf[0];
        temp.D8[0] = pbuf[1];
        startaddr = temp.D16;
        dataddr = 9;
        is_ok = true;
        sum = 0;

        for (i = 0; (i < (pbuf[8]/2)) && ((i*2) < PCS_BUFF_LEN); i++)
        {
            sum += pbuf[i+dataddr];
        }
        if(sum==0) is_ok = false;

        if (startaddr==40100-40001)
        {
            RegVal.D8[1] = pbuf[dataddr++];
            RegVal.D8[0] = pbuf[dataddr++];
        
        }
        else if(startaddr==40102-40001)
        {
            RegVal.D8[1] = pbuf[dataddr++];
            RegVal.D8[0] = pbuf[dataddr++];
    

        }
        else
        {
            is_ok = false;
        }
        if(is_ok == true)
        {
        
        }
    }
    else if (pbuf[7] == 0x06)
    {
        memset(LogStr, 0, sizeof(LogStr));
        temp.D8[1] = pbuf[8]; // 寄存器地址H
        temp.D8[0] = pbuf[9]; // 寄存器地址L
        startaddr = temp.D16; // 寄存器地址
        RegVal.D8[1] = pbuf[10];
        RegVal.D8[0] = pbuf[11];

        INT16U address=Taida_HOLD_INDEX(num+1,temp.D16);
        SET_HOLD(address,RegVal.D16);

    }
    else if (pbuf[7] == 0x10)
    {
    
    }
    else if (pbuf[7] == 0x04)
    {

    }
    else
    {
        DebugModbusBuf("PCS Invalid data!\n", pbuf, 30);
    }
}

/* -------------------- 写线程入口-------------------- */
/**
 * @brief PCS台达写任务线程函数
 * @details 该函数负责与PCS设备建立连接，进行Modbus TCP通信，实现数据读写功能。
 *          包含以下主要功能：
 *          1. 创建并维护与PCS设备的TCP连接
 *          2. 根据系统状态执行不同的读写操作
 *          3. 处理通信故障并自动重连
 *          4. 支持多PCS设备并发控制
 * @param arg 线程参数，转换为PCS设备编号(int)
 * @return void* 线程返回值，始终返回NULL
 */
static inline uint8_t is_run_state(SYS_State_ENUM st)
{
    return (st == SYSRun) || (st == SYSWarnRun);
}
void* Pcs_Trina_Write_Task(void *arg)
{
    sysPara *sys_cfg = SysConf_GetInfo();

    int pcs_num = (int)(intptr_t)arg;   
    INT8U sysnum =0;
    SYS_State_ENUM state;
    // 防越界
    if (pcs_num < 0 || pcs_num >= 10) {
        LOG_INFO("PCS write task got invalid pcs_num=%d", pcs_num);
        return NULL;
    }
    LOG_INFO("PCS write task started for PCS-%d", pcs_num);

    INT8U loop = 0;
    int read_recv_res = 0;
    INT16U timeout_cnt=0;
    uint64_t time_keepalive = 0;
    struct timespec ts;
    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons(sys_cfg->pcs_port[pcs_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->pcs_ip[pcs_num]);
        struct timeval time_out = {0, (1 * 1000)};

        // 创建本地客户端socket
        if ((socket_Trina_Pcs[pcs_num] = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("PCS-%d Create write socket failure! ip:%s[port:%d]",
                    pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
            sleep(5);
            continue;
        }

        LOG_INFO("pcs-%d: write Connect SUCCESS! ip:%s[port:%d]",
                pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
        timeout_cnt=0;    
        if (IsWarn == Get_PCS_Comm(pcs_num)) Set_PCS_Comm(pcs_num, IsNoFault, FALSE);
        else                                 Set_PCS_Comm(pcs_num, IsNoFault, TRUE);


        /*保活时间 ms*/
        clock_gettime(CLOCK_MONOTONIC, &ts);
        time_keepalive = ts.tv_sec * 1000LL + ts.tv_nsec / 1000000;
        while (1) {
        if(GET_INPUT(P2P_mode)==0)
        {               

         PCS_Trina_Write(pcs_num);
  
        int16_t numbytes=Recv_Modbus_Back(socket_Trina_Pcs[pcs_num], pcs_num, PCS_BUFF_LEN, PCS_Trina_Write_DataProcess);
        /*保活机制1s一次,pcs超过30s没有收到报文会断开当前socket*/
        clock_gettime(CLOCK_MONOTONIC, &ts);
        uint64_t now_ms = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
        if (numbytes > 0)
        {
            time_keepalive = now_ms;
        }
        if ((time_keepalive + 1000ULL) < now_ms) {
            LOG_INFO("PCS-%d: 发送keepalive报文 ", pcs_num);
            Modbus_TCP_Read(socket_Trina_Pcs[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04addr2, PCS_Trina_04size2, CMD_DELAY_0);
            time_keepalive = now_ms;
        }
        if (numbytes < 0)  
        {
             LOG_INFO("PCS-%d:其他套接字故障 ", pcs_num);
            close(socket_Trina_Pcs[pcs_num]);
            socket_Trina_Pcs[pcs_num] = -1;
            LOG_INFO("PCS-%d: 连接异常，断开服务端连接 ", pcs_num);
            break;
        }
        else if (numbytes == 0) {
            timeout_cnt++;
            if (timeout_cnt >= 30000) {   //超时时间约为400ms
                LOG_INFO("PCS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                pcs_num, timeout_cnt);
                close(socket_Trina_Pcs[pcs_num]);   
                socket_Trina_Pcs[pcs_num] = -1;            
                break;
            }
        }
        else
        {
            timeout_cnt = 0;
            Set_PCS_Comm(pcs_num, IsNoFault, TRUE);      
        }
        }
        else
        {
            sleep(5);
        }
        }
    
    }  
    close(socket_Trina_Pcs[pcs_num]);
    return NULL;
}

/*主从PCS下发功率分配策略*/
/**
 * @brief 计算并分配功率控制系统的输出功率
 * 
 * 根据系统运行状态和从机故障情况，对PCS(功率转换系统)的有功和无功功率进行分配。
 * 同时计算并设置主从机的BAT1分配比例。
 * 
 * @details 功能说明：
 * 1. 获取系统1和系统2的运行状态
 * 2. 根据不同的运行场景(情况5-7及默认情况)进行功率分配：
 *    - 情况5：系统1运行且从机故障，系统2运行且从机无故障
 *    - 情况6：系统1运行且从机故障，系统2运行且从机故障
 *    - 情况7：系统1运行且从机无故障，系统2运行且从机故障
 *    - 系统切换情况(覆盖情况3、4)
 *    - 默认情况
 * 3. 计算并限制BAT1分配比例在0-1000范围内
 * 4. 记录功率分配结果日志
 * 
 * @note 特殊字符处理：
 * - 支持制表符(\t)、回车符(\r)和换行符(\n)
 * 
 * @return Power_Divider_result_t 包含以下字段的功率分配结果：
 *         - P1_out, P2_out, P3_out, P4_out: 各PCS的有功功率输出
 *         - pcs_r_power1_2, pcs_r_power3_4: PCS组1-2和3-4的无功功率输出
 *         - master1bat1_powerate: 主机1的BAT1分配比例(0-1000)
 *         - master2bat1_powerate: 主机2的BAT1分配比例(0-1000)
 */
Power_Divider_result_t G3_Power_Divider()
{ 
    
    SYS_State_ENUM state[2]; 
    pthread_mutex_lock(&g_power_mtx);

    INT16U PCS1_sys_state = (INT16U)GET_INPUT(2600 + 300 * 0 + 100);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 0) & 0x1u) == 1u);//group1,主机PCS的运行
    
    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(2600 + 300 * 1 + 100);//group1,从机PCS的运行状态

    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 0) & 0x1u) == 1u);//group1,从机PCS的运行


    INT16U PCS2_sys_state = (INT16U)GET_INPUT(2600 + 300 * 2 + 100);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行


    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(2600 + 300 * 3 + 100);//group2,主机PCS的运行状态

    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 0) & 0x1u) == 1u);//group2,从机PCS的运行

/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
 if((master1_Run!=1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run==1))
{

    G3_Power_Result.P1_out=pcs4_power;
    G3_Power_Result.P2_out=pcs1_power;
    G3_Power_Result.P3_out=pcs2_power;
    G3_Power_Result.P4_out=pcs3_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;

}

/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs4_power;
    G3_Power_Result.P3_out=pcs2_power;
    G3_Power_Result.P4_out=pcs3_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;

    }
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机运行，系统2处于主机故障，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs4_power;
    G3_Power_Result.P4_out=pcs3_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}

/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs3_power;
    G3_Power_Result.P3_out=pcs2_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}


/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run!=1)&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs3_power;
    G3_Power_Result.P3_out=pcs4_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}
    /*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run==1)&&(slave2_Run!=1)))
{
    G3_Power_Result.P1_out=pcs4_power;
    G3_Power_Result.P2_out=pcs1_power;
    G3_Power_Result.P3_out=pcs2_power;
    G3_Power_Result.P4_out=pcs3_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run!=1))&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs4_power;
    G3_Power_Result.P2_out=pcs1_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;

}
    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/

else if((master2_Run==1)&&(slave2_Run==1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
    G3_Power_Result.P1_out=pcs3_power;
    G3_Power_Result.P2_out=pcs4_power;
    G3_Power_Result.P3_out=pcs1_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
    }
/*情况14：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run!=1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
    {
    G3_Power_Result.P1_out=pcs3_power;
    G3_Power_Result.P2_out=pcs4_power;
    G3_Power_Result.P3_out=pcs1_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;


    }
    /*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run!=1)&&(master2_Run!=1)&&(slave2_Run!=1)&&(slave1_Run==1))
{
    G3_Power_Result.P1_out=pcs4_power;
    G3_Power_Result.P2_out=pcs1_power;
    G3_Power_Result.P3_out=pcs2_power;
    G3_Power_Result.P4_out=pcs3_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}

    /*情况15：系统2主机处于故障，且系统2从机运行，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if((master2_Run==1)&&(slave2_Run!=1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
    G3_Power_Result.P1_out=pcs4_power;
    G3_Power_Result.P2_out=pcs3_power;
    G3_Power_Result.P3_out=pcs2_power;
    G3_Power_Result.P4_out=pcs1_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}

/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}

    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs4_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;

}
    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run!=1))
{
G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
    G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
}
else//匹配1，情况
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=pcs_r_power1_2;
    G3_Power_Result.pcs_r_power3_4=pcs_r_power3_4;
    // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out ;
    G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (G3_Power_Result.P1_out+G3_Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (G3_Power_Result.P3_out+G3_Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    G3_Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out,G3_Power_Result.P2_out,G3_Power_Result.P3_out,G3_Power_Result.P4_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master2bat1_powerate);
    return G3_Power_Result;
    }

}
/**
 * @brief 计算并分配PCS(功率转换系统)的功率输出
 * 
 * 该函数根据系统状态和故障情况，计算并分配两个系统的功率输出，
 * 包括有功功率和无功功率的分配，以及各电池组(BAT)的功率分配比例。
 * 
 * @return Power_Divider_result_t 返回包含功率分配结果的结构体
 * 
 * @note 函数内部使用互斥锁(g_power_mtx)保护共享资源
 * @note 功率分配比例范围为0-1000，表示0%-100%
 * 
 * 处理的主要情况包括:
 * - 情况5: 系统1运行但有从机故障，系统2运行且从机无故障
 * - 情况6: 系统1运行且从机无故障，系统2运行但有从机故障
 * - 系统切换: 当系统2运行而系统1不运行时
 * - 默认情况: 其他所有运行状态
 */
Power_Divider_result_t G3_Power_Divider1()
{ 
    
    SYS_State_ENUM state[2]; 
    pthread_mutex_lock(&g_power_mtx);

    INT16U PCS1_sys_state = (INT16U)GET_INPUT(2600 + 300 * 0 + 100);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 0) & 0x1u) == 1u);//group1,主机PCS的运行
    
    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(2600 + 300 * 1 + 100);//group1,从机PCS的运行状态

    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 0) & 0x1u) == 1u);//group1,从机PCS的运行


    INT16U PCS2_sys_state = (INT16U)GET_INPUT(2600 + 300 * 2 + 100);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行


    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(2600 + 300 * 3 + 100);//group2,主机PCS的运行状态

    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 0) & 0x1u) == 1u);//group2,从机PCS的运行
/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
 if((master1_Run!=1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run==1))
{

    G3_Power_Result.P1_out=pcs7_power;
    G3_Power_Result.P2_out=pcs8_power;
    G3_Power_Result.P3_out=pcs1_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.P5_out=pcs3_power;
    G3_Power_Result.P6_out=pcs4_power;
    G3_Power_Result.P7_out=pcs5_power;
    G3_Power_Result.P8_out=pcs6_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}


    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;

}

/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs7_power;
    G3_Power_Result.P4_out=pcs8_power;

    G3_Power_Result.P5_out=pcs3_power;
    G3_Power_Result.P6_out=pcs4_power;
    G3_Power_Result.P7_out=pcs5_power;
    G3_Power_Result.P8_out=pcs6_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;

    }
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机运行，系统2处于主机故障，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.P5_out=pcs7_power;
    G3_Power_Result.P6_out=pcs8_power;
    G3_Power_Result.P7_out=pcs5_power;
    G3_Power_Result.P8_out=pcs6_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}

/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs5_power;
    G3_Power_Result.P4_out=pcs6_power;

    G3_Power_Result.P5_out=pcs3_power;
    G3_Power_Result.P6_out=pcs4_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}


/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run!=1)&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs5_power;
    G3_Power_Result.P4_out=pcs6_power;

    G3_Power_Result.P5_out=pcs7_power;
    G3_Power_Result.P6_out=pcs8_power;
    G3_Power_Result.P7_out=pcs3_power;
    G3_Power_Result.P8_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}
    /*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run==1)&&(slave2_Run!=1)))
{
    G3_Power_Result.P1_out=pcs5_power;
    G3_Power_Result.P2_out=pcs6_power;
    G3_Power_Result.P3_out=pcs1_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.P5_out=pcs3_power;
    G3_Power_Result.P6_out=pcs4_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run!=1))&&(slave2_Run==1))
{
    G3_Power_Result.P1_out=pcs5_power;
    G3_Power_Result.P2_out=pcs6_power;
    G3_Power_Result.P3_out=pcs1_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.P5_out=pcs7_power;
    G3_Power_Result.P6_out=pcs8_power;
    G3_Power_Result.P7_out=pcs3_power;
    G3_Power_Result.P8_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;

}
    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/

else if((master2_Run==1)&&(slave2_Run==1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
    G3_Power_Result.P1_out=pcs5_power;
    G3_Power_Result.P2_out=pcs6_power;
    G3_Power_Result.P3_out=pcs7_power;
    G3_Power_Result.P4_out=pcs8_power;

    G3_Power_Result.P5_out=pcs1_power;
    G3_Power_Result.P6_out=pcs2_power;
    G3_Power_Result.P7_out=pcs3_power;
    G3_Power_Result.P8_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;

        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}
   
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
    }
/*情况14：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run!=1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
    {
    G3_Power_Result.P1_out=pcs5_power;
    G3_Power_Result.P2_out=pcs6_power;
    G3_Power_Result.P3_out=pcs7_power;
    G3_Power_Result.P4_out=pcs8_power;

    G3_Power_Result.P5_out=pcs1_power;
    G3_Power_Result.P6_out=pcs2_power;
    G3_Power_Result.P7_out=pcs3_power;
    G3_Power_Result.P8_out=pcs4_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;

        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;



    }
    /*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run!=1)&&(master2_Run!=1)&&(slave2_Run!=1)&&(slave1_Run==1))
{
    G3_Power_Result.P1_out=pcs3_power;
    G3_Power_Result.P2_out=pcs4_power;
    G3_Power_Result.P3_out=pcs1_power;
    G3_Power_Result.P4_out=pcs2_power;

    G3_Power_Result.P5_out=pcs5_power;
    G3_Power_Result.P6_out=pcs6_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}

    /*情况15：系统2主机处于故障，且系统2从机运行，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if((master2_Run==1)&&(slave2_Run!=1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
    G3_Power_Result.P1_out=pcs5_power;
    G3_Power_Result.P2_out=pcs6_power;
    G3_Power_Result.P3_out=pcs7_power;
    G3_Power_Result.P4_out=pcs8_power;

    G3_Power_Result.P5_out=pcs3_power;
    G3_Power_Result.P6_out=pcs4_power;
    G3_Power_Result.P7_out=pcs1_power;
    G3_Power_Result.P8_out=pcs2_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;

        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}

/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.P5_out=pcs5_power;
    G3_Power_Result.P6_out=pcs6_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=pcs_r_power1_2;
    G3_Power_Result.pcs_r_power3_4=pcs_r_power3_4;
    // 主机1BAT1分配比例
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
}

    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.P5_out=pcs5_power;
    G3_Power_Result.P6_out=pcs6_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=pcs_r_power1_2;
    G3_Power_Result.pcs_r_power3_4=pcs_r_power3_4;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
        G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;

}
    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run!=1))
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.P5_out=pcs5_power;
    G3_Power_Result.P6_out=pcs6_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=0;
    G3_Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;

    G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);   
    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;

}
else//匹配1情况
{
    G3_Power_Result.P1_out=pcs1_power;
    G3_Power_Result.P2_out=pcs2_power;
    G3_Power_Result.P3_out=pcs3_power;
    G3_Power_Result.P4_out=pcs4_power;

    G3_Power_Result.P5_out=pcs5_power;
    G3_Power_Result.P6_out=pcs6_power;
    G3_Power_Result.P7_out=pcs7_power;
    G3_Power_Result.P8_out=pcs8_power;

    G3_Power_Result.pcs_r_power1_2=pcs_r_power1_2;
    G3_Power_Result.pcs_r_power3_4=pcs_r_power3_4;
        // 主机1BAT1分配比例
    INT32S total_out1 = G3_Power_Result.P1_out + G3_Power_Result.P2_out ;
    INT32S total_out2 = G3_Power_Result.P3_out + G3_Power_Result.P4_out;
    INT32S total_out3 = G3_Power_Result.P5_out + G3_Power_Result.P6_out ;
    INT32S total_out4 = G3_Power_Result.P7_out + G3_Power_Result.P8_out;
    

    G3_Power_Result.pcs_r_rate1_2 = (pcs_r_power1_2 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate1_2 = LIMIT(G3_Power_Result.pcs_r_rate1_2, -120, 120);
    G3_Power_Result.pcs_r_rate3_4 = (pcs_r_power3_4 * 100) / (3556 * 2);
    G3_Power_Result.pcs_r_rate3_4 = LIMIT(G3_Power_Result.pcs_r_rate3_4, -120, 120);

    if(total_out1!=0)
    {
    INT32S master1bat1_powerate = (G3_Power_Result.P1_out * 1000 ) / (total_out1);
    if (master1bat1_powerate < 0) master1bat1_powerate = 0;
    if (master1bat1_powerate > 1000) master1bat1_powerate = 1000;
    G3_Power_Result.master1bat1_powerate = (INT16S)master1bat1_powerate;
    // 主机1BAT2分配比例
     G3_Power_Result.master1bat2_powerate = 1000-master1bat1_powerate;
    }
    else
    {
        G3_Power_Result.master1bat1_powerate =500;
        G3_Power_Result.master1bat2_powerate =500;

    }
if(total_out2!=0)
{
    INT32S slave1bat1_powerate = (G3_Power_Result.P3_out * 1000 ) / (total_out2);
    if (slave1bat1_powerate < 0) slave1bat1_powerate = 0;
    if (slave1bat1_powerate > 1000) slave1bat1_powerate = 1000;
    G3_Power_Result.slave1bat1_powerate = (INT16S)slave1bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave1bat2_powerate = 1000-slave1bat1_powerate;
}
else
{
    G3_Power_Result.slave1bat1_powerate=500;
    G3_Power_Result.slave1bat2_powerate=500;

}
if(total_out3!=0)
{
    INT32S master2bat1_powerate = (G3_Power_Result.P5_out * 1000 ) / (total_out3);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    G3_Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.master2bat2_powerate = 1000-master2bat1_powerate;
}
else
{
    G3_Power_Result.master2bat1_powerate=500;
    G3_Power_Result.master2bat2_powerate=500;

}
if(total_out4!=0)
{
    INT32S slave2bat1_powerate = (G3_Power_Result.P7_out * 1000 ) / (total_out4);
    if (slave2bat1_powerate < 0) slave2bat1_powerate = 0;
    if (slave2bat1_powerate > 1000) slave2bat1_powerate = 1000;
    G3_Power_Result.slave2bat1_powerate = (INT16S)slave2bat1_powerate;
    // 主机1BAT2分配比例
    G3_Power_Result.slave2bat2_powerate = 1000-slave2bat1_powerate;
}
else
{
    G3_Power_Result.slave2bat1_powerate=500;
    G3_Power_Result.slave2bat2_powerate=500;

}

    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,G3_Power_Result.P1_out+G3_Power_Result.P2_out,G3_Power_Result.P3_out+G3_Power_Result.P4_out,G3_Power_Result.P5_out+G3_Power_Result.P6_out,G3_Power_Result.P7_out+G3_Power_Result.P8_out,G3_Power_Result.master1bat1_powerate,G3_Power_Result.master1bat2_powerate,G3_Power_Result.slave1bat1_powerate,G3_Power_Result.master2bat1_powerate,G3_Power_Result.master2bat2_powerate,G3_Power_Result.slave2bat1_powerate);
    return G3_Power_Result;
    }

}

/*地址映射到bit*/
static const MapEntry G_Map[] = {
    /* address, reg,   bit, inv, width */
    {27000,      40100-40001,  0,   0,   1},   // Reset
    {27001,      40100-40001,  1,   0,   1},   // Fault Clear
    {27004,      40100-40001,  4,   0,   1},   // Toggle Bit
    {27005,      40100-40001,  5,   0,   1},   // Rocof Cease

    {27016,      40101-40001,  0,   0,   1},   // PQ Mode
    {27017,      40101-40001,  1,   0,   1},   // PF Mode
    {27018,      40101-40001,  2,   0,   1},   // Volt Var Mode
    {27019,      40101-40001,  3,   0,   1},   // Volt Watt Mode
    {27020,      40101-40001,  4,   0,   1},   // Freq Watt Mode
    {27021,      40101-40001,  5,   0,   1},   // Black Start Mode

    {27040,      40102-40001,  8,   0,   1},   // EnableCharge
    {27041,      40102-40001,  9,   0,   1},   // EnableDischarge
    {27039,      40102-40001,  15,   0,   1},   // StatisticsClear

    {27042,      40104-40001,  0,   0,   1},   // MVG3MCBClose
    {27043,      40104-40001,  1,   0,   1},   // MVG3MCBOpen
    {27044,      40104-40001,  2,   0,   1},   // ResetRelayTrip
    {27045,      40104-40001,  3,   0,   1},   // SetRelayOCPTh
};
static const INT16U g_map_count = sizeof(G_Map)/sizeof(G_Map[0]);

static const MapEntry* Map_Find(INT16U address) {
    for (INT16U i = 0; i < g_map_count; ++i) {
        if (G_Map[i].address == address) return &G_Map[i];
    }
    return NULL;
}


/* -------------------- MV 总功率分配-------------------- */
/**
 * @brief MV功率分配函数
 * 根据系统配置和BMS(电池管理系统)状态，进行功率分配和更新
 */
/**
 * @brief 中压(MV)功率分配函数
 * 
 * 该函数根据系统运行状态、PCS(功率转换系统)状态和BMS(电池管理系统)状态，
 * 实现对8个BMS模块和4个PCS的功率分配。函数会检查不同的运行场景，并相应地
 * 设置每个模块的最大充放电功率、SOC、模块数量、最大功率等参数。
 * 
 * 功能说明：
 * 1. 获取系统配置信息和两个系统的运行状态
 * 2. 根据不同的运行场景(8种情况)分配功率：
 *    - 情况1: 系统1运行无故障，系统2不运行
 *    - 情况2: 系统1运行从机有故障，系统2不运行
 *    - 情况3: 系统2运行无故障，系统1不运行
 *    - 情况4: 系统2运行从机有故障，系统1不运行
 *    - 情况5: 系统1和2都运行，系统1从机故障，系统2无故障
 *    - 情况6: 系统1和2都运行，系统1和2都从机有故障或都无从机
 *    - 情况7: 系统1和2都运行，系统1无故障，系统2从机故障
 *    - 情况8: 系统1运行，系统2运行，且无故障
 * 3. 调用MCU_AI_step1()进行总功率分配
 * 4. 检查PCS功率分配是否变化，如有变化则更新并设置写标志
 * 
 * @param void 无参数
 * @return void 无返回值
 * 
 * @note 函数内部会访问和修改多个全局变量，包括BMS功率参数、SOC值、
 *       PCS额定功率、模块数量、功率分配标志等
 * @note 函数包含延时操作usleep(1000*10)
 */
void G3_MV_Power_allocate1(void)
{
    // 获取系统配置信息
    sysPara *sys_cfg = SysConf_GetInfo();
    INT16U pcs_num=sys_cfg->pcsNum;

    INT16U PCS1_sys_state = (INT16U)GET_INPUT(2600 + 300 * 0 + 100);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 0) & 0x1u) == 1u);//group1,主机PCS的运行
    
    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(2600 + 300 * 1 + 100);//group1,从机PCS的运行状态

    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 0) & 0x1u) == 1u);//group1,从机PCS的运行


    INT16U PCS2_sys_state = (INT16U)GET_INPUT(2600 + 300 * 2 + 100);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行


    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(2600 + 300 * 3 + 100);//group2,主机PCS的运行状态

    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 0) & 0x1u) == 1u);//group2,从机PCS的运行

//LOG_INFO("state[0]:%d,state[1]；%d,getinput:%d,(GET_INPUT(29271):%d,(GET_INPUT(29471):%d", state[0], state[1],GET_INPUT(17062),GET_INPUT(29271),GET_INPUT(29471));

if(BusType==D_BUS)
{
    /*情况1：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2无从机故障，发有功，发无功*/
if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run==1))
{
//  LOG_INFO("情况1");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);


    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);


    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms5_max_charge_power = GET_INPUT(28838); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms5_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms6_max_charge_power = GET_INPUT(29038); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms6_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms6_max_discharge_power' */
    } 
    else 
    {
        bms6_max_discharge_power = 0;
    }
    bms6_soc = GET_INPUT(29003);

    
    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms7_max_charge_power = GET_INPUT(29238); /* '<Root>/bms7_max_charge_power' */
    } 
    else 
    {
        bms7_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms7_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms7_max_discharge_power' */
    } 
    else 
    {
        bms7_max_discharge_power = 0;
    }
    bms7_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms8_max_charge_power = GET_INPUT(29438); /* '<Root>/bms8_max_charge_power' */
    } 
    else 
    {
        bms8_max_charge_power = 0;
    }

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms8_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms8_max_discharge_power' */
    } 
    else 
    {
        bms8_max_discharge_power = 0;
    }
    bms8_soc = GET_INPUT(29403);
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =8;              /* '<Root>/module_num' */
    mv_max_power            =10000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = GET_HOLD(1011);  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782         ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   2782         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            

}
/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run!=1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run==1))
{
    // LOG_INFO("情况2");
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);


    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29003);

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms5_max_charge_power = GET_INPUT(29238); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms5_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms6_max_charge_power = GET_INPUT(29438); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms6_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms6_max_discharge_power' */
    } 
    else 
    {
        bms6_max_discharge_power = 0;
    }
    bms6_soc = GET_INPUT(29403);
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =6;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0        ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run==1))
{
    // LOG_INFO("情况3");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29003);

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms5_max_charge_power = GET_INPUT(29238); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms5_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms6_max_charge_power = GET_INPUT(29438); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms6_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms6_max_discharge_power' */
    } 
    else 
    {
        bms6_max_discharge_power = 0;
    }
    bms6_soc = GET_INPUT(29403);

    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =6;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机运行，系统2处于故障，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run==1))
{
    // LOG_INFO("情况4");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);



    if((GET_INPUT(28471) == 0) || (GET_INPUT(28471) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28438); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28471) == 0) || (GET_INPUT(28471) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28437); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28403);

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(28638); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(28603);



    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms5_max_charge_power = GET_INPUT(29238); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms5_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms6_max_charge_power = GET_INPUT(29438); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms6_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms6_max_discharge_power' */
    } 
    else 
    {
        bms6_max_discharge_power = 0;
    }
    bms6_soc = GET_INPUT(29403);

    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =6;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run!=1))
{
    //  LOG_INFO("情况5");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms5_max_charge_power = GET_INPUT(28838); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms5_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms6_max_charge_power = GET_INPUT(29038); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms6_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms6_max_discharge_power' */
    } 
    else 
    {
        bms6_max_discharge_power = 0;
    }
    bms6_soc = GET_INPUT(29003);
    
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =6;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =  2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}

    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run!=1))
{
// LOG_INFO("情况6");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run!=1))
{
// LOG_INFO("情况7");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29003);
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}
/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run!=1)&&(slave2_Run==1))
{
// LOG_INFO("情况8");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(29238); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29438); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29403);
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}

/*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run==1)&&(slave2_Run!=1)))
{
//LOG_INFO("情况9");
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29003);
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run!=1))&&(slave2_Run==1))
{
// LOG_INFO("情况10");
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(29238); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29438); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29403);
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}

    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if((master2_Run==1)&&(slave2_Run==1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
// LOG_INFO("情况11");
    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms1_max_charge_power = GET_INPUT(28838); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms2_max_charge_power = GET_INPUT(29038); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms2_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms2_max_discharge_power' */
    } 
    else 
    {
        bms2_max_discharge_power = 0;
    }
    bms2_soc = GET_INPUT(29003);

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(29238); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms4_max_charge_power = GET_INPUT(29438); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29403);

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}


    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    // LOG_INFO("情况12");
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;           
}

/*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run!=1)&&(master2_Run!=1)&&(slave2_Run!=1)&&(slave1_Run==1))
{
//  LOG_INFO("情况13");
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);

    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;           
}

    /*情况14：系统2主机处于故障，且系统2从机运行，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run!=1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
//  LOG_INFO("情况14");
    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2)) 
    {
        bms1_max_charge_power = GET_INPUT(29238); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1)) 
    {
        bms1_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(29203);

    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2)) 
    {
        bms2_max_charge_power = GET_INPUT(29438); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1)) 
    {
        bms2_max_discharge_power = GET_INPUT(29437); /* '<Root>/bms2_max_discharge_power' */
    } 
    else 
    {
        bms2_max_discharge_power = 0;
    }
    bms2_soc = GET_INPUT(29403);
    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
    /*情况15：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if((master2_Run==1)&&(slave2_Run!=1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
    // LOG_INFO("情况15");
    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2)) 
    {
        bms1_max_charge_power = GET_INPUT(28838); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28803);

    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2)) 
    {
        bms2_max_charge_power = GET_INPUT(29038); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1)) 
    {
        bms2_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms2_max_discharge_power' */
    } 
    else 
    {
        bms2_max_discharge_power = 0;
    }
    bms2_soc = GET_INPUT(29003);
    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =1;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}



else
{

    LOG_INFO("不在分配范围内");

}

    MCU_AI_step();//MV总功率分配

    if((pcspowerallocate[0]!=pcs1_power) ||
    (pcspowerallocate[1]!=pcs2_power) ||
    (pcspowerallocate[2]!=pcs3_power) ||
    (pcspowerallocate[3]!=pcs4_power) ||
    (pcspowerallocate[4]!=pcs5_power) ||
    (pcspowerallocate[5]!=pcs6_power) ||
    (pcspowerallocate[6]!=pcs7_power) ||
    (pcspowerallocate[7]!=pcs8_power) ||

    (pcspowerallocate[8]!=pcs_r_power1_2) ||
    (pcspowerallocate[9]!=pcs_r_power3_4) ||
        MV_Power_Flag==true ||
        init_power_flag==1
    )
    {
        MV_Power_Flag=false;
        init_power_flag=0;
        pcspowerallocate[0]=pcs1_power;
        pcspowerallocate[1]=pcs2_power;
        pcspowerallocate[2]=pcs3_power;
        pcspowerallocate[3]=pcs4_power;
        pcspowerallocate[4]=pcs5_power;
        pcspowerallocate[5]=pcs6_power;
        pcspowerallocate[6]=pcs7_power;
        pcspowerallocate[7]=pcs8_power;
        pcspowerallocate[8]=pcs_r_power1_2;
        pcspowerallocate[9]=pcs_r_power3_4;


        PCS_Write_Flag[0]=true;
        PCS_Write_Flag[1]=true;
        PCS_Write_Flag[2]=true;
        PCS_Write_Flag[3]=true;
        PCSaddr[0]=1010;
        PCSaddr[1]=1010;
        PCSaddr[2]=1010;
        PCSaddr[3]=1010;
    

LOG_INFO("pcs1_power:%d,pcs2_power:%d,pcs3_power:%d,pcs4_power:%d,pcs5_power:%d,pcs6_power:%d,pcs7_power:%d,pcs8_power:%d,pcs_r_power1_2:%d,pcs_r_power3_4:%d",pcs1_power,pcs2_power,pcs3_power,pcs4_power,pcs5_power,pcs6_power,pcs7_power,pcs8_power,pcs_r_power1_2,pcs_r_power3_4);


    }


}
else if(BusType==S_BUS)
{
    /*情况1：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2无从机故障，发有功，发无功*/
if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run==1))
{
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);


    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);


        bms5_max_charge_power = 0;



        bms5_max_discharge_power = 0;

    bms5_soc = 0;


        bms6_max_charge_power = 0;


        bms6_max_discharge_power = 0;

    bms6_soc = 0;

    

        bms7_max_charge_power = 0;



        bms7_max_discharge_power = 0;

    bms7_soc = 0;


        bms8_max_charge_power = 0;



        bms8_max_discharge_power = 0;

    bms8_soc = 0;
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =10000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = GET_HOLD(1011);  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782         ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   2782         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            

}
/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run!=1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run==1))
{
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28403);


    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28638); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28603);


        bms4_max_charge_power = 0;


        bms4_max_discharge_power = 0;

    bms4_soc = 0;


        bms5_max_charge_power = 0;



        bms5_max_discharge_power = 0;

    bms5_soc = 0;


        bms6_max_charge_power = 0;



        bms6_max_discharge_power = 0;

    bms6_soc = 0;
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =3;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0        ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run==1))
{
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28403);

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28638); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28603);


        bms4_max_charge_power = 0;

        bms4_max_discharge_power = 0;

    bms4_soc = 0;


        bms5_max_charge_power = 0;


        bms5_max_discharge_power = 0;

    bms5_soc = 0;


        bms6_max_charge_power = 0;



        bms6_max_discharge_power = 0;

    bms6_soc = 0;

    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =3;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run==1))
{
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);



    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 2)) 
    {
        bms3_max_charge_power = GET_INPUT(28638); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28603);


        bms4_max_charge_power = 0;


        bms4_max_discharge_power = 0;

    bms4_soc = 0;




        bms5_max_charge_power = 0;


        bms5_max_discharge_power = 0;

    bms5_soc = 0;


        bms6_max_charge_power = 0;



        bms6_max_discharge_power = 0;

    bms6_soc = 0;

    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =3;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run==1)&&(slave2_Run!=1))
{
    
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);


        bms4_max_charge_power   = 0;


        bms4_max_discharge_power= 0;

    bms4_soc                = 0;


        bms5_max_charge_power = 0;



        bms5_max_discharge_power = 0;

    bms5_soc = 0;


        bms6_max_charge_power = 0;


        bms6_max_discharge_power = 0;

    bms6_soc = 0;
    
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =3;              /* '<Root>/module_num' */
    mv_max_power            =7500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =  2782       ;                             
    pcs3_rated_power        =   2782         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}

    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&(slave1_Run==1)&&(master2_Run!=1)&&(slave2_Run!=1))
{

    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);


        bms3_max_charge_power   = 0;


        bms3_max_discharge_power= 0;

    bms3_soc                = 0;


        bms4_max_charge_power   = 0;


        bms4_max_discharge_power= 0;

    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run==1)&&(slave2_Run!=1))
{

    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28403);


        bms3_max_charge_power = 0;



        bms3_max_discharge_power = 0;

    bms3_soc =0;


        bms4_max_charge_power = 0;

        bms4_max_discharge_power = 0;

    bms4_soc = 0;
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}
/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run==1)&&(slave1_Run!=1)&&(master2_Run!=1)&&(slave2_Run==1))
{

    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);


        bms3_max_charge_power = 0;


        bms3_max_discharge_power = 0;

    bms3_soc = 0;


        bms4_max_charge_power = 0;


        bms4_max_discharge_power = 0;

    bms4_soc = 0;
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}

/*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run==1)&&(slave2_Run!=1)))
{

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);

    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28403);


        bms3_max_charge_power = 0;


        bms3_max_discharge_power = 0;

    bms3_soc = 0;


        bms4_max_charge_power = 0;


        bms4_max_discharge_power = 0;

    bms4_soc = 0;
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        


}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((master1_Run!=1)&&((slave1_Run==1))&&((master2_Run!=1))&&(slave2_Run==1))
{

    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);

    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);


        bms3_max_charge_power = 0;



        bms3_max_discharge_power = 0;

    bms3_soc = 0;


        bms4_max_charge_power = 0;


        bms4_max_discharge_power = 0;

    bms4_soc = 0;
    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        

}

    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if((master2_Run==1)&&(slave2_Run==1)&&(master1_Run!=1)&&(slave1_Run!=1))
{

    if((GET_INPUT(28471) == 0) || (GET_INPUT(28471) == 2)) 
    {
        bms1_max_charge_power = GET_INPUT(28438); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if((GET_INPUT(28471) == 0) || (GET_INPUT(28471) == 1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28437); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28403);

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 2)) 
    {
        bms2_max_charge_power = GET_INPUT(28638); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 1)) 
    {
        bms2_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms2_max_discharge_power' */
    } 
    else 
    {
        bms2_max_discharge_power = 0;
    }
    bms2_soc = GET_INPUT(28603);


        bms3_max_charge_power = 0;



        bms3_max_discharge_power = 0;

    bms3_soc = 0;


        bms4_max_charge_power = 0;



        bms4_max_discharge_power = 0;

    bms4_soc = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =2;              /* '<Root>/module_num' */
    mv_max_power            =5000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   2782         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}


    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run==1)&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);


        bms2_max_charge_power   = 0;

        bms2_max_discharge_power= 0;

    bms2_soc                = 0;

    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =1;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;           
}

/*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if((master1_Run!=1)&&(master2_Run!=1)&&(slave2_Run!=1)&&(slave1_Run==1))
{
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);


        bms2_max_charge_power   = 0;

        bms2_max_discharge_power= 0;

    bms2_soc                = 0;

    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =1;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;           
}

    /*情况14：系统2主机处于故障，且系统2从机运行，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run!=1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    
    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 2)) 
    {
        bms1_max_charge_power = GET_INPUT(28638); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if((GET_INPUT(28671) == 0) || (GET_INPUT(28671) == 1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28603);


        bms2_max_charge_power = 0;

        bms2_max_discharge_power = 0;

    bms2_soc = 0;
    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =1;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}
    /*情况15：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if((master2_Run==1)&&(slave2_Run!=1)&&(master1_Run!=1)&&(slave1_Run!=1))
{
    
    if((GET_INPUT(28471) == 0) || (GET_INPUT(28471) == 2)) 
    {
        bms1_max_charge_power = GET_INPUT(28438); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if((GET_INPUT(28471) == 0) || (GET_INPUT(28471) == 1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28437); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28403);

        bms2_max_charge_power = 0;

        bms2_max_discharge_power = 0;

    bms2_soc = 0;
    bms3_max_charge_power   = 0;
    bms3_max_discharge_power= 0;
    bms3_soc                = 0;

    bms4_max_charge_power   = 0;
    bms4_max_discharge_power= 0;
    bms4_soc                = 0;

    bms5_max_charge_power   = 0; /* '<Root>/bms5_max_charge_power' */
    bms5_max_discharge_power= 0;/* '<Root>/bms5_max_discharge_power' */
    bms5_soc                = 0;  
            
    bms6_max_charge_power   = 0; /* '<Root>/bms6_max_charge_power' */
    bms6_max_discharge_power= 0;/* '<Root>/bms6_max_discharge_power' */
    bms6_soc                = 0;  
        
    bms7_max_charge_power   = 0; /* '<Root>/bms7_max_charge_power' */
    bms7_max_discharge_power= 0;/* '<Root>/bms7_max_discharge_power' */
    bms7_soc                = 0;  
            
    bms8_max_charge_power   = 0; /* '<Root>/bms8_max_charge_power' */
    bms8_max_discharge_power= 0;/* '<Root>/bms8_max_discharge_power' */
    bms8_soc                = 0;              /* '<Root>/bms8_soc' */
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =1;              /* '<Root>/module_num' */
    mv_max_power            =2500;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = 0;  
    pcs1_rated_power        =   2782         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
}


else
{

    LOG_INFO("不在分配范围内");

}

    MCU_AI_step();//MV总功率分配

    if((pcspowerallocate[0]!=pcs1_power) ||
    (pcspowerallocate[1]!=pcs2_power) ||
    (pcspowerallocate[2]!=pcs3_power) ||
    (pcspowerallocate[3]!=pcs4_power) ||


    (pcspowerallocate[8]!=pcs_r_power1_2) ||
    (pcspowerallocate[9]!=pcs_r_power3_4) ||
        MV_Power_Flag==true||
        init_power_flag==1
    )
    {
        MV_Power_Flag=false;
        init_power_flag=0;
        pcspowerallocate[0]=pcs1_power;
        pcspowerallocate[1]=pcs2_power;
        pcspowerallocate[2]=pcs3_power;
        pcspowerallocate[3]=pcs4_power;


        pcspowerallocate[8]=pcs_r_power1_2;
        pcspowerallocate[9]=pcs_r_power3_4;


        PCS_Write_Flag[0]=true;
        PCS_Write_Flag[1]=true;
        PCS_Write_Flag[2]=true;
        PCS_Write_Flag[3]=true;
        PCSaddr[0]=1010;
        PCSaddr[1]=1010;
        PCSaddr[2]=1010;
        PCSaddr[3]=1010;

LOG_INFO("pcs1_power:%d,pcs2_power:%d,pcs3_power:%d,pcs4_power:%d,pcs5_power:%d,pcs6_power:%d,pcs7_power:%d,pcs8_power:%d,pcs_r_power1_2:%d,pcs_r_power3_4:%d",pcs1_power,pcs2_power,pcs3_power,pcs4_power,pcs5_power,pcs6_power,pcs7_power,pcs8_power,pcs_r_power1_2,pcs_r_power3_4);


    }


}
}

/* -------------------- -------------------- */
/**
 * @brief 创建并启动PCS Taida写任务和心跳任务
 * 
 * 该函数根据输入的PCS编号创建两个线程：
 * 1. Pcs_Taida_Write_Task - PCS Taida写任务线程
 * 2. PCS_HB_Task - PCS心跳任务线程
 * 
 * @param _num 输入参数，指向包含PCS编号的字符指针
 *             有效值为0-9之间的整数
 *             注意：该参数会被转换为unsigned char再转为int使用
 * 
 * @note 
 * - 如果PCS编号无效(不在0-9范围内)，函数会记录日志并直接返回
 * - 创建的线程都设置为分离状态(PTHREAD_CREATE_DETACHED)
 * - 线程参数通过值传递方式传递

* 
* @warning 
* - 线程参数通过(void*)(intptr_t)pcs_num转换，确保在64位系统上也能正确传递
* 
* @return 无返回值
*/
void PCS_TRina_write_Task(const char *_num)
{
    const int pcs_num = (int)(unsigned char)(*_num);

    if (pcs_num < 0 || pcs_num >= 10) {
        LOG_INFO("PCS_write_task got invalid pcs_num=%d", pcs_num);
        return;
    }

    pthread_t thread_pcs;
    pthread_attr_t attr;
    struct sched_param param;
    int ret;

    LOG_INFO("PCS_write_task create pcs_num=%d", pcs_num);

    pthread_attr_init(&attr);

    ret = pthread_create(
        &thread_pcs,
        &attr,
        Pcs_Trina_Write_Task,
        (void *)(intptr_t)pcs_num);

    pthread_attr_destroy(&attr);

    if (ret != 0) {
        LOG_INFO("pthread_create Pcs_Trina_Write_Task failed: %s", strerror(ret));
        return;
    }


    memset(&param, 0, sizeof(param));
    param.sched_priority = 50;

    ret = pthread_setschedparam(thread_pcs, SCHED_FIFO, &param);
    if (ret != 0) {
        LOG_INFO("pthread_setschedparam failed: %s", strerror(ret));
    } else {
        LOG_INFO("Pcs_Trina_Write_Task pcs_num=%d set SCHED_FIFO priority=%d success",
                 pcs_num, param.sched_priority);
    }

    pthread_detach(thread_pcs);
}
void* PCS_Trina_HB_Task(void*arg)
{
sysPara *sys_cfg = SysConf_GetInfo();

    int pcs_num = (int)(intptr_t)arg;   

    // 防越界
    if (pcs_num < 0 || pcs_num >= 10) {
        LOG_INFO("PCS hb task got invalid pcs_num=%d", pcs_num);
        return NULL;
    }
    LOG_INFO("PCS hb task started for PCS-%d", pcs_num);

    INT8U loop = 0;
    int read_recv_res = 0;
    INT8U timeout_cnt=0;
    INT8U hb = 0;
    uint32_t hb_period_ms   = 3000;   // 心跳周期
    uint32_t read_period_ms = 200; // 读取周期
    uint32_t hour_cmd_period_ms = 3600 * 1000;   // 1小时下发一次对时命令
    uint64_t next_hour_cmd_ms = 0; // 
    uint64_t next_hb_ms   = 0;
    uint64_t next_read_ms = 0;
    uint16_t time_buf[9]; // 日期时间字符串
    SYS_TIME_BUF time;
    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons(sys_cfg->pcs_port[pcs_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->pcs_ip[pcs_num]);
        struct timeval time_out = {0, (400 * 1000)};

        // 创建本地客户端socket
        if ((socket_Trina_Heart_Pcs[pcs_num] = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("PCS-%d Create hb socket failure! ip:%s[port:%d]",
                    pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
            sleep(5);
            continue;
        }

        LOG_INFO("pcs-%d: hb Connect SUCCESS! ip:%s[port:%d]",
                pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
        timeout_cnt=0;  
    
        // uint64_t now = Now_Ms();
        // next_hb_ms   = now + hb_period_ms;
        // next_read_ms = now + read_period_ms;  
        // next_hour_cmd_ms = now + hour_cmd_period_ms;
while (1) {
           // now = Now_Ms();

            // // 1) 到点发送心跳写
            // if (now >= next_hb_ms) {
            //     next_hb_ms += hb_period_ms; 

            //     hb ^= 1u;
            //     INT16U newv = hb ? (INT16U)16 : (INT16U)0;  // bit4

    
            //     if (newv != Control_Word1_Buf[pcs_num]) {
            //         Modbus_TCP_Write_Heart(socket_Trina_Heart_Pcs[pcs_num],
            //                             0x01, 
            //                             0x06, 
            //                             PCS_Taida_OPRATE_addr,
            //                             newv, 
            //                             CMD_DELAY_200);
            //         Control_Word1_Buf[pcs_num] = newv;
            //     }
            // }

            // // 2) 到点发送读请求
            // if (now >= next_read_ms) {
            //     next_read_ms += read_period_ms;

            //     Modbus_TCP_Read(socket_Trina_Heart_Pcs[pcs_num],
            //                     PCS_Trina_SLAVE_ADDR,
            //                     MODBUS_READ_TYPE_04,
            //                     Each_PCS_Taida_addr2,
            //                     Each_PCS_Taida_size2,
            //                     CMD_DELAY_200);
            // }
            // if ((now >= next_hour_cmd_ms)||(Set_Time_Flag==true)) {
            //     Set_Time_Flag=false;
            //     next_hour_cmd_ms += hour_cmd_period_ms;
            //     Get_System_Time(&time);
            //     time_buf[0] = time.year;
            //     time_buf[1] = time.month;
            //     time_buf[2] = time.day;
            //     time_buf[3] = time.hour;
            //     time_buf[4] = time.minute;
            //     time_buf[5] = time.second;
            //     time_buf[6] = time.millisecond ;
            // for(int num=0;num<sys_cfg->pcsNum;num++)
            // {
            //     Modbus_TCP_Write_Multiple(socket_Trina_Heart_Pcs[num], 
            //     PCS_Trina_SLAVE_ADDR,
            //     Time_Set_Addr,
            //     7, 
            //     time_buf, 
            //     200);   
            // }

            // }
            Modbus_TCP_Read(socket_Trina_Heart_Pcs[pcs_num], PCS_Trina_SLAVE_ADDR, MODBUS_READ_TYPE_04, PCS_Trina_04size4, PCS_Trina_04size4, CMD_DELAY_100);
            usleep(20*1000);
            int16_t numbytes = Recv_Modbus_Back(socket_Trina_Heart_Pcs[pcs_num],
                                                pcs_num,
                                                PCS_BUFF_LEN,
                                                PCS_Trina_Write_DataProcess);
            if (numbytes < 0) {
                close(socket_Trina_Heart_Pcs[pcs_num]);
                socket_Trina_Pcs[pcs_num]=-1;
                break;
            } else if (numbytes == 0) {
                timeout_cnt++;
                if (timeout_cnt >= 75) {
                    close(socket_Trina_Heart_Pcs[pcs_num]);
                    socket_Trina_Pcs[pcs_num]=-1;
                    break;
                }
            } else {
                timeout_cnt = 0;
            }
        }
    }
    return NULL;
}



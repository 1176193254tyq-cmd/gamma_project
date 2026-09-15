#define _POSIX_C_SOURCE 200809L
#include <stdint.h>           // 
#include <pthread.h>
#include "time.h"
#include "main.h"
#include "pcs_taida_write.h"
#include "global_mcu_ai.h"
#include "MCU_AI.h"
//#include "MCU_AI1.h"
#include "pcs_write_queue.h" 

#include "pf_convert_final.h"
extern INT8U BusType;
INT32S socket_Taida_Pcs[10]={0};//pcs socket
INT32S socket_Heart_Pcs[10]={0};//pcs heart socket
PF_CONVERT_t g_convert[2] = {0};//PF 转换结构体,每个系统一份 S / PF 指令：0 -> PCS0 系统，1 -> PCS2 系统
/*EMS/Web下达指令时的全局变量）*/
extern INT8U  BMSnum;
extern BOOLEAN PCS_Write_Flag[10];
extern INT32S  PCSWrite_Lenth[10];
extern int  PCSaddr[10];
extern int  PCSvalue[10];
extern BOOLEAN MV_Power_Flag;
extern INT32S PCSactivepower[10];
extern int PCSPF[10];
int pcspowerallocate[20]; //记录PCS的功率分配的数值
int PF_Convertdata[2]; //记录PF转换后的数值
bool PFConvert_Write_Flag[4];//记录PF转换后的数值是否写入标志
int First_Flag[2]={1,1};//记录第一次写入的标志，一个MV有两个系统，所以有两个标志
char LogStr[100] = {0};
int Heart_PCS_Num;//用于记录心跳时候PCS的编号
MV_Q_Result reactive_power; 
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
extern uint16_T reactive_rate;         /* '<Root>/reactive_rate' */
/********************** */
volatile INT16U Control_Word1_Buf[4]={0};//台达PCS控制字1
volatile INT16U Control_Word3_Buf[4]={0};//台达PCS控制字3
/* 前置声明 */
static void Write_Flags_to_Queue(int num);
static void PCS_Taida_Write(int num);
static void PCS_Taida_Write_DataProcess(unsigned char *ptr, int num);

extern void MCU_AI_step(void);
extern void MCU_AI_step1(void);
void* Pcs_Taida_Write_Task(void *arg);
void  MV_Power_allocate1(void);
void* PCS_HB_Task(void*arg);
void PF_Convert(INT16U pcs_num);
Power_Divider_result_t Power_Divider();
Power_Divider_result_t Power_Result = {0};
Power_Divider_result_t Power_Divider1();
static pthread_mutex_t g_power_mtx = PTHREAD_MUTEX_INITIALIZER;//功率互斥锁,变量资源保护
static pthread_mutex_t g_mv_alloc_call_mtx = PTHREAD_MUTEX_INITIALIZER;//避免MV分配逻辑被双线程并发重入
static pthread_mutex_t g_mv_alloc_pf_mtx = PTHREAD_MUTEX_INITIALIZER;//避免MV分配逻辑被双线程并发重入

struct timespec t1;
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
static void PCS_Taida_Write(int num)
{
    /* 先把旧标志转为队列，保证兼容 */
    Write_Flags_to_Queue(num);
    INT8U pcs_num_mask; //PCS数量掩码，用于映射PCS实际设备地址
    PcsWriteReq req;
    INT16U send_power_cmd_buf[10] = {0};
    while (pcs_write_try_dequeue(num, &req)) {
        INT16U addr  = req.addr;
        INT16U value = req.value;

        /* 特殊：功率写入（27107 / 27407），先发送系数寄存器再写功率 */
        if (addr == 1010) {
            if(BusType==S_BUS)
            {
                Power_Divider();
                if (num == 0) {
                    if ((Power_Result.P1_out + Power_Result.P2_out) < 0) {
                        Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                            Master_Bat1_Charge_Power_Coef, (INT16U)Power_Result.master1bat1_powerate, CMD_DELAY_200);                                                    
                    } else {
                        Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                            Master_Bat1_Discharge_Power_Coef, (INT16U)Power_Result.master1bat1_powerate, CMD_DELAY_200);          
                    }
                    Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        Active_Power_Addr, Power_Result.P1_out + Power_Result.P2_out, CMD_DELAY_200);
                    Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        Reactive_Power_Addr, reactive_power.group1_q_cmd, CMD_DELAY_200);
                }
                else if (num == 2) {
                    if ((Power_Result.P3_out + Power_Result.P4_out) < 0) {
                        Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                            Master_Bat1_Charge_Power_Coef, (INT16U)Power_Result.master2bat1_powerate, CMD_DELAY_200);                     
                    } else {
                        Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                            Master_Bat1_Discharge_Power_Coef, (INT16U)Power_Result.master2bat1_powerate, CMD_DELAY_200);                   
                    }                
                    Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        Active_Power_Addr, (INT16U)(Power_Result.P3_out + Power_Result.P4_out), CMD_DELAY_200);
                    Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        Reactive_Power_Addr, reactive_power.group2_q_cmd, CMD_DELAY_200);
                }

            }
            else
            {
                Power_Divider1();
                if (num == 0) 
                {
                    if ((Power_Result.P1_out + Power_Result.P2_out+Power_Result.P3_out + Power_Result.P4_out) < 0) 
                    {
   
                        send_power_cmd_buf[0]=250;
                        send_power_cmd_buf[1]=250;
                        send_power_cmd_buf[2]=250;
                        send_power_cmd_buf[3]=(INT16U)Power_Result.master1bat1_powerate;
                        send_power_cmd_buf[4]=(INT16U)Power_Result.master1bat2_powerate;
                        send_power_cmd_buf[5]=(INT16U)Power_Result.slave1bat1_powerate;
                        send_power_cmd_buf[6]=(INT16U)(Power_Result.P1_out + Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
                        send_power_cmd_buf[7]=reactive_power.group1_q_cmd;
                        Modbus_TCP_Write_Multiple(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR, Master_Bat1_Discharge_Power_Coef, 8, send_power_cmd_buf, 200);
                        
                            
                    } 
                    else
                    {   
                        send_power_cmd_buf[0]=(INT16U)Power_Result.master1bat1_powerate;
                        send_power_cmd_buf[1]=(INT16U)Power_Result.master1bat2_powerate;
                        send_power_cmd_buf[2]=(INT16U)Power_Result.slave1bat1_powerate;
                        send_power_cmd_buf[3]=250;
                        send_power_cmd_buf[4]=250;
                        send_power_cmd_buf[5]=250;
                        send_power_cmd_buf[6]=(INT16U)(Power_Result.P1_out + Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
                        send_power_cmd_buf[7]=reactive_power.group1_q_cmd;
                        Modbus_TCP_Write_Multiple(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR, Master_Bat1_Discharge_Power_Coef, 8, send_power_cmd_buf, 200);

                    }
                }
                else if (num == 2) 
                {
                    if ((Power_Result.P5_out + Power_Result.P6_out+Power_Result.P7_out + Power_Result.P8_out) < 0) 
                    {
                        send_power_cmd_buf[0]=250;
                        send_power_cmd_buf[1]=250;
                        send_power_cmd_buf[2]=250;
                        send_power_cmd_buf[3]=(INT16U)Power_Result.master2bat1_powerate;
                        send_power_cmd_buf[4]=(INT16U)Power_Result.master2bat2_powerate;
                        send_power_cmd_buf[5]=(INT16U)Power_Result.slave2bat1_powerate;
                        send_power_cmd_buf[6]=(INT16U)(Power_Result.P5_out + Power_Result.P6_out+Power_Result.P7_out + Power_Result.P8_out);
                        send_power_cmd_buf[7]=reactive_power.group2_q_cmd;
                        Modbus_TCP_Write_Multiple(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR, Master_Bat1_Discharge_Power_Coef, 8, send_power_cmd_buf, 200);
                    
                        
                    } 
                    else 
                    {
                        // Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        //     Master_Bat1_Discharge_Power_Coef, (INT16U)Power_Result.master2bat1_powerate, CMD_DELAY_200);

                        // Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        //     Master_Bat2_Discharge_Power_Coef, (INT16U)Power_Result.master2bat2_powerate, CMD_DELAY_200);
                        // Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                        //     Slave_Bat1_Discharge_Power_Coef, (INT16U)Power_Result.slave2bat1_powerate, CMD_DELAY_200);  
                        send_power_cmd_buf[0]=(INT16U)Power_Result.master2bat1_powerate;
                        send_power_cmd_buf[1]=(INT16U)Power_Result.master2bat2_powerate;
                        send_power_cmd_buf[2]=(INT16U)Power_Result.slave2bat1_powerate;
                        send_power_cmd_buf[3]=250;
                        send_power_cmd_buf[4]=250;
                        send_power_cmd_buf[5]=250;
                        send_power_cmd_buf[6]=(INT16U)(Power_Result.P5_out + Power_Result.P6_out+Power_Result.P7_out + Power_Result.P8_out);
                        send_power_cmd_buf[7]=reactive_power.group2_q_cmd;
                        Modbus_TCP_Write_Multiple(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR, Master_Bat1_Discharge_Power_Coef, 8, send_power_cmd_buf, 200);    
                    
                    }               
                    // Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                    //     Active_Power_Addr, (INT16U)(Power_Result.P5_out + Power_Result.P6_out+Power_Result.P7_out + Power_Result.P8_out), CMD_DELAY_200);
                    // Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR,
                    //     Reactive_Power_Addr, Power_Result.pcs_r_power3_4, CMD_DELAY_200);
                }

            }          
            continue;
        }
        
        /* 特殊：控制字 27000~27045 或 27300~27345（位映射写整字） */
        if ( ((addr>=27000)&&(addr<=27045)) || ((addr>=27300)&&(addr<=27345)) ) 
        {
            Controlword_Bit(addr, (uint8_t)(value & 0x01), num);
            continue;
        }

    /* 常规：单寄存器 0x06 根据不同的PCS编号来分配掩码，之后进行地址映射*/
        switch (num) {
            case 0:
                pcs_num_mask = 0;
                break;
            case 1:
                pcs_num_mask = 4;
                break;
            case 2:
                pcs_num_mask = 1;
                break;
            case 3:
                pcs_num_mask = 5;
                break;
            default:
                break;
        }
        INT16U idx = Taida_Addr_From_Index(pcs_num_mask+1, addr);
        
        Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR, idx, value, 200);
      
   
        bool need_reset = false;
        switch (num) {
            case 0:
                need_reset = (addr == 12024 || addr == 12025);
                break;
            case 1:
                need_reset = (addr == 12324 || addr == 12325);
                break;
            case 2:
                need_reset = (addr == 12624 || addr == 12625);
                break;
            case 3:
                need_reset = (addr == 12924 || addr == 12925);
                break;
            default:
                break;
        }
        if (need_reset) {
            sleep(5);
            Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num], PCS_Taida_SLAVE_ADDR, 0x63, 1, CMD_DELAY_15);
            LOG_INFO("PCS[%d] 复位 ok", num);
        }


    }

if (Group1_power_multi_Flag==true)
        {
            Group1_power_multi_Flag=false;
                send_power_cmd_buf[0]=GET_HOLD(27050);//Master Bat1 Discharge Power Coef
                send_power_cmd_buf[1]=GET_HOLD(27046); //Master Bat2 Discharge Power Coef
                send_power_cmd_buf[2]=GET_HOLD(27047); //Slave Bat1 Discharge Power Coef 
                send_power_cmd_buf[3]=GET_HOLD(27051); //Master Bat1 Charge Power Coef 
                send_power_cmd_buf[4]=GET_HOLD(27048); //Master Bat2 Charge Power Coef
                send_power_cmd_buf[5]=GET_HOLD(27049); //Slave Bat1 Charge Power Coef
                send_power_cmd_buf[6]=GET_HOLD(27052); //Active Power Reference 
               
                Modbus_TCP_Write_Multiple(socket_Taida_Pcs[0], PCS_Taida_SLAVE_ADDR, Master_Bat1_Discharge_Power_Coef, 7, send_power_cmd_buf, 200);
            

        }
        if (Group2_power_multi_Flag==true)
        {
            Group2_power_multi_Flag=false;
                send_power_cmd_buf[0]=GET_HOLD(27350);
                send_power_cmd_buf[1]=GET_HOLD(27346);
                send_power_cmd_buf[2]=GET_HOLD(27347);
                send_power_cmd_buf[3]=GET_HOLD(27351);
                send_power_cmd_buf[4]=GET_HOLD(27348);
                send_power_cmd_buf[5]=GET_HOLD(27349);
                send_power_cmd_buf[6]=GET_HOLD(27352);
               
                Modbus_TCP_Write_Multiple(socket_Taida_Pcs[2], PCS_Taida_SLAVE_ADDR, Master_Bat1_Discharge_Power_Coef, 7, send_power_cmd_buf, 200);
            

        }
    /*处理PF转化下发的命令*/
if (num == 0 || num == 2) {
        INT16U idx = (num != 0) ? 1u : 0u;

        if (PFConvert_Write_Flag[idx]) {
            PFConvert_Write_Flag[idx] = false;

            Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num],
                                            PCS_Taida_SLAVE_ADDR,
                                            112,
                                            (INT16S)g_convert[idx].S,
                                            CMD_DELAY_15);

            Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num],
                                            PCS_Taida_SLAVE_ADDR,
                                            113,
                                            (INT16S)g_convert[idx].PF,
                                            CMD_DELAY_15);

            LOG_INFO("PCS[%d] PF convert write: S=%d, PF=%d",
                    num, g_convert[idx].S, g_convert[idx].PF);
        }
    }

}

int led_off(void)
{
    int fd = open("/sys/class/leds/led2/brightness", O_WRONLY);
    if (fd < 0)
    {
        perror("open failed");
        return -1;
    }

    const char *value = "0";

    if (write(fd, value, strlen(value)) < 0)
    {
        perror("write failed");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
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
static void PCS_Taida_Write_DataProcess(unsigned char *ptr, int num)
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
            Control_Word3_Buf[num]=RegVal.D16;

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
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
   
    double rtt_us =
    (t1.tv_sec - t0.tv_sec) * 1e6 +
    (t1.tv_nsec - t0.tv_nsec) / 1e3;
    LOG_INFO("start_ts: %ld.%09ld", t0.tv_sec, t0.tv_nsec);
    LOG_INFO("end_ts  : %ld.%09ld", t1.tv_sec, t1.tv_nsec);
    LOG_INFO("RTT = %.3f us", rtt_us);
    Hex_To_String(LogStr, sizeof(LogStr), (const unsigned char *)pbuf, 30);
    LOG_INFO("LC Receive PCS 0x06, address:%d, buffer => %s", temp.D16, LogStr);

    }
    else if (pbuf[7] == 0x10)
    {
    Hex_To_String(LogStr, sizeof(LogStr), (const unsigned char *)pbuf, 40);
    LOG_INFO("LC Receive PCS 0x10, address:%d, buffer => %s", temp.D16, LogStr);
    }
    else if (pbuf[7] == 0x04)
    {

    }
    else
    {
    //   DebugModbusBuf("PCS Invalid data!\n", pbuf, 30);
    }
}
static void PCS_Taida_Write_Heart_DataProcess(unsigned char *ptr, int num)
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
            Control_Word3_Buf[num]=RegVal.D16;

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
void* Pcs_Taida_Write_Task(void *arg)
{
    sysPara *sys_cfg = SysConf_GetInfo();

    int pcs_num = (int)(intptr_t)arg;   

    // 防越界
    if (pcs_num < 0 || pcs_num >= 10) {
        LOG_INFO("PCS write task got invalid pcs_num=%d", pcs_num);
        return NULL;
    }
    LOG_INFO("PCS write task started for PCS-%d", pcs_num);

    INT8U loop = 0;
    int read_recv_res = 0;
    INT8U timeout_cnt=0;
    while (1) {
        // 配置socket资料，默认使用IPV4方式
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port   = htons(sys_cfg->pcs_port[pcs_num]);
        server_addr.sin_addr.s_addr = inet_addr((char *)sys_cfg->pcs_ip[pcs_num]);
        struct timeval time_out = {0, (2 * 1000)};

        // 创建本地客户端socket
        if ((socket_Taida_Pcs[pcs_num] = Create_Client_Socket(server_addr, time_out)) == -1) {
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

        while (1) {
        if(GET_INPUT(P2P_mode)==0)
        {               
            switch (loop) {

                case 0:
                if(pcs_num==0||pcs_num==2)
                {
                    INT8U sysnum = (pcs_num!= 0) ? 1 : 0;
                    SYS_State_ENUM state = Get_State_Sys(sysnum); //获取系统的状态

                        static uint8_t prev_is_run[2] = {0, 0};   // 假设 sysnum 只有 0/1 两套
                        uint8_t now_is_run = is_run_state(state);

                        // 上升沿：之前不是运行态，现在是运行态
                        if (!prev_is_run[sysnum] && now_is_run) {
                            LOG_INFO("执行功率分配策略！");
                            sleep(2); 
                            PCSactivepower[sysnum]=0;
                             init_power_flag = 1;
                        }

                        prev_is_run[sysnum] = now_is_run;

                        
                if(((GET_HOLD(27016)==1)||(GET_HOLD(27316)==1))&&((state==SYSRun)||(state==SYSWarnRun)))
                    {
                        //有两个线程可能都会进入到这个函数中，因此需要加锁，避免高并发导致的数据混乱，以及崩溃问题
                        if (pthread_mutex_trylock(&g_mv_alloc_call_mtx) == 0) {
                            MV_Power_allocate1();
                            pthread_mutex_unlock(&g_mv_alloc_call_mtx);
                        }
                    }
                        
                }    
                    loop++;
                    break;
                case 1:
                if(pcs_num==0||pcs_num==2)
                {
                    INT8U sysnum = (pcs_num!= 0) ? 1 : 0;
                    SYS_State_ENUM state = Get_State_Sys(sysnum); //获取系统的状态
                    if(((GET_HOLD(27017)==1)||(GET_HOLD(27317)==1))&&((state==SYSRun)||(state==SYSWarnRun)))
                    {
        
                        if (pthread_mutex_trylock(&g_mv_alloc_pf_mtx) == 0) {
                           PF_Convert(sysnum);
                            //LOG_INFO("执行PF！");
                            pthread_mutex_unlock(&g_mv_alloc_pf_mtx);
                        }
                    }
                }
                    loop++;
                    break;

                case 2:
                // Modbus_TCP_Read(socket_Taida_Pcs[pcs_num], 
                //                 PCS_Taida_SLAVE_ADDR, 
                //                 MODBUS_READ_TYPE_04, 
                //                 Each_PCS_Taida_addr1, 
                //                 Each_PCS_Taida_size1, 
                //                 CMD_DELAY_50);
               // LOG_INFO("主从pcs_num：%d",pcs_num);
                    loop++;
                    break;

                default:
                    loop = 0;
                    break;
            }
            PCS_Taida_Write(pcs_num);
            usleep(5*1000);
        int16_t numbytes=Recv_Modbus_Back(socket_Taida_Pcs[pcs_num], pcs_num, PCS_BUFF_LEN, PCS_Taida_Write_DataProcess);
       // LOG_INFO("socket_Heart_Pcs[%d]:%d",pcs_num,socket_Heart_Pcs[pcs_num]);
        if (socket_Heart_Pcs[pcs_num]==-1)  
                {
                    LOG_INFO("PCS-%d:其他套接字故障 ", pcs_num);
                    close(socket_Taida_Pcs[pcs_num]);
                    LOG_INFO("PCS-%d: 连接异常，断开服务端连接 ", pcs_num);
                    break;
                }
                // else if (numbytes == 0) {
                //     timeout_cnt++;
                // if (timeout_cnt >= 75) {   //超时时间约为400ms
                //     LOG_INFO("PCS-%d: 连续超时次数过多(%d)，认为通讯异常，准备重连", 
                //     pcs_num, timeout_cnt);
                //     close(socket_Taida_Pcs[pcs_num]);               
                //     break;
                // }
                // }
            else 
                {
                    timeout_cnt=0;
                }
        }
        else
        {
            sleep(5);
        }
        }
    
    }  
    close(socket_Taida_Pcs[pcs_num]);
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
Power_Divider_result_t Power_Divider()
{ 
    
    SYS_State_ENUM state[2]; 
    pthread_mutex_lock(&g_power_mtx);

    INT16U PCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 60);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT8U  master1_fault  = (INT8U)(((PCS1_sys_state >> 4) & 0x1u) == 1u);//group1,主机PCS的运行故障

    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 62);//group1,从机PCS的运行状态
    INT8U  slave1_Standby  = (INT8U)(((slavePCS1_sys_state >> 0) & 0x1u) == 1u);//group1,从机PCS的运行
    INT8U  slave1_fault = (INT8U)(((slavePCS1_sys_state >> 1) & 0x1u) == 1u);//group1,从机PCS的运行故障 
    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 2) & 0x1u) == 1u);//group1,从机PCS的运行


    INT16U PCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 60);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT8U  master2_fault  = (INT8U)(((PCS2_sys_state >> 4) & 0x1u) == 1u);

    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 62);//group2,主机PCS的运行状态
    INT8U  slave2_Standby  = (INT8U)(((slavePCS2_sys_state >> 0) & 0x1u) == 1u);//group2,从机PCS的运行
    INT8U  slave2_fault = (INT8U)(((slavePCS2_sys_state >> 1) & 0x1u) == 1u);//group2,从机PCS的运行故障 
    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 2) & 0x1u) == 1u);//group2,从机PCS的运行
/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
if(((master1_Run==1)&&(master1_fault==1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{

    Power_Result.P1_out=pcs4_power;
    Power_Result.P2_out=pcs1_power;
    Power_Result.P3_out=pcs2_power;
    Power_Result.P4_out=pcs3_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;

}

/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run!=1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs4_power;
    Power_Result.P3_out=pcs2_power;
    Power_Result.P4_out=pcs3_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;

    }
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机运行，系统2处于主机故障，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs4_power;
    Power_Result.P4_out=pcs3_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}

/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&((master2_fault!=1)))&&(slave2_Run!=1)))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs3_power;
    Power_Result.P3_out=pcs2_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}


/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs3_power;
    Power_Result.P3_out=pcs4_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}
    /*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault!=1)==1)&&(slave2_Run!=1)))
{
    Power_Result.P1_out=pcs4_power;
    Power_Result.P2_out=pcs1_power;
    Power_Result.P3_out=pcs2_power;
    Power_Result.P4_out=pcs3_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{
    Power_Result.P1_out=pcs4_power;
    Power_Result.P2_out=pcs1_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;

}
    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/

else if(((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    Power_Result.P1_out=pcs3_power;
    Power_Result.P2_out=pcs4_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
    }
/*情况14：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
    {
    Power_Result.P1_out=pcs3_power;
    Power_Result.P2_out=pcs4_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;


    }
    /*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault==1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run==1))
{
    Power_Result.P1_out=pcs4_power;
    Power_Result.P2_out=pcs1_power;
    Power_Result.P3_out=pcs2_power;
    Power_Result.P4_out=pcs3_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}

    /*情况15：系统2主机处于运行，且系统2从机故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1))&&(slave2_Run!=1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    Power_Result.P1_out=pcs4_power;
    Power_Result.P2_out=pcs3_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}

/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run!=1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
}

    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run)&&(master1_fault!=1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs4_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;

}
    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run)&&((master2_Run!=1)&&(slave2_Run!=1)))
{
   Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;  
}
else//匹配1
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.pcs_r_power1_2=pcs_r_power1_2;
    Power_Result.pcs_r_power3_4=pcs_r_power3_4;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out ;
    INT32S total_out2 = Power_Result.P3_out + Power_Result.P4_out ;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;

    }
    else
    {
        Power_Result.master1bat1_powerate =500;

    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P3_out+Power_Result.P4_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;

}
else
{
    Power_Result.master2bat1_powerate=500;

}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d, 主机2BAT1分配比例:%d"
        ,Power_Result.P1_out,Power_Result.P2_out,Power_Result.P3_out,Power_Result.P4_out,Power_Result.master1bat1_powerate,Power_Result.master2bat1_powerate);
    return Power_Result;
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
Power_Divider_result_t Power_Divider1()
{ 
    
    SYS_State_ENUM state[2]; 
    pthread_mutex_lock(&g_power_mtx);

    INT16U PCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 60);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT8U  master1_fault  = (INT8U)(((PCS1_sys_state >> 4) & 0x1u) == 1u);//group1,主机PCS的运行故障

    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 62);//group1,从机PCS的运行状态
    INT8U  slave1_Standby  = (INT8U)(((slavePCS1_sys_state >> 0) & 0x1u) == 1u);//group1,从机PCS的运行
    INT8U  slave1_fault = (INT8U)(((slavePCS1_sys_state >> 1) & 0x1u) == 1u);//group1,从机PCS的运行故障 
    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 2) & 0x1u) == 1u);//group1,从机PCS的运行


    INT16U PCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 60);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT8U  master2_fault  = (INT8U)(((PCS2_sys_state >> 4) & 0x1u) == 1u);

    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 62);//group2,主机PCS的运行状态
    INT8U  slave2_Standby  = (INT8U)(((slavePCS2_sys_state >> 0) & 0x1u) == 1u);//group2,从机PCS的运行
    INT8U  slave2_fault = (INT8U)(((slavePCS2_sys_state >> 1) & 0x1u) == 1u);//group2,从机PCS的运行故障 
    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 2) & 0x1u) == 1u);//group2,从机PCS的运行

/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
if(((master1_Run==1)&&(master1_fault==1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{

    Power_Result.P1_out=pcs7_power;
    Power_Result.P2_out=pcs8_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.P5_out=pcs3_power;
    Power_Result.P6_out=pcs4_power;
    Power_Result.P7_out=pcs5_power;
    Power_Result.P8_out=pcs6_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;

}

/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run!=1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs7_power;
    Power_Result.P4_out=pcs8_power;

    Power_Result.P5_out=pcs3_power;
    Power_Result.P6_out=pcs4_power;
    Power_Result.P7_out=pcs5_power;
    Power_Result.P8_out=pcs6_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
//  INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    INT32S masterbat2_powerate = 1000 - masterbat1_powerate;
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;

    }
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机运行，系统2处于主机故障，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.P5_out=pcs7_power;
    Power_Result.P6_out=pcs8_power;
    Power_Result.P7_out=pcs5_power;
    Power_Result.P8_out=pcs6_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);

    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}

/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&((master2_fault!=1)))&&(slave2_Run!=1)))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs5_power;
    Power_Result.P4_out=pcs6_power;

    Power_Result.P5_out=pcs3_power;
    Power_Result.P6_out=pcs4_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
// INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    INT32S masterbat2_powerate = 1000 - masterbat1_powerate;
if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    //INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    INT32S master2bat2_powerate = 1000 - master2bat1_powerate;
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}


/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs5_power;
    Power_Result.P4_out=pcs6_power;

    Power_Result.P5_out=pcs7_power;
    Power_Result.P6_out=pcs8_power;
    Power_Result.P7_out=pcs3_power;
    Power_Result.P8_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    //INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    INT32S masterbat2_powerate = 1000 - masterbat1_powerate;
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}
    /*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault!=1)==1)&&(slave2_Run!=1)))
{
    Power_Result.P1_out=pcs5_power;
    Power_Result.P2_out=pcs6_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.P5_out=pcs3_power;
    Power_Result.P6_out=pcs4_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
//  INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    INT32S master2bat2_powerate = 1000 - master2bat1_powerate;
if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{
    Power_Result.P1_out=pcs5_power;
    Power_Result.P2_out=pcs6_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.P5_out=pcs7_power;
    Power_Result.P6_out=pcs8_power;
    Power_Result.P7_out=pcs3_power;
    Power_Result.P8_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
        // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;

}
    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/

else if(((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    Power_Result.P1_out=pcs5_power;
    Power_Result.P2_out=pcs6_power;
    Power_Result.P3_out=pcs7_power;
    Power_Result.P4_out=pcs8_power;

    Power_Result.P5_out=pcs1_power;
    Power_Result.P6_out=pcs2_power;
    Power_Result.P7_out=pcs3_power;
    Power_Result.P8_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;

    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    // 主机1BAT1分配比例
    Power_Result.master1bat1_powerate = 250;//(恢复成PCS默认数值)
    // 主机1BAT2分配比例
    Power_Result.master1bat2_powerate = 250;
    // 从机1BAT1分配比例
    Power_Result.slave1bat1_powerate = 250;

    if(total_out2!=0)
    {
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
    }
    else
    {
        Power_Result.master2bat1_powerate = 250;
        Power_Result.master2bat2_powerate = 250;
        Power_Result.slave2bat1_powerate = 250;
    }   
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
    }
/*情况14：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
    {
    Power_Result.P1_out=pcs5_power;
    Power_Result.P2_out=pcs6_power;
    Power_Result.P3_out=pcs7_power;
    Power_Result.P4_out=pcs8_power;

    Power_Result.P5_out=pcs1_power;
    Power_Result.P6_out=pcs2_power;
    Power_Result.P7_out=pcs3_power;
    Power_Result.P8_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;

    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    // 主机1BAT1分配比例
    Power_Result.master1bat1_powerate = 250;//(恢复成PCS默认数值)
    // 主机1BAT2分配比例
    Power_Result.master1bat2_powerate = 250;
    // 从机1BAT1分配比例
    Power_Result.slave1bat1_powerate = 250;

    if(total_out2!=0)
    {
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
// INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    INT32S master2bat2_powerate = 1000 - master2bat1_powerate; 
if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
    }
    else
    {
        Power_Result.master2bat1_powerate = 250;
        Power_Result.master2bat2_powerate = 250;
        Power_Result.slave2bat1_powerate = 250;
    }   
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;



    }
    /*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault==1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run==1))
{
    Power_Result.P1_out=pcs3_power;
    Power_Result.P2_out=pcs4_power;
    Power_Result.P3_out=pcs1_power;
    Power_Result.P4_out=pcs2_power;

    Power_Result.P5_out=pcs5_power;
    Power_Result.P6_out=pcs6_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}

    /*情况15：系统2主机运行，且系统2从机故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1))&&(slave2_Run!=1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    Power_Result.P1_out=pcs5_power;
    Power_Result.P2_out=pcs6_power;
    Power_Result.P3_out=pcs7_power;
    Power_Result.P4_out=pcs8_power;

    Power_Result.P5_out=pcs1_power;
    Power_Result.P6_out=pcs2_power;
    Power_Result.P7_out=pcs3_power;
    Power_Result.P8_out=pcs4_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;

    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    // 主机1BAT1分配比例
    Power_Result.master1bat1_powerate = 250;//(恢复成PCS默认数值)
    // 主机1BAT2分配比例
    Power_Result.master1bat2_powerate = 250;
    // 从机1BAT1分配比例
    Power_Result.slave1bat1_powerate = 250;

    if(total_out2!=0)
    {
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
    }
    else
    {
        Power_Result.master2bat1_powerate = 250;
        Power_Result.master2bat2_powerate = 250;
        Power_Result.slave2bat1_powerate = 250;
    }   
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}

/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run!=1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.P5_out=pcs5_power;
    Power_Result.P6_out=pcs6_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    //INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    INT32S master2bat2_powerate = 1000 - master2bat1_powerate; 
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}

    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run)&&(master1_fault!=1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.P5_out=pcs5_power;
    Power_Result.P6_out=pcs6_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
// INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    INT32S masterbat2_powerate = 1000 - masterbat1_powerate;  
if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;


}
    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run)&&((master2_Run!=1)&&(slave2_Run!=1)))
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.P5_out=pcs5_power;
    Power_Result.P6_out=pcs6_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=0;
    Power_Result.pcs_r_power3_4=0;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
}
else//匹配1
{
    Power_Result.P1_out=pcs1_power;
    Power_Result.P2_out=pcs2_power;
    Power_Result.P3_out=pcs3_power;
    Power_Result.P4_out=pcs4_power;

    Power_Result.P5_out=pcs5_power;
    Power_Result.P6_out=pcs6_power;
    Power_Result.P7_out=pcs7_power;
    Power_Result.P8_out=pcs8_power;

    Power_Result.pcs_r_power1_2=pcs_r_power1_2;
    Power_Result.pcs_r_power3_4=pcs_r_power3_4;
    // 主机1BAT1分配比例
    INT32S total_out1 = Power_Result.P1_out + Power_Result.P2_out + Power_Result.P3_out + Power_Result.P4_out;
    INT32S total_out2 = Power_Result.P5_out + Power_Result.P6_out + Power_Result.P7_out + Power_Result.P8_out;
    if(total_out1!=0)
    {
    INT32S masterbat1_powerate = (Power_Result.P1_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat1_powerate < 0) masterbat1_powerate = 0;
    if (masterbat1_powerate > 1000) masterbat1_powerate = 1000;
    Power_Result.master1bat1_powerate = (INT16S)masterbat1_powerate;
    // 主机1BAT2分配比例
    INT32S masterbat2_powerate = (Power_Result.P2_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (masterbat2_powerate < 0) masterbat2_powerate = 0;
    if (masterbat2_powerate > 1000) masterbat2_powerate = 1000;
    Power_Result.master1bat2_powerate = (INT16S)masterbat2_powerate;

    // 从机1BAT1分配比例
    INT32S slaver1bat1_powerate = (Power_Result.P3_out * 1000 ) / (Power_Result.P1_out+Power_Result.P2_out+Power_Result.P3_out+Power_Result.P4_out);
    if (slaver1bat1_powerate < 0) slaver1bat1_powerate = 0;
    if (slaver1bat1_powerate > 1000) slaver1bat1_powerate = 1000;
    Power_Result.slave1bat1_powerate = (INT16S)slaver1bat1_powerate;
    }
    else
    {
        Power_Result.master1bat1_powerate =250;
        Power_Result.master1bat2_powerate =250;
        Power_Result.slave1bat1_powerate =250;
    }
if(total_out2!=0)
{
    // 主机2BAT1分配比例
    INT32S master2bat1_powerate = (Power_Result.P5_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat1_powerate < 0) master2bat1_powerate = 0;
    if (master2bat1_powerate > 1000) master2bat1_powerate = 1000;
    Power_Result.master2bat1_powerate = (INT16S)master2bat1_powerate;
    // 主机2BAT2分配比例
    INT32S master2bat2_powerate = (Power_Result.P6_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (master2bat2_powerate < 0) master2bat2_powerate = 0;
    if (master2bat2_powerate > 1000) master2bat2_powerate = 1000;
    Power_Result.master2bat2_powerate = (INT16S)master2bat2_powerate;

    // 从机2BAT1分配比例
    INT32S slaver2bat1_powerate = (Power_Result.P7_out * 1000 ) / (Power_Result.P5_out+Power_Result.P6_out+Power_Result.P7_out+Power_Result.P8_out);
    if (slaver2bat1_powerate < 0) slaver2bat1_powerate = 0;
    if (slaver2bat1_powerate > 1000) slaver2bat1_powerate = 1000;
    Power_Result.slave2bat1_powerate = (INT16S)slaver2bat1_powerate;
}
else
{
    Power_Result.master2bat1_powerate=250;
    Power_Result.master2bat2_powerate=250;
    Power_Result.slave2bat1_powerate= 250;
}
    pthread_mutex_unlock(&g_power_mtx);
    LOG_INFO("主pcs1总：%d,从pcs1总：%d,主pcs2总：%d,从pcs2总：%d,主机1BAT1分配比例:%d,主机1BAT2分配比例:%d, 从机1BAT1分配比例:%d, 主机2BAT1分配比例:%d,主机2BAT2分配比例:%d,从机2BAT1分配比例:%d"
        ,Power_Result.P1_out+Power_Result.P2_out,Power_Result.P3_out+Power_Result.P4_out,Power_Result.P5_out+Power_Result.P6_out,Power_Result.P7_out+Power_Result.P8_out,Power_Result.master1bat1_powerate,Power_Result.master1bat2_powerate,Power_Result.slave1bat1_powerate,Power_Result.master2bat1_powerate,Power_Result.master2bat2_powerate,Power_Result.slave2bat1_powerate);
    return Power_Result;
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

/* 写入：把“点位=0/1”改写到映射的寄存器位上（读改写整字） */
int Controlword_Bit(uint16_t address, uint8_t val01,int num)
{
    INT16U word=0;

    INT16U baseaddr;
    if((address>=27300)&&(address<=27345))//第二个仓
    {
        baseaddr=address-300;
    }
    else
    {
        baseaddr=address;
    }
    const MapEntry* position = Map_Find(baseaddr);
    if (!position) return -1;                       /* 未映射 */
    if (position->width != 1) return 0;

    INT16U mask  = (INT16U)(1 << position->bit);
    INT8U  value = (val01 ? 1 : 0);
    // if (position->invert) value ^= 1;              /* 反相逻辑 */
    if(position->address==27040||position->address==27041)
    {    
        if (value) word =  Control_Word3_Buf[0]|mask;                      /* 置位 */
        else       word = Control_Word3_Buf[0]&(INT16U)~mask;             /* 清位 */
        LOG_INFO("Control_Word3_Buf[%d] :%d,word:%d",0,Control_Word3_Buf[0],word);                
    }
    else if (position->address==27340||position->address==27341)
    {
        if (value) word =  Control_Word3_Buf[1]|mask;                      /* 置位 */
        else       word = Control_Word3_Buf[1]&(INT16U)~mask;             /* 清位 */
        LOG_INFO("Control_Word3_Buf[%d] :%d,word:%d",1,Control_Word3_Buf[1],word);      
    }
    else
    {
        if (value) word |=  mask;                      /* 置位 */
        else      word &= (INT16U)~mask;             /* 清位 */
    }

    LOG_INFO("要发送的地址是%d,要发送的数值是%d",position->reg_addr,word);
    Modbus_TCP_Write06_SingleRegist(socket_Taida_Pcs[num],
                                    PCS_Taida_SLAVE_ADDR,
                                    position->reg_addr,
                                    word, 
                                    CMD_DELAY_15);

    return 0;
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
void MV_Power_allocate1(void)
{
    // 获取系统配置信息
    sysPara *sys_cfg = SysConf_GetInfo();
    INT16U pcs_num=sys_cfg->pcsNum;

    INT16U PCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 60);//group1,主机PCS的运行状态
    INT8U  master1_Run  = (INT8U)(((PCS1_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT8U  master1_fault  = (INT8U)(((PCS1_sys_state >> 4) & 0x1u) == 1u);//group1,主机PCS的运行故障

    INT16U slavePCS1_sys_state = (INT16U)GET_INPUT(17000 + 300 * 0 + 62);//group1,从机PCS的运行状态
    INT8U  slave1_Standby  = (INT8U)(((slavePCS1_sys_state >> 0) & 0x1u) == 1u);//group1,从机PCS的运行
    INT8U  slave1_fault = (INT8U)(((slavePCS1_sys_state >> 1) & 0x1u) == 1u);//group1,从机PCS的运行故障 
    INT8U  slave1_Run  = (INT8U)(((slavePCS1_sys_state >> 2) & 0x1u) == 1u);//group1,从机PCS的运行


    INT16U PCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 60);//group2,主机PCS的运行状态
    INT8U  master2_Run  = (INT8U)(((PCS2_sys_state >> 3) & 0x1u) == 1u);//group1,主机PCS的运行
    INT8U  master2_fault  = (INT8U)(((PCS2_sys_state >> 4) & 0x1u) == 1u);

    INT16U slavePCS2_sys_state = (INT16U)GET_INPUT(17000 + 300 * 1 + 62);//group2,主机PCS的运行状态
    INT8U  slave2_Standby  = (INT8U)(((slavePCS2_sys_state >> 0) & 0x1u) == 1u);//group2,从机PCS的运行
    INT8U  slave2_fault = (INT8U)(((slavePCS2_sys_state >> 1) & 0x1u) == 1u);//group2,从机PCS的运行故障 
    INT8U  slave2_Run  = (INT8U)(((slavePCS2_sys_state >> 2) & 0x1u) == 1u);//group2,从机PCS的运行
  
    // PCS1
    INT8U PCS1BUS1 =(INT8U)((((GET_INPUT(17000 + 69) >> 0) & 0x1u) == 0u) 
                        &&(((GET_INPUT(17000 + 72) >> 0) & 0x1u) == 0u));
    INT8U PCS1BUS2 =(INT8U)((((GET_INPUT(17000 + 69) >> 1) & 0x1u) == 0u) 
                        &&(((GET_INPUT(17000 + 72) >> 1) & 0x1u) == 0u));
    // PCS2:
    
    
    INT8U PCS2BUS1 =(INT8U)((((GET_INPUT(2600 + 300 + 203) >> 0) & 0x1u) == 0u) 
                        &&(((GET_INPUT(2600 + 300 + 211) >> 0) & 0x1u) == 0u));
    INT8U PCS2BUS2 =(INT8U)((((GET_INPUT(2600 + 300 + 203) >> 1) & 0x1u) == 0u) 
                        &&(((GET_INPUT(2600 + 300 + 211) >> 1) & 0x1u) == 0u));

    // PCS3:

    INT8U PCS3BUS1 =(INT8U)((((GET_INPUT(17000 + 69+ 300) >> 0) & 0x1u) == 0u) 
                        &&(((GET_INPUT(17000 + 72 + 300) >> 0) & 0x1u) == 0u));
    INT8U PCS3BUS2 =(INT8U)((((GET_INPUT(17000 + 69 + 300) >> 1) & 0x1u) == 0u) 
                        &&(((GET_INPUT(17000 + 72 + 300) >> 1) & 0x1u) == 0u));


    // PCS4:

    INT8U PCS4BUS1 =(INT8U)((((GET_INPUT(2600 + 900 + 203) >> 0) & 0x1u) == 0u) 
                        &&(((GET_INPUT(2600 + 900 + 211) >> 0) & 0x1u) == 0u));
    INT8U PCS4BUS2 =(INT8U)((((GET_INPUT(2600 + 900 + 203) >> 1) & 0x1u) == 0u) 
                        &&(((GET_INPUT(2600 + 900 + 211) >> 1) & 0x1u) == 0u));


//LOG_INFO("state[0]:%d,state[1]；%d,getinput:%d,(GET_INPUT(29271):%d,(GET_INPUT(29471):%d", state[0], state[1],GET_INPUT(17062),GET_INPUT(29271),GET_INPUT(29471));
 MV_ReactivePowerControl_A(
        GET_HOLD(1011),

        GET_INPUT(2600+300*0+18),
        GET_INPUT(2600+300*1+18),

        GET_INPUT(2600+300*2+18),
        GET_INPUT(2600+300*3+18),

        &reactive_power);
// LOG_INFO("group1 无功 ：%d,group2 无功：%d",reactive_power.group1_q_cmd,reactive_power.group2_q_cmd);


if(BusType==D_BUS)
{



    /*情况1：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2无从机故障，发有功，发无功*/
if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
//  LOG_INFO("情况1");
    if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
     
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);


    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);


    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms5_max_charge_power = GET_INPUT(28838); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms5_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms6_max_charge_power = GET_INPUT(29038); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1)) 
    {
        bms6_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms6_max_discharge_power' */
    } 
    else 
    {
        bms6_max_discharge_power = 0;
    }
    bms6_soc = GET_INPUT(29003);

    
    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms7_max_charge_power = GET_INPUT(29238); /* '<Root>/bms7_max_charge_power' */
    } 
    else 
    {
        bms7_max_charge_power = 0;
    }

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1)) 
    {
        bms7_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms7_max_discharge_power' */
    } 
    else 
    {
        bms7_max_discharge_power = 0;
    }
    bms7_soc = GET_INPUT(29203);

    if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms8_max_charge_power = GET_INPUT(29438); /* '<Root>/bms8_max_charge_power' */
    } 
    else 
    {
        bms8_max_charge_power = 0;
    }

    if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
 if((GET_INPUT(28071) == 3) || (GET_INPUT(28271) == 3)|| (GET_INPUT(28471) == 3)|| (GET_INPUT(28671) == 3)
    || (GET_INPUT(28871) == 3)|| (GET_INPUT(29071) == 3)|| (GET_INPUT(29271) == 3)|| (GET_INPUT(29471) == 3)
    ) //当BMS出现待机时，不发无功
    {
        mv_r_power              = 0;  
    } 
    else 
    {
        mv_r_power              = GET_HOLD(1011);  
    }
    
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500         ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   2500         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;  
    reactive_rate           =  sys_cfg->reactiverate;       

}
/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault==1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
    // LOG_INFO("情况2");
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);


    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

   if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1)) 
    {
        bms4_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29003);

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms5_max_charge_power = GET_INPUT(29238); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1)) 
    {
        bms5_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(29203);

    if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms6_max_charge_power = GET_INPUT(29438); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }

   if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0        ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
}
/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run!=1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
    // LOG_INFO("情况3");
    if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

     if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1))  
    {
        bms4_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(29003);

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms5_max_charge_power = GET_INPUT(29238); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

   if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1))
    {
        bms5_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(29203);

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms6_max_charge_power = GET_INPUT(29438); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;      
    reactive_rate           =  sys_cfg->reactiverate;       
}
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机运行，系统2处于故障，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1))
{
    // LOG_INFO("情况4");
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);



      if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms3_max_charge_power = GET_INPUT(28438); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

  if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28437); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28403);

       if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(28638); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms4_max_discharge_power = GET_INPUT(28637); /* '<Root>/bms4_max_discharge_power' */
    } 
    else 
    {
        bms4_max_discharge_power = 0;
    }
    bms4_soc = GET_INPUT(28603);



    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms5_max_charge_power = GET_INPUT(29238); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

   if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1))
    {
        bms5_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(29203);

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms6_max_charge_power = GET_INPUT(29438); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
}
/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run!=1))
{
    //  LOG_INFO("情况5");
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);

    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms5_max_charge_power = GET_INPUT(28838); /* '<Root>/bms5_max_charge_power' */
    } 
    else 
    {
        bms5_max_charge_power = 0;
    }

     if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms5_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms5_max_discharge_power' */
    } 
    else 
    {
        bms5_max_discharge_power = 0;
    }
    bms5_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms6_max_charge_power = GET_INPUT(29038); /* '<Root>/bms6_max_charge_power' */
    } 
    else 
    {
        bms6_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1))  
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =  2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        
    reactive_rate           =  sys_cfg->reactiverate;     
}

    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run)&&((master2_Run!=1)&&(slave2_Run!=1)))
{
// LOG_INFO("情况6");
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        
    reactive_rate           =  sys_cfg->reactiverate;     
}
/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&((master2_fault!=1)))&&(slave2_Run!=1)))
{
// LOG_INFO("情况7");
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

     if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1))  
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;       
    reactive_rate           =  sys_cfg->reactiverate;  


}
/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{
// LOG_INFO("情况8");
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(29238); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

   if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1))
    {
        bms3_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(29203);

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29438); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;    


}

/*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault!=1)==1)&&(slave2_Run!=1)))
{
//LOG_INFO("情况9");
     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);

    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28838); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

     if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms3_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29038); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1))  
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        
    reactive_rate           =  sys_cfg->reactiverate; 


}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{
//LOG_INFO("情况10");
     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28603);

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(29238); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

   if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1))
    {
        bms3_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(29203);

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29438); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }
     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;       
    reactive_rate           =  sys_cfg->reactiverate;  


}

    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
//LOG_INFO("情况11");
    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms1_max_charge_power = GET_INPUT(28838); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

     if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms2_max_charge_power = GET_INPUT(29038); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1))  
    {
        bms2_max_discharge_power = GET_INPUT(29037); /* '<Root>/bms2_max_discharge_power' */
    } 
    else 
    {
        bms2_max_discharge_power = 0;
    }
    bms2_soc = GET_INPUT(29003);

    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms3_max_charge_power = GET_INPUT(29238); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

   if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1))
    {
        bms3_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms3_max_discharge_power' */
    } 
    else 
    {
        bms3_max_discharge_power = 0;
    }
    bms3_soc = GET_INPUT(29203);

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms4_max_charge_power = GET_INPUT(29438); /* '<Root>/bms4_max_charge_power' */
    } 
    else 
    {
        bms4_max_charge_power = 0;
    }

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
}


    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run)&&(master1_fault!=1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
    //LOG_INFO("情况12");
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;           
    reactive_rate           =  sys_cfg->reactiverate; 
}

/*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault==1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run==1))
{
 //LOG_INFO("情况13");
     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;          
    reactive_rate           =  sys_cfg->reactiverate;  
}

    /*情况14：系统2主机处于故障，且系统2从机运行，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
 //LOG_INFO("情况14");
    if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 2))&&(PCS4BUS1==1)) 
    {
        bms1_max_charge_power = GET_INPUT(29238); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

   if(((GET_INPUT(29271) == 0) || (GET_INPUT(29271) == 1))&&(PCS4BUS1==1))
    {
        bms1_max_discharge_power = GET_INPUT(29237); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(29203);

     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 2))&&(PCS4BUS2==1)) 
    {
        bms2_max_charge_power = GET_INPUT(29438); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
     if(((GET_INPUT(29471) == 0) || (GET_INPUT(29471) == 1))&&(PCS4BUS2==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;          
    reactive_rate           =  sys_cfg->reactiverate;   
}
    /*情况15：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1))&&(slave2_Run!=1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    //LOG_INFO("情况15");
    if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 2))&&(PCS3BUS1==1)) 
    {
        bms1_max_charge_power = GET_INPUT(28838); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

     if(((GET_INPUT(28871) == 0) || (GET_INPUT(28871) == 1))&&(PCS3BUS1==1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28837); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28803);

    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 2))&&(PCS3BUS2==1)) 
    {
        bms2_max_charge_power = GET_INPUT(29038); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if(((GET_INPUT(29071) == 0) || (GET_INPUT(29071) == 1))&&(PCS3BUS2==1))  
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
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
//LOG_INFO("group1 无功 ：%d,group2 无功：%d",reactive_power.group1_q_cmd,reactive_power.group2_q_cmd);
    (pcspowerallocate[8]!=reactive_power.group1_q_cmd) ||
    (pcspowerallocate[9]!=reactive_power.group2_q_cmd) ||
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
        pcspowerallocate[8]=reactive_power.group1_q_cmd;
        pcspowerallocate[9]=reactive_power.group2_q_cmd;


        PCS_Write_Flag[0]=true;
        PCS_Write_Flag[2]=true;
        PCSaddr[0]=1010;
        PCSaddr[2]=1010;
    

LOG_INFO("pcs1_power:%d,pcs2_power:%d,pcs3_power:%d,pcs4_power:%d,pcs5_power:%d,pcs6_power:%d,pcs7_power:%d,pcs8_power:%d,pcs_r_power1_2:%d,pcs_r_power3_4:%d",pcs1_power,pcs2_power,pcs3_power,pcs4_power,pcs5_power,pcs6_power,pcs7_power,pcs8_power,reactive_power.group1_q_cmd,reactive_power.group2_q_cmd);


    }


}
else if(BusType==S_BUS)
{
    /*情况1：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2无从机故障，发有功，发无功*/
if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);


     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms3_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms3_max_discharge_power= 0;
    }
    bms3_soc                = GET_INPUT(28403);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms4_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms4_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
    {
        bms4_max_discharge_power= GET_INPUT(28637);
    }
    else
    {
        bms4_max_discharge_power= 0;
    }
    bms4_soc                = GET_INPUT(28603);


    //     bms5_max_charge_power = 0;



    //     bms5_max_discharge_power = 0;

    // bms5_soc = 0;


    //     bms6_max_charge_power = 0;


    //     bms6_max_discharge_power = 0;

    // bms6_soc = 0;

    

    //     bms7_max_charge_power = 0;



    //     bms7_max_discharge_power = 0;

    // bms7_soc = 0;


    //     bms8_max_charge_power = 0;



    //     bms8_max_discharge_power = 0;

    // bms8_soc = 0;
    bus2_or_bus1            =0;              /* '<Root>/bus2_or_bus1' */
    module_num              =4;              /* '<Root>/module_num' */
    mv_max_power            =10000;          /* '<Root>/mv_max_power' */
    mv_power                = GET_HOLD(1010);
    mv_r_power              = GET_HOLD(1011);  
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500         ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   2500         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
    reactive_rate           =  sys_cfg->reactiverate; 
 //  LOG_INFO("reactive_rate :%d",mv_r_power);

}
/*情况2：系统1，主机处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault==1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms2_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28403);


       if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28638); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0        ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;            
    reactive_rate           =  sys_cfg->reactiverate; 
}
/*情况3：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run!=1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run==1))
{
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
    {
        bms2_max_discharge_power= GET_INPUT(28437);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28403);

       if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28638); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
}
/*情况4：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机无故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1))
{
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);



       if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1)) 
    {
        bms3_max_charge_power = GET_INPUT(28638); /* '<Root>/bms3_max_charge_power' */
    } 
    else 
    {
        bms3_max_charge_power = 0;
    }

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
}
/*情况5：系统1处于正常运行或者告警运行，但是系统1从机无故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run==1)&&((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run!=1))
{
    
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms2_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms2_max_discharge_power= 0;
    }
    bms2_soc                = GET_INPUT(28203);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms3_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms3_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =  2500       ;                             
    pcs3_rated_power        =   2500         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;      
    reactive_rate           =  sys_cfg->reactiverate;       
}

    /*情况6：系统1处于正常运行或者告警运行，且系统1从机没有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault!=1))&&(slave1_Run)&&((master2_Run!=1)&&(slave2_Run!=1)))
{

      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;        
}
/*情况7：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&((master2_fault!=1)))&&(slave2_Run!=1)))
{

      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;
    reactive_rate           =  sys_cfg->reactiverate;         


}
/*情况8：系统1处于正常运行或者告警运行，但是系统1从机有故障，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault!=1))&&((slave1_Run!=1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{

      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
    {
        bms1_max_discharge_power= GET_INPUT(28037);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28003);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        
    reactive_rate           =  sys_cfg->reactiverate; 

}

/*情况9：系统1处于故障，但是系统1从机运行，系统2处于正常运行或者告警运行，系统2从机故障，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault!=1)==1)&&(slave2_Run!=1)))
{

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);

     if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms2_max_charge_power   = GET_INPUT(28438);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
    if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        
    reactive_rate           =  sys_cfg->reactiverate; 

}
/*情况10：系统1处于故障，但是系统1从机运行，系统2处于主机故障，系统2从机运行，发有功，不发无功*/

else if((((master1_Run==1)&&(master1_fault==1))&&((slave1_Run==1))&&((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)))
{

        if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
    {
        bms1_max_discharge_power= GET_INPUT(28237);
    }
    else
    {
        bms1_max_discharge_power= 0;
    }
    bms1_soc                = GET_INPUT(28203);

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1))
    {
        bms2_max_charge_power   = GET_INPUT(28638);
    }
    else
    {
        bms2_max_charge_power   = 0;
    }
     if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;        
    reactive_rate           =  sys_cfg->reactiverate; 
}

    /*情况11：系统2处于正常运行或者告警运行，且系统2从机没有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1)&&(master2_fault!=1))&&(slave2_Run)&&((master1_Run!=1)&&(slave1_Run!=1)))
{

      if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms1_max_charge_power = GET_INPUT(28438); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

  if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1)) 
    {
        bms1_max_discharge_power = GET_INPUT(28437); /* '<Root>/bms1_max_discharge_power' */
    } 
    else 
    {
        bms1_max_discharge_power = 0;
    }
    bms1_soc = GET_INPUT(28403);

       if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1)) 
    {
        bms2_max_charge_power = GET_INPUT(28638); /* '<Root>/bms2_max_charge_power' */
    } 
    else 
    {
        bms2_max_charge_power = 0;
    }
    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   2500         ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;   
    reactive_rate           =  sys_cfg->reactiverate;          
}


    /*情况12：系统1处于正常运行或者告警运行，且系统1从机有故障，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run)&&(master1_fault!=1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run!=1))
{
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==2))&&(PCS1BUS1==1))
    {
        bms1_max_charge_power   = GET_INPUT(28038);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
      if(((GET_INPUT(28071)==0)||(GET_INPUT(28071)==1))&&(PCS1BUS1==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;  
    reactive_rate           =  sys_cfg->reactiverate;          
}

/*情况13：系统1处于主机故障，且系统1从机运行，系统2不处于运行状态（切除系统2），发有功，不发无功*/
else if(((master1_Run==1)&&(master1_fault==1))&&((master2_Run!=1)&&(slave2_Run!=1))&&(slave1_Run==1))
{
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==2))&&(PCS1BUS2==1))
    {
        bms1_max_charge_power   = GET_INPUT(28238);
    }
    else
    {
        bms1_max_charge_power   = 0;
    }
     if(((GET_INPUT(28271)==0)||(GET_INPUT(28271)==1))&&(PCS1BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =  0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;     
    reactive_rate           =  sys_cfg->reactiverate;       
}

    /*情况14：系统2主机处于故障，且系统2从机运行，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1)&&(master2_fault==1))&&(slave2_Run==1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    
       if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==2))&&(PCS2BUS2==1)) 
    {
        bms1_max_charge_power = GET_INPUT(28638); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

    if(((GET_INPUT(28671)==0)||(GET_INPUT(28671)==1))&&(PCS2BUS2==1))
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;  
    reactive_rate           =  sys_cfg->reactiverate;           
}
    /*情况15：系统2处于正常运行或者告警运行，且系统2从机有故障，系统1不处于运行状态（切除系统1），发有功，不发无功*/
else if(((master2_Run==1))&&(slave2_Run!=1)&&((master1_Run!=1)&&(slave1_Run!=1)))
{
    
      if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==2))&&(PCS2BUS1==1))
    {
        bms1_max_charge_power = GET_INPUT(28438); /* '<Root>/bms1_max_charge_power' */
    } 
    else 
    {
        bms1_max_charge_power = 0;
    }

  if(((GET_INPUT(28471)==0)||(GET_INPUT(28471)==1))&&(PCS2BUS1==1)) 
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
    pcs1_rated_power        =   2500         ;                              
    pcs2_rated_power        =   0       ;                             
    pcs3_rated_power        =   0         ;             
    pcs4_rated_power        =   0         ;             
    target_h_soc            = 1000 ;             
    target_l_soc            = 0 ;    
    reactive_rate           =  sys_cfg->reactiverate;        
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


    (pcspowerallocate[8]!=reactive_power.group1_q_cmd) ||
    (pcspowerallocate[9]!=reactive_power.group2_q_cmd) ||
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


        pcspowerallocate[8]=reactive_power.group1_q_cmd;
        pcspowerallocate[9]=reactive_power.group2_q_cmd;


        PCS_Write_Flag[0]=true;
        PCS_Write_Flag[2]=true;
        PCSaddr[0]=1010;
        PCSaddr[2]=1010;
    

LOG_INFO("pcs1_power:%d,pcs2_power:%d,pcs3_power:%d,pcs4_power:%d,pcs5_power:%d,pcs6_power:%d,pcs7_power:%d,pcs8_power:%d,pcs_r_power1_2:%d,pcs_r_power3_4:%d",pcs1_power,pcs2_power,pcs3_power,pcs4_power,pcs5_power,pcs6_power,pcs7_power,pcs8_power,reactive_power.group1_q_cmd,reactive_power.group2_q_cmd);


    }


}
}
void Get_System_Time(SYS_TIME_BUF *tbuf)
{
    struct timeval tv;
    struct tm *tm_now;

    // 获取当前时间（秒 + 微秒）
    gettimeofday(&tv, NULL);

    // 转换为本地时间
    tm_now = localtime(&tv.tv_sec);

    tbuf->year  = tm_now->tm_year + 1900;
    tbuf->month = tm_now->tm_mon + 1;
    tbuf->day   = tm_now->tm_mday;

    tbuf->hour  = tm_now->tm_hour;
    tbuf->minute= tm_now->tm_min;
    tbuf->second= tm_now->tm_sec;

    // 微秒转毫秒
    tbuf->millisecond = tv.tv_usec / 1000;
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
void  PCS_TAida_write_Task(const char *_num)
{
    //初始化
    const int pcs_num = (int)(unsigned char)(*_num);

    // 防越界
    if (pcs_num < 0 || pcs_num >= 10) {
        LOG_INFO("PCS_wirte_task got invalid pcs_num=%d", pcs_num);
        return;
    }
    sysPara* sys_cfg = SysConf_GetInfo();
    (void)sys_cfg;

    LOG_INFO("PCS_wirte_task-%d", pcs_num);

    // 设置线程属性
    pthread_attr_t thread_attrR;
    pthread_attr_init(&thread_attrR);
    pthread_attr_setdetachstate(&thread_attrR, PTHREAD_CREATE_DETACHED);

    // 创建线程：★ 按值传参
    pthread_t thread_pcs;
    pthread_create(&thread_pcs, &thread_attrR, Pcs_Taida_Write_Task, (void*)(intptr_t)pcs_num);

    pthread_create(&thread_pcs, &thread_attrR, PCS_HB_Task, (void*)(intptr_t)pcs_num);
    while(1) {
        usleep(500*1000);
    }
}

static inline uint64_t Now_Ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ull + (uint64_t)(ts.tv_nsec / 1000000ull);
}
/*PCS心跳线程*/
/**
 * @brief PCS心跳任务线程函数
 * @details 负责与指定PCS设备建立连接、维护心跳通信，并在连接断开时自动重连。
 *          该线程包含两个主要循环：
 *          1. 外层循环：处理连接建立、断线重连和退避策略
 *          2. 内层循环：心跳通信的持续维护
 * 
 * @param arg 线程参数，转换为PCS设备编号(int类型)
 * @return void* 返回NULL表示线程正常退出
 * 
 * @note 包含特殊字符处理：\t(制表符)、\r(回车符)、\n(换行符)
 * 
 * 功能流程：
 * 1. 初始化系统参数和心跳句柄
 * 2. 尝试建立TCP连接(带退避策略)
 * 3. 启动或更新心跳通信
 * 4. 维护连接状态
 * 5. 处理异常情况和资源释放
 */
void* PCS_HB_Task(void*arg)
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
        if ((socket_Heart_Pcs[pcs_num] = Create_Client_Socket(server_addr, time_out)) == -1) {
            LOG_INFO("PCS-%d Create hb socket failure! ip:%s[port:%d]",
                    pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
            sleep(5);
            continue;
        }

        LOG_INFO("pcs-%d: hb Connect SUCCESS! ip:%s[port:%d]",
                pcs_num, sys_cfg->pcs_ip[pcs_num], sys_cfg->pcs_port[pcs_num]);
        timeout_cnt=0;  
    
        uint64_t now = Now_Ms();
        next_hb_ms   = now + hb_period_ms;
        next_read_ms = now + read_period_ms;  
        next_hour_cmd_ms = now + hour_cmd_period_ms;
while (1) {
            now = Now_Ms();

            // 1) 到点发送心跳写
            if (now >= next_hb_ms) {
                next_hb_ms += hb_period_ms; 

                hb ^= 1u;
                INT16U newv = hb ? (INT16U)16 : (INT16U)0;  // bit4

    
                if (newv != Control_Word1_Buf[pcs_num]) {
                    Modbus_TCP_Write_Heart(socket_Heart_Pcs[pcs_num],
                                        0x01, 
                                        0x06, 
                                        PCS_Taida_OPRATE_addr,
                                        newv, 
                                        CMD_DELAY_200);
                    Control_Word1_Buf[pcs_num] = newv;
                }
            }

            // 2) 到点发送读请求
            if (now >= next_read_ms) {
                next_read_ms += read_period_ms;

                Modbus_TCP_Read(socket_Heart_Pcs[pcs_num],
                                PCS_Taida_SLAVE_ADDR,
                                MODBUS_READ_TYPE_04,
                                Each_PCS_Taida_addr2,
                                Each_PCS_Taida_size2,
                                CMD_DELAY_200);
            }
            if ((now >= next_hour_cmd_ms)||(Set_Time_Flag==true)) {
                Set_Time_Flag=false;
                next_hour_cmd_ms += hour_cmd_period_ms;
                Get_System_Time(&time);
                time_buf[0] = time.year;
                time_buf[1] = time.month;
                time_buf[2] = time.day;
                time_buf[3] = time.hour;
                time_buf[4] = time.minute;
                time_buf[5] = time.second;
                time_buf[6] = time.millisecond ;
            for(int num=0;num<sys_cfg->pcsNum;num++)
            {
                Modbus_TCP_Write_Multiple(socket_Heart_Pcs[num], 
                PCS_Taida_SLAVE_ADDR,
                Time_Set_Addr,
                7, 
                time_buf, 
                200);   
            }

            }
            usleep(20*1000);
            int16_t numbytes = Recv_Modbus_Back(socket_Heart_Pcs[pcs_num],
                                                pcs_num,
                                                PCS_BUFF_LEN,
                                                PCS_Taida_Write_Heart_DataProcess);
            if (numbytes < 0) {
                close(socket_Heart_Pcs[pcs_num]);
                socket_Heart_Pcs[pcs_num]=-1;
                break;
            } else if (numbytes == 0) {
                timeout_cnt++;
                if (timeout_cnt >= 75) {
                    close(socket_Heart_Pcs[pcs_num]);
                    socket_Heart_Pcs[pcs_num]=-1;
                    break;
                }
            } else {
                timeout_cnt = 0;
            }
        }
    }
    return NULL;
}


static PfLatch g_latch[2] = {0};

/**
 * @brief 执行功率因数转换处理
 * @details 该函数处理PCS（功率转换系统）的功率因数转换逻辑，包括：
 *          - 将PCS编号归一化为系统索引
 *          - 初始化时读取初始值
 *          - 监测有功功率(P)和功率因数(PF)的变化
 *          - 实现变化合并窗口和超时机制
 *          - 执行P-PF到Q-S的转换计算
 *          - 生成设备控制指令并更新结果
 * 
 * @param pcs_num PCS编号，用于标识不同的PCS系统
 *                 - 0表示系统0（PCS0）
 *                 - 2表示系统1（PCS2）
 * @return 无返回值
 * 
 * @note 依赖全局变量：
 *       - g_latch[]: 存储锁存状态
 *       - g_convert[]: 存储转换结果
 *       - First_Flag[]: 初始化标志
 *       - PCSactivepower[]: 有功功率值
 *       - PCSPF[]: 功率因数值
 *       - PFConvert_Write_Flag[]: 写入标志
 * @note 依赖外部函数：
 *       - GET_HOLD(): 读取保持寄存器值
 *       - Now_Ms(): 获取当前毫秒时间
 *       - P_PF_to_QS(): 计算无功功率Q和视在功率S
 *       - P_PF_to_Spf(): 计算设备指令参数
 *       - lroundf(): 浮点数四舍五入
 *       - LOG_INFO(): 日志输出
 */
/**
 * @brief 功率因数转换函数
 * @details 将PCS系统的有功功率和功率因数转换为视在功率和功率因数指令值
 * 
 * @param pcs_num PCS系统编号，0表示PCS0，2表示PCS2
 * 
 * 功能说明：
 * 1. 将PCS系统编号归一化为系统索引
 * 2. 首次运行时从寄存器读取初始值
 * 3. 监控有功功率(P)和功率因数(PF)的变化
 * 4. 实现变化值的合并窗口机制：
 *    - 等待P和PF都变化或超时
 *    - 超时情况下使用新值与上一次值组合
 * 5. 执行功率转换计算：
 *    - 计算无功功率Q和视在功率S
 *    - 生成设备指令值S_cmd和pf_cmd
 * 6. 记录日志并更新状态
 * 
 * @note 内部使用全局变量：
 * - g_latch：存储上一次的状态信息
 * - g_convert：存储转换结果
 * - First_Flag：首次运行标志
 * - PCSactivepower：有功功率值
 * - PCSPF：功率因数值
 * - PFConvert_Write_Flag：写入标志
 */

void PF_Convert(INT16U pcs_num)
{
    INT16U idx = (pcs_num != 0) ? 1u : 0u;

    PfLatch      *Last     = &g_latch[idx];
    PF_CONVERT_t *cvresult = &g_convert[idx];

    INT16U addrP  = 27052 + 300 * idx;
    INT16U addrPF = 27110 + 300 * idx;

    static INT16S s_webP_raw[2]      = {0};
    static INT16S s_webPF_raw[2]     = {0};
    static INT16S s_lastOutS_raw[2]  = {0};
    static INT16S s_lastOutPF_raw[2] = {0};
    static bool   s_hasWebRaw[2]     = {false};
    static bool   s_hasOutRaw[2]     = {false};

    if (First_Flag[idx] == 1) {
        First_Flag[idx] = 0;

        INT16S initP  = (INT16S)GET_HOLD(addrP);
        INT16S initPF = (INT16S)GET_HOLD(addrPF);

        PCSactivepower[idx] = initP;
        PCSPF[idx]          = initPF;

        s_webP_raw[idx]  = initP;
        s_webPF_raw[idx] = initPF;
        s_hasWebRaw[idx] = true;

        PFConvert_Write_Flag[idx] = false;

        LOG_INFO("PF_Convert init idx=%u: addrP=%u P=%d, addrPF=%u PF=%d",
                 idx, addrP, initP, addrPF, initPF);
    }

    if (!Last->inited) {
        INT16S P0  = (INT16S)PCSactivepower[idx];
        INT16S PF0 = (INT16S)PCSPF[idx];

        Last->lastP_raw  = (INT16S)P0;
        Last->lastPF_raw = (INT16S)PF0;
        Last->seen_mask  = 0;
        Last->t_start_ms = 0;
        Last->inited     = true;

        s_webP_raw[idx]  = P0;
        s_webPF_raw[idx] = PF0;
        s_hasWebRaw[idx] = true;

        PFConvert_Write_Flag[idx] = false;

        return;
    }

    INT16S curP_raw  = (INT16S)PCSactivepower[idx];
    INT16S curPF_raw = (INT16S)PCSPF[idx];

    INT16S curP  = curP_raw;
    INT16S curPF = curPF_raw;
 // LOG_INFO("curP_raw=%d, curPF_raw=%d", curP_raw, curPF_raw);
    if (s_hasWebRaw[idx] && s_hasOutRaw[idx]) {
        if ((curP_raw == s_lastOutS_raw[idx]) &&
            (curPF_raw == s_lastOutPF_raw[idx])) {

            curP  = s_webP_raw[idx];
            curPF = s_webPF_raw[idx];

            PFConvert_Write_Flag[idx] = false;

        //    LOG_INFO("PF_Convert idx=%u ignore output feedback: rawP=%d rawPF=%d -> webP=%d webPF=%d",
        //             idx, curP_raw, curPF_raw, curP, curPF);
        }

    }

    INT16S lastP_cmp  = (INT16S)Last->lastP_raw;
    INT16S lastPF_cmp = (INT16S)Last->lastPF_raw;

    // LOG_INFO("PF_Convert check pcs_num=%u idx=%u addrP=%u addrPF=%u "
    //          "rawP=%d rawPF=%d curP=%d curPF=%d lastP=%d lastPF=%d seen=0x%02x writeFlag=%d",
    //          pcs_num,
    //          idx,
    //          addrP,
    //          addrPF,
    //          curP_raw,
    //          curPF_raw,
    //          curP,
    //          curPF,
    //          lastP_cmp,
    //          lastPF_cmp,
    //          Last->seen_mask,
    //          PFConvert_Write_Flag[idx]);

    bool p_changed  = (curP  != lastP_cmp);
    bool pf_changed = (curPF != lastPF_cmp);

    if (p_changed) {
        Last->pendP_raw  = (INT16S)curP;
        Last->seen_mask |= 0x01u;
    }

    if (pf_changed) {
        Last->pendPF_raw = (INT16S)curPF;
        Last->seen_mask |= 0x02u;
    }

    if (Last->seen_mask == 0) {
        PFConvert_Write_Flag[idx] = false;
        return;
    }

    uint64_t now = Now_Ms();

    if ((Last->seen_mask & 0x03u) == 0x03u) {
        /* P 和 PF 都到齐 */
    } else {
        if (Last->t_start_ms == 0) {
            Last->t_start_ms = now;
            PFConvert_Write_Flag[idx] = false;
            return;
        }

        if (now - Last->t_start_ms < TIMEOUT_MS) {
            PFConvert_Write_Flag[idx] = false;
            return;
        }

        if ((Last->seen_mask & 0x01u) == 0x00u) {
            Last->pendP_raw = (INT16S)lastP_cmp;
            Last->seen_mask |= 0x01u;
        }

        if ((Last->seen_mask & 0x02u) == 0x00u) {
            Last->pendPF_raw = (INT16S)lastPF_cmp;
            Last->seen_mask |= 0x02u;
        }
    }

    const INT16S P_raw  = (INT16S)Last->pendP_raw;
    const INT16S PF_raw = (INT16S)Last->pendPF_raw;

    const float P  = (float)P_raw;
    const float PF = (float)PF_raw / 100.0f;

    float Q      = 0.0f;
    float S      = 0.0f;
    float S_cmd  = 0.0f;
    float pf_cmd = 0.0f;
    bool convert_flag=false;
   convert_flag= P_PF_to_S_pf_cmd(P, PF, &S_cmd, &pf_cmd);

    cvresult->S  = (INT16S)lroundf(S_cmd);
    cvresult->PF = (INT16S)lroundf(pf_cmd * 100.0f);

    s_webP_raw[idx]  = (INT16S)P_raw;
    s_webPF_raw[idx] = (INT16S)PF_raw;
    s_hasWebRaw[idx] = true;

    s_lastOutS_raw[idx]  = (INT16S)cvresult->S;
    s_lastOutPF_raw[idx] = (INT16S)cvresult->PF;
    s_hasOutRaw[idx]    = true;
    // LOG_INFO("convert_flag :%d",convert_flag);
    if(convert_flag)
    {
         PFConvert_Write_Flag[idx] = true;   

    }


    float P_dev = fabsf(S_cmd) * pf_cmd;
    float Q_dev = S_cmd * sqrtf(fmaxf(0.0f, 1.0f - pf_cmd * pf_cmd));

    LOG_INFO("PF_Convert idx=%u: P=%8.1f kW, PF=%+6.3f | Q=%+8.1f kVar, S=%8.1f kVA | "
             "S_cmd=%+8.1f, pf_cmd=%+7.4f | Dev=> P=%8.1f, Q=%8.1f",
             idx,
             P,
             PF,
             Q,
             S,
             S_cmd,
             pf_cmd,
             P_dev,
             Q_dev);

    Last->lastP_raw  = (INT16S)P_raw;
    Last->lastPF_raw = (INT16S)PF_raw;
    Last->seen_mask  = 0;
    Last->t_start_ms = 0;
}
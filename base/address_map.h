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

 #ifndef ADDRESS_MAP_H
 #define ADDRESS_MAP_H
#include "system.h"
 
 #define HOLD_SIZE 65535
 #define INPUT_SIZE 65535
 #define ALL_SIZE (INPUT_SIZE + HOLD_SIZE)
 
 
 // 联合体结构定义寄存器模块
 typedef union RegistersModel {   
     struct {
         unsigned short Input[INPUT_SIZE];
         unsigned short Hold[HOLD_SIZE];
     };
     unsigned short Reg[ALL_SIZE];
     unsigned char ubyte[ALL_SIZE*2];
 } RegistersModel,*pRegistersModel;
 
 extern volatile RegistersModel RegM;
 int Find_Bank_N(int addr, int *base_out);
 int Trina_Hold_Address(int pcs_index, int index);
 int Find_Trina_N(int addr, int *base_out);
 INT32S Taida_Addr_From_Index(INT8U pcs_index, INT32S index);
int CEM9000_INPUT(INT16U address, INT16U value);
int Write_Bits_By_Addr(INT16U addr, INT16U value,INT16U pcs_num);
int CEM9000_HOLD(INT16S value);
 INT32S G2pro_Addr_From_Index(INT8U bms_index, INT32S index);
 INT32S Trina_Addr_From_Index(INT8U pcs_index, INT32S index);
 // 反向映射：index → address
 #define HOLD_ADDRESS(index) 


 // 正向映射：address → index
 #define HOLD_INDEX(address)




//  // 宏定义获取映射值（同时检查有效索引）
 #define GET_HOLD(address)    \ 
  ({ \
         RegM.Hold[address]; \
 })   
 
 #define SET_HOLD(address, val)  \ 
  ({ \
         RegM.Hold[address]=val; \
 })  
     

 // 正向映射：address → index
 #define INPUT_INDEX(address) (address)

 // 反向映射：index → address
 #define INPUT_ADDRESS(index)  (index)

 
 #define BANK_SIZE 10
 #define RACK_SIZE 15
 #define R_INPUT_SIZE 1152
 #define R_ALL_SIZE (BANK_SIZE * RACK_SIZE * R_INPUT_SIZE)
 
 typedef union RankModel {
     struct {
    unsigned short Input[BANK_SIZE][RACK_SIZE * R_INPUT_SIZE];
     };
     unsigned short Reg[R_ALL_SIZE];
     unsigned char ubyte[R_ALL_SIZE * 2];
 } RankModel, *pRankModel;
 
 extern volatile RankModel RankM;
  // 宏定义获取映射值
// ================= PE PCS Index宏 ===================
 // 正向映射：address → index，实际设备地址映射对外地址
//  #define PE_INPUT_INDEX(address) \
//  ((address) == 4326 ? 125: /*有功功率低压*/\
//   (address) == 4327 ? 127 : /*无功功率低压*/\
//   (address) == 4329 ? 122 : /*低压功率因数值*/\
//   (address) == 1003 ? 106 : /*当前状态*/\
//   (address) == 6513 ? 2630 : /*模块1输出有功功率*/\
//   (address) == 6532 ? 2893 : /*模块1湿度*/\
//   (address) == 6548 ? 2930 : /*模块2输出有功功率*/\
//   (address) == 6567 ? 3193 : /*模块2湿度*/\
//   (address) == 6583 ? 3230 : /*模块3输出有功功率*/\
//   (address) == 6611 ? 3493 : /*模块3湿度*/\
//   (address) == 6618 ? 3530 : /*模块4输出有功功率*/\
//   (address) == 6637 ? 3793 : /*模块4湿度*/\
//   (address) == 572 ? 153 : /*总运行时间-年*/\
//   (address) == 573 ? 155 : /*总运行时间-日*/\
//   (address) == 1003 ? 106 : /*当前状态*/\
//   (address) == 4300 ? 25822: /*中压RS相电压*/\
//   (address) == 4301 ? 25824 : /*中压ST相电压*/\
//   (address) == 4302 ? 25826 : /*中压TR相电压*/\
//   (address) == 4303 ? 25800 : /*中压相电流1*/\
//   (address) == 4304 ? 25802 : /*中压相电流2*/\
//   (address) == 4305 ? 25804 : /*中压相电流3*/\
//   (address) == 4306 ? 25808 : /*中压有功*/\
//   (address) == 4307 ? 25810 : /*中压无功*/\
//   (address) == 4308 ? 25812 : /*中压视在功率*/\
//   (address) == 4309 ? 25814 : /*中压功率因数*/\
//   ((address) >= 574 && (address) < 576 ? ((address) - (574) + 156) : /*总运行时间-时*/\
//   ((address) >= 1512 && (address) < 1544 ? ((address) -(1512)+260) : /*告警码1～告警码32*/\
//   ((address) >= 4320 && (address) < 4326 ? ((address) -(4320)+270) : /*低压RS相电压～低压相电流3*/\
//   ((address) >= 4331 && (address) < 4333 ? ((address) -(4331)+129) : -1 )))))/*电网频率值*/\




/**********************************************PE寄存器地址映射 **********************************************************************/  
#define PE_INPUT_INDEX(address) ( \
    ((address) == 4326) ? 125 : /* 有功功率低压 */ \
    ((address) == 4327) ? 127 : /* 无功功率低压 */ \
    ((address) == 4329) ? 122 : /* 低压功率因数值 */ \
    ((address) == 6513) ? 2630 : /* 模块1输出有功功率 */ \
    ((address) == 6532) ? 2893 : /* 模块1湿度 */ \
    ((address) == 6548) ? 2930 : /* 模块2输出有功功率 */ \
    ((address) == 6567) ? 3193 : /* 模块2湿度 */ \
    ((address) == 6583) ? 3230 : /* 模块3输出有功功率 */ \
    ((address) == 6611) ? 3493 : /* 模块3湿度 */ \
    ((address) == 6618) ? 3530 : /* 模块4输出有功功率 */ \
    ((address) == 6637) ? 3793 : /* 模块4湿度 */ \
    ((address) == 572) ? 153 : /* 总运行时间-年 */ \
    ((address) == 573) ? 155 : /* 总运行时间-日 */ \
    ((address) == 4300) ? 25822 : /* 中压RS相电压 */ \
    ((address) == 4301) ? 25824 : /* 中压ST相电压 */ \
    ((address) == 4302) ? 25826 : /* 中压TR相电压 */ \
    ((address) == 4303) ? 25800 : /* 中压相电流1 */ \
    ((address) == 4304) ? 25802 : /* 中压相电流2 */ \
    ((address) == 4305) ? 25804 : /* 中压相电流3 */ \
    ((address) == 4306) ? 25808 : /* 中压有功 */ \
    ((address) == 4307) ? 25810 : /* 中压无功 */ \
    ((address) == 4308) ? 25812 : /* 中压视在功率 */ \
    ((address) == 4309) ? 25814 : /* 中压功率因数 */ \
    (((address) >= 574) && ((address) < 576)) ? ((address) - 574 + 156) : \
    (((address) >= 1512) && ((address) < 1544)) ? ((address) - 1512 + 260) : \
    (((address) >= 4320) && ((address) < 4326)) ? ((address) - 4320 + 204) : \
    (((address) >= 4331) && ((address) < 4333)) ? ((address) - 4331 + 129) : \
    -1 \
)
//PE的PCS设置地址，实际设备地址映射对外地址
  #define PE_HOLD_INDEX(address) \
  (address) == 281 ? 7: /*母线功率阀值*/\
  (address) == 38 ? 6: /*PCS起停*/\
  (address) == 41 ? 1490 : /*故障复位*/\
  (address) == 262 ? 27000 : /*舱1下功率*/\
  (address) == 263 ? 27000+300:  /*舱2下功率*/\
  (address) == 5 ? 1505: /*系统对时秒～秒*/\
  (address) == 6 ? 1504: /*系统对时秒～分*/\
  (address) == 7 ? 1503 : /*系统对时秒～时*/\
  (address) == 8 ? 1502 : /*系统对时秒～日*/\
  (address) == 9 ? 1501:  /*系统对时秒～月*/\
  (address) == 10 ? 1500: -1 /*系统对时秒～年*/\


 // 反向映射：index → address，对外地址映射实际设备地址
  #define PE_HOLD_ADDRESS(index) \
  (index) == 1505 ? 5: /*系统对时秒～秒*/\
  (index) == 1504 ? 6: /*系统对时秒～分*/\
  (index) == 1503 ? 7 : /*系统对时秒～时*/\
  (index) == 1502 ? 8 : /*系统对时秒～日*/\
  (index) == 1501 ? 9:  /*系统对时秒～月*/\
  (index) == 1500 ? 10:  /*系统对时秒～年*/\
  (index) == 7 ? 281 : \
  (index) == 6 ? 38 : \
  (index) == 1490 ? 41 :  \
  (index) == 27000 ? 262 : /*舱1下功率*/\
  (index) == 27000+300 ?263 :-1 /*舱2下功率*/\
  /**********************************************END **********************************************************************/  


/**********************************************天合寄存器地址映射 **********************************************************************/  

#define Trina_INPUT_INDEX(pcs_index, address) ( \
  ((address) >= 4000 && (address) < 4012 ? ((address) - 4000 + 2600+0+300*(pcs_index-1)) : /*硬件版本～MS_FPGA变更版本*/\
  ((address) >= 4023 && (address) < 4039 ? ((address) - 4023 + 2600+23+300*(pcs_index-1)) : /*DSP故障信息表1～FPGA告警信息表4*/\
  ((address) >= 4050 && (address) < 4063 ? ((address) - 4050 + 2600+50+300*(pcs_index-1)) : /*ARM与控制DSP间的通讯故障1～ARM系统级故障*/\
  ((address) >= 4064 && (address) < 4088 ? ((address) - 4064 + 2600+64+300*(pcs_index-1)) : /*累积充放电量*/\
  ((address) >= 4100 && (address) < 4149 ? ((address) - 4100 + 2600+100+300*(pcs_index-1)) : /*运行状态～电池功率*/\
  ((address) >= 4502 && (address) < 4528 ? ((address) - 4502 + 2600+152+300*(pcs_index-1)) : /*DSP故障信息1～IMD软件版本号*/\
  ((address) >= 4550 && (address) < 4570 ? ((address) - 4550 + 2600+200+300*(pcs_index-1)) : /*电感电流R～ISO2Rn*/\
  ((address) >= 4580 && (address) < 4626 ? ((address) - 4580 + 2600+230+300*(pcs_index-1)) : -1 ))))))))/*R相风扇速度～ T相IGBT12温度（运行信息）*/\
)


#define Trina_HOLD_INDEX(pcs_index, address) ( \
  ((address) >= 3000 && (address) < 3037 ? ((address) - 3000 + 12000+0+700*(pcs_index-1)) : /*并联模式～支路充电分配系数*/\
  ((address) >= 3077 && (address) < 3079 ? ((address) - 3077 + 12000+77+700*(pcs_index-1)) : /*升级信号～恢复出厂设置*/\
  ((address) >= 3086 && (address) < 3090 ? ((address) - 3086 + 12000+86+700*(pcs_index-1)) : /*IP地址*/\
  ((address) >= 3094 && (address) < 3100 ? ((address) - 3094 + 12000+94+700*(pcs_index-1)) : /*时间设置*/\
  ((address) >= 3100 && (address) < 3250 ? ((address) - 3100 + 12000+100+700*(pcs_index-1)) : /*庵桂参数*/\
  ((address) >= 3250 && (address) < 3350 ? ((address) - 3250 + 12000+250+700*(pcs_index-1)) : /*庵桂参数*/\
  ((address) >= 3350 && (address) < 3410 ? ((address) - 3350 + 12000+350+700*(pcs_index-1)) : /**/\
  ((address) >= 3800 && (address) < 3822 ? ((address) - 3800 + 12000+440+700*(pcs_index-1)) : /*桥臂电流ak~PCC电流T相B*/\
  ((address) >= 3900 && (address) < 3972 ? ((address) - 3900 + 12000+534+700*(pcs_index-1)) :   -1 )))))))))/*交流电感瞬时数值过流～ BUS2绝缘阻抗故障时间*/\
)


/***********************************************END************************************************************************************/


/**********************************************cem9000 **********************************************************************/  

#define Cem9000_INPUT_INDEX(address) ( \
  ((address) >= 0 && (address) < 34 ? ((address) - 0 + 27450) : \
  ((address) >= 34 && (address) < 108 ? ((address) - 34 + 25300) : \
  ((address) >= 108 && (address) < 130 ? ((address) - 108 + 25800) : \
  ((address) >= 130 && (address) < 146 ? ((address) - 130 + 25000) : \
  -1 )))))

#define Cem9000_hold_INDEX(address) ( \
  ((address) >= 2000 && (address) < 2007 ? ((address) - 2000 + 33200) : \
  -1 )) 



/***********************************************END************************************************************************************/
/**********************************************台达寄存器地址映射 **********************************************************************/  

#define Taida_INPUT_INDEX(pcs_index, address) ( \
  ((address) >= 30185-30001 && (address) < 30211-30001 ? ((address) - (30185-30001) + 2600+0+300*(pcs_index-1)) : /*Module1 Battery Voltage～PCS Rated Capacity*/\
  ((address) >= 30478-30001 && (address) < 30490-30001 ? ((address) - (30478-30001) + 2600+219+300*(pcs_index-1)) : /*Module1 FPGA Fault Word1~ Module1 Comm Fault Word1   */\
  ((address) >= 30578-30001 && (address) < 30590-30001 ? ((address) - (30578-30001) + 2600+233+300*(pcs_index-1)) : /*Module2 FPGA Fault Word1 ~ Module2 Comm Fault Word1  */\
  ((address) >= 30701-30001 && (address) < 30765-30001 ? ((address) - (30701-30001) + 2600+26+300*(pcs_index-1)) : /*模块1时放电量-高位  ~ 模块2统计时间-时 */\
  ((address) >= 30765-30001 && (address) < 30821-30001 ? ((address) - (30765-30001) + 2600+90+300*(pcs_index-1)) : /*AC Hourly Disc Capacity High Byte  ~ AC Statistical Hour */\
  ((address) >= 30011-30001 && (address) < 30045-30001 ? ((address) - (30011-30001) + 2600+152+300*(pcs_index-1)) : /*版本号 */\
  (address) == 30404-30001 ? 217+2600+300*(pcs_index-1) : /* Controller Fault Word1  */\
  (address) == 30492-30001 ? 231+2600+300*(pcs_index-1) : /* Module1 Warning Word1 */\
  (address) == 30493-30001 ? 232+2600+300*(pcs_index-1) : /* Module1 Warning Word2  */\
  (address) == 30592-30001 ? 245+2600+300*(pcs_index-1) : /* Module2 Warning Word1 */\
  (address) == 30593-30001 ? 246+2600+300*(pcs_index-1) : /* Module2 Warning Word2  */\
  (address) == 40522-40001 ? 186+2600+300*(pcs_index-1) : /* certification  */\
  (address) == 30408-30001 ? 218+2600+300*(pcs_index-1) : -1 ))))))/*Controller Warning Word1  */\
  )
#define Master_Taida_INPUT_INDEX(pcs_index, address) ( \
  ((address) >= 30011-30001 && (address) < 30045-30001 ? ((address) - (30011-30001) + 17000+0+300*(pcs_index-1)) : /*master -Local Controller Version 1 ~ mJ2FPGA Version 3  */\
  ((address) >= 30100-30001 && (address) < 30122-30001 ? ((address) - (30100-30001)+ 17000+34+300*(pcs_index-1)) : /*master -Grid Volt RS ~ Battery Total Current  */\
  ((address) >= 30125-30001 && (address) < 30132-30001 ? ((address) - (30125-30001) + 17000+56+300*(pcs_index-1)) : /*Rocof df/dt  ~ System State Word3  */\
  ((address) >= 30136-30001 && (address) < 30143-30001 ? ((address) - (30136-30001) + 17000+64+300*(pcs_index-1)) : /*System Efficient Mode1  ~ MV Integrated Fault Word1  */\
  ((address) >= 30237-30001 && (address) < 30241-30001 ? ((address) - (30237-30001) + 2600+200+300*(pcs_index-1)) : /*Slave System Fault Word1 ~Slave System Fault Word4 */\
  ((address) >= 30243-30001 && (address) < 30250-30001 ? ((address) - (30243-30001) + 2600+210+300*(pcs_index-1)) : /*Slave System Fault Word1 ~Slave System Fault Word4 */\
  ((address) >= 30148-30001 && (address) < 30160-30001 ? ((address) - (30148-30001) + 17000+73+300*(pcs_index-1)) : /*MV Status Word1  ~ Relay Ground Trip OCP Value  */\
  ((address) >= 30045-30001 && (address) < 30052-30001 ? ((address) - (30045-30001) + 17000+85+300*(pcs_index-1)) : /*PCS Time Year   ~ PCS Time mSec  */\
  (address) == 30134-30001 ? 17000+63+300*(pcs_index-1) : /* System Operation Status  */\
  (address) == 30144-30001 ? 17000+71+300*(pcs_index-1) : /* System Warning Word1  */\
  (address) == 30145-30001 ? 17000+72+300*(pcs_index-1) : /* System Warning Word2  */ -1 ))))))))/*master- Controller Warning Word1 */\
  )
  
#define Taida_HOLD_INDEX(pcs_index, address) ( \
  ((address) >= 40201-40001 && (address) < 40225-40001 ? ((address) - (40201-40001) + 12000+0+300*(pcs_index-1)) : /*master&slave-Grid Discharge Power Calibration k ~POI Theta offset Calibration b*/\
  ((address) >= 40011-40001 && (address) < 40018-40001 ? ((address) - (40011-40001) + 12000+26+300*(pcs_index-1)) : /*master&slave-PCS Time Year ~ PCS Time mSec*/\
  ((address) >= 40241-40001 && (address) < 40243-40001 ? ((address) - (40241-40001) + 12000+24+300*(pcs_index-1)) : -1 ))))/*master&slave-Set PCS Master ~Slave1 Control Enable*/\


#define Master_Taida_HOLD_INDEX(pcs_index, address) ( \
  ((address) >= 40113-40001 && (address) < 40117-40001 ? ((address) - (40113-40001) + 27000+109+300*(pcs_index-1)) : /*group-Apparent Power Reference～Island Frequency Reference*/\
  ((address) >= 40229-40001 && (address) < 40235-40001 ? ((address) - (40229-40001) + 27000+113+300*(pcs_index-1)) : /*group-DisDutyPRef ～Discharge power limitation */\
  (address) == 40105-40001 ? 50+27000+300*(pcs_index-1) : /*Master Bat1 Discharge Power Coef*/\
  (address) == 40103-40001 ? 103+27000+300*(pcs_index-1) : /*System ON/OFF Cmd*/\
  (address) == 40106-40001 ? 46+27000+300*(pcs_index-1) : /*Master Bat2 Discharge Power Coef */\
  (address) == 40107-40001 ? 47+27000+300*(pcs_index-1) : /*Slave Bat1 Discharge Power Coef */\
  (address) == 40109-40001 ? 48+27000+300*(pcs_index-1) : /*Master Bat2 Charge Power Coef  */\
  (address) == 40110-40001 ? 49+27000+300*(pcs_index-1) : /*Slave Bat1 charge Power Coef  */\
  (address) == 40111-40001 ? 52+27000+300*(pcs_index-1) : /*有功 */\
  (address) == 40112-40001 ? 53+27000+300*(pcs_index-1) : /*无功  */\
  (address) == 40108-40001 ? 51+27000+300*(pcs_index-1) : -1 )))/*Master Bat1 Charge Power Coef*/\




/*反响映射 */
  #define Taida_HOLD_ADDRESS(index) \
  (index) == 27107 ? 40111-40001 : /*主机1下有功功率*/\
  (index) == 27108 ?40112-40001 :/*主机1下无功功率*/\
  (index) == 27407 ? 40111-40001 : /*主机2下有功功率*/\
  (index) == 27408 ?40112-40001 :-1 /*主机2下无功功率*/\



/***********************************************END************************************************************************************/

/**********************************************UPS寄存器地址映射 **********************************************************************/  
#define UPS_INPUT_INDEX(address) ( \
    ((address) == 208) ? 27113 : /* UPS Mode inquiry */ \
    ((address) == 879) ? 27114 : /* Unit Battery Capacity(AH) */ \
    ((address) == 675) ? 27115 : /* Fault kind */ \
    ((address) == 1010) ? 27116 : /* Battery Piece Number */ \
    ((address) == 1264) ? 27126 : /* Alarm word 1 */ \
    ((address) == 178) ? 27107 : /* P Battery voltage */ \
    ((address) == 0) ? 27129 : /* P Battery voltage */ \
    (((address) >= 1012) && ((address) < 1016)) ? ((address) - 1012 + 27117) : /*Input phase~Output rated VA*/\
    (((address) >= 1017) && ((address) < 1019)) ? ((address) - 1017 + 27121) : /*Input phase~Output rated VA*/\
    (((address) >= 169) && ((address) < 176)) ? ((address) - 169 + 27100) : /*Battery current~Battery remain time*/\
    (((address) >= 180) && ((address) < 183)) ? ((address) - 180 + 27108) : /*UPS Internal Temperature~Ups status*/\
    (((address) >= 191) && ((address) < 193)) ? ((address) - 191 + 27111) : /*Battery capacity~Battery remain time*/\
    (((address) >= 3) && ((address) < 6)) ? ((address) - 3 + 27130) :  /*warning0~5*/\
    (((address) >= 1162) && ((address) < 1166)) ? ((address) - 1162 + 27123) : /*Battery Voltage~Rating Output Frequency*/\
    -1 \
)


/***********************************************END************************************************************************************/









/***********************************************协能BMS寄存器地址映射********************************************************************/

// ================= HOLD BANK Index宏 ===================
//xieneng BMS实际设备地址映射对外地址
#define HOLD_B_INDEX(bank_index, address) \
((address) >= 1 && (address) < 9 ? ((address-1) + 35001+300*(bank_index-1)) : /*一键并网开关～时间-秒*/\
  (address) == 11 ? 35011+300*(bank_index-1) : /*复位*/\
  (address) == 12800 ? 35274+300*(bank_index-1) : /*设置rack起停*/\
  ((address) >= 256 && (address) < 274 ? ((address-256) + 35256+300*(bank_index-1)) : -1))/*绝缘采样开关～客户端3本地端口号   */\


  // ================= BANK Index宏 ===================
  //xieneng BMS实际设备地址映射对外地址
 #define INPUT_B_INDEX(bank_index, address) \
  (address) == 4097 ? 0+30000+100*(bank_index-1): /*一键并网开关状态*/\
  (address) == 4098 ? 1+30000+100*(bank_index-1) : /*Rack起停状态*/\
  ((address) >= 4101 && (address) < 4141 ? ((address) - 4101 + 2+30000+100*(bank_index-1)) : /*Rack在线状态~RACK起停状态*/\
  ((address) >= 16426 && (address) < 16435 ? ((address) - 16426 + 0+48000+50*(bank_index-1)) : /*水泵*/\
  ((address) >= 16437 && (address) < 16435 ? ((address) - 16469 + 0+49000+50*(bank_index-1)) : /*空调*/\
  (address) == 16558 ? 0+49000+20*(bank_index-1) : /*CO 浓度*/\
  (address) == 16560 ? 1+49000+20*(bank_index-1) : /*CO 故障和告警*/\
  (address) == 16562 ? 2+49000+20*(bank_index-1) : /*CO 监控器状态*/\
  (address) == 16566 ? 3+49000+20*(bank_index-1) : /*CO 可燃气体探测*/\
  (address) == 16567 ? 3+49400+20*(bank_index-1) : /*气表读数*/\
  ((address) >= 16569 && (address) < 16576 ? ((address) - 16569 + 1+49400+20*(bank_index-1)) : /*气体*/\
  (address) == 16577 ? 0+49800+20*(bank_index-1) : /*气表读数*/\
  ((address) >= 4144 && (address) < 4155 ? ((address) - 4144 + 43+30000+100*(bank_index-1)) : /*累积充电电量高位～Rack故障汇总*/\
  ((address) >= 16592 && (address) < 16594 ? ((address) - 16592 + 0+49900+20*(bank_index-1)) : /*TCP扩展IO*/\
  (address) == 4143 ? 42+30000+100*(bank_index-1) : -1 ))))))/*Rack在线状态2*/\

  #define INPUT_B_test_INDEX(bank_index, address) \
  (address) == 4097 ? 62+28000+200*(bank_index-1): /*一键并网开关状态*/\
  (address) == 4098 ? 64+28000+200*(bank_index-1) : /*Rack起停状态*/\
  (address) == 4102 ? 71+28000+200*(bank_index-1) : /*系统运行状态*/\
  (address) == 4103 ? 72+28000+200*(bank_index-1) : /*系统充放电状态*/\
  (address) == 4107 ? 40+28000+200*(bank_index-1) : /*系统一级报警汇总*/\
  (address) == 4106 ? 41+28000+200*(bank_index-1) : /*系统二级报警汇总*/\
  (address) == 4105 ? 42+28000+200*(bank_index-1) : /*系统三级报警汇总（预警）*/\
  (address) == 4104 ? 39+28000+200*(bank_index-1) : /*BAU报警状态*/\
  (address) == 16485 ? 51+28000+200*(bank_index-1) : /*Rack一级报警汇总2*/\
  ((address) >= 4110 && (address) < 4137 ? ((address) - 4110 + 3+28000+200*(bank_index-1)) : /*系统SOC~系统平均温度*/\
  (address) == 4108 ? 0+28000+200*(bank_index-1) : /*系统总电压*/\
  (address) == 4109 ? 2+28000+200*(bank_index-1) : /*系统总电流*/\
  (address) == 4153 ? 74+28000+200*(bank_index-1) : /*BMS心跳*/\
  ((address) >= 4151 && (address) < 4153 ? ((address) - 4151 + 37+28000+200*(bank_index-1)) : /*堆可用放电功率~堆可用充电功率*/\
  (address) == 4154 ? 50+28000+200*(bank_index-1) : -1))/*Rack 故障汇总*/\
  
 // ================= RACK Index宏 ===================
 #define INPUT_R_INDEX(bank_index,rack_index,address) \
  ((address) >= 8193 && (address) < 8226 ? ((address) - 8193)+31000+(rack_index-1)*100+(bank_index-1)*1500 : /*Rack运行状态～Rack高压箱温度2*/\
  ((address) >= 8236 && (address) < 8239 ? ((address) - 8236 + 33+31000+(rack_index-1)*100+(bank_index-1)*1500) : /*Rack告警*/\
  (address) == 12812 ? 36+31000+(rack_index-1)*100+(bank_index-1)*1500 : -1))/*均衡开关*/\

 // ================= CELL Index宏 ===================
 #define INPUT_C_INDEX(rack_index,address) \
 ((address) >= 8736 && (address) < 8864 ? ((address) - 8736)+(rack_index-1)*1152 : /*Rack正～负极柱温度*/\
  ((address) >= 9216 && (address) < 10240 ? ((address) - 9216 + 128)+(rack_index-1)*1152 : -1))/*Rack单体电池电压～温度*/\

/***********************************************END********************************************************************/
/******************************************************G2pro寄存器地址映射 ***********************************************/
  // BMS实际设备地址映射对外地址
 #define G2pro_INPUT_B_INDEX(bank_index, address) \
  ((address) >= 4097 && (address) < 4117 ? ((address) - 4097 + 39+28000+200*(bank_index-1)) : /*BMS系统故障汇总~协议版本号*/\
  ((address) >= 4136 && (address) < 4150 ? ((address) - 4136 + 62+28000+200*(bank_index-1)) : /*一键并网开关状态~分配ID状态*/\
  ((address) >= 4150 && (address) < 4175 ? ((address) - 4150 + 76+28000+200*(bank_index-1)) : /*rack1~24_cut_in_fail*/\
  ((address) >= 4214 && (address) < 4239 ? ((address) - 4214 + 100+28000+200*(bank_index-1)) : /*rack1~24_connect_fail*/\
  ((address) >= 4281 && (address) < 4286 ? ((address) - 4281 + 127+28000+200*(bank_index-1)) : /*对外复归告警汇总1~对外BAU一级报警（严重告警）*/\
  ((address) >= 4287 && (address) < 4294 ? ((address) - 4287 + 131+28000+200*(bank_index-1)) : /*RACK_新增严重告警汇总~放电簇间电流差大报警*/\
  ((address) >= 8193 && (address) < 8232 ? ((address) - 8193 + 0+28000+200*(bank_index-1)) : /*系统总电压~堆可用充电功率*/\
  ((address) >= 1536 && (address) < 1560 ? ((address) - 1536 + 0+64900+30*(bank_index-1)) : /*除湿机通讯故障~DI12_急停按钮*/\
  ((address) >= 1736 && (address) < 1759 ? ((address) - 1736 + 0+61000+50*(bank_index-1)) : /*液冷机_告警状态1~当前策略模式*/\
  ((address) >= 1790 && (address) < 1792 ? ((address) - 1790 + 0+64000+20*(bank_index-1)) : /*氢气*/\
 -1 ))))))))))\

 // ================= RACK Index宏 ===================
 #define G2pro_INPUT_R_INDEX(bank_index,rack_index,address) \
  ((address) >= 528 && (address) < 642 ? ((address) - 528)+31000+(rack_index-1)*200+(bank_index-1)*3000 : /*单体最高电压~模组电压过低停机时电压值*/\
  ((address) >= 16 && (address) < 56 ? ((address) - 16 + 114+31000+(rack_index-1)*200+(bank_index-1)*3000) : /*功能安全告警详细故障~电池系统一级报警信息（Warning）*/\
  -1))\

  // ================= CELL Index宏 ===================
 #define G2pro_INPUT_C_INDEX(rack_index,address) \
 ((address) >= 5121 && (address) < 5633 ? ((address) - 5121)+128+(rack_index-1)*2000 : /*单体电压*/\
 ((address) >= 6657 && (address) < 6722 ? ((address) - 6657)+1152+(rack_index-1)*2000 : /*铜牌温度*/\
 ((address) >= 8769 && (address) < 8773 ? ((address) - 8769)+0+(rack_index-1)*2000 : /*PACK正极柱温度*/\
 ((address) >= 8897 && (address) < 8901 ? ((address) - 8897)+64+(rack_index-1)*2000 : /*PACK负极柱温度*/\
  ((address) >= 6145 && (address) < 6657 ? ((address) - 6145 + 640)+(rack_index-1)*2000 : -1)))))/*电池温度*/\
 
  // ================= HOLD BANK Index宏 ===================
//xieneng BMS实际设备地址映射对外地址
#define G2pro_HOLD_B_INDEX(bank_index, address) \
((address) >= 12289 && (address) < 12317 ? ((address-12289) + 35001+300*(bank_index-1)) : /*一键并网开关～时间-设置Rack启用/停用*/\
((address) >= 21037 && (address) < 21045 ? ((address-21037) + 35032+300*(bank_index-1)) : /*NTP时区～BAU-EMS 通讯故障（心跳）*/\
((address) >= 12357 && (address) < 12360 ? ((address-12357) + 35029+300*(bank_index-1)) : -1)))/*本地/远程控制切换～一键复归   */\

/************************************************************************************************** */
 #define GET_INPUT(address) \
 ({ \
     (address >= 0 && address < 100000) ? \
     RegM.Input[address] : 0; \
 })
 
 #define SET_INPUT(address, val) \
 ({ \
     if (address >= 0 && address < 100000) \
      RegM.Input[address] = (val); \
 })

 #define GET_C_INPUT(bank_index,address) \
 ({ \
     (address >= 0 && address < R_INPUT_SIZE+(RACK_SIZE-1)*1152 && bank_index >= 0 && bank_index < BANK_SIZE) ? \
     RankM.Input[bank_index][address] : 0; \
 })
 
 #define SET_C_INPUT(bank_index,address, val) \
 ({ \
     if (address >= 0 && address < R_INPUT_SIZE+(RACK_SIZE-1)*1152 && bank_index >= 0 && bank_index < BANK_SIZE) \
         RankM.Input[bank_index][address] = (val); \
 })

 extern pRegistersModel const pRegM;
 extern pRankModel const pRankM;





 #endif //ADDRESS_MAP_H
 
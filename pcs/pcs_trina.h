#ifndef PCS_TRINA_H
#define PCS_TRINA_H
void PCS_Trina_Task(const char *arg);

#define PCS_Trina_SLAVE_ADDR   (1)
#define PCS_BUFF_LEN        (300)

#define PCS_Trina_04addr1      (4000)  
#define PCS_Trina_04size1      (12)    //硬件版本～模块2FPGA版本

#define PCS_Trina_04addr2      (4023)  
#define PCS_Trina_04size2      (4038-4023+1)    //DSP故障表1～FPGA告警表4

#define PCS_Trina_04addr3      (4050)  
#define PCS_Trina_04size3      (4062-4050+1)    //ARM与控制DSP间的通讯故障1～升级状态

#define PCS_Trina_04addr7      (4064)  
#define PCS_Trina_04size7      (4087-4064+1)    //累积电量

#define PCS_Trina_04addr4      (4100)  
#define PCS_Trina_04size4      (4148-4100+1)    //系统状态~ 电池功率（J2）遥测信息

#define PCS_Trina_04addr5      (4502)  
#define PCS_Trina_04size5      (4527-4502+1)    //J-DSP版本号~ 模块2DSP版本

#define PCS_Trina_04addr6      (4550)  
#define PCS_Trina_04size6      (4569-4550+1)    //电感电流R～ ISO2Rn

#define PCS_Trina_04addr8      (4580)  
#define PCS_Trina_04size8      (4625-4580+1)    //IGBT温度


#define PCS_Trina_03addr1      (3000)  
#define PCS_Trina_03size1      (37)    //并联模式～支路充电分配系数

#define PCS_Trina_03addr2      (3077)  
#define PCS_Trina_03size2      (2)    //升级信号～工厂复位

#define PCS_Trina_03addr3      (3086)  
#define PCS_Trina_03size3      (4)    //IP地址～子网掩码

#define PCS_Trina_03addr4      (3094)  
#define PCS_Trina_03size4      (6)    //年月日设置

#define PCS_Trina_03addr5      (3100)  
#define PCS_Trina_03size5      (3224-3100+1)    //安规参数

#define PCS_Trina_03addr6      (3225)  
#define PCS_Trina_03size6      (3349-3225+1)    //安规参数


#define PCS_Trina_03addr7      (3350)  
#define PCS_Trina_03size7      (3410-3350+1)    //(3409-3350+1) 

#define PCS_Trina_03addr8      (3800)  
#define PCS_Trina_03size8     (3821-3800+1)    //

#define PCS_Trina_03addr9      (3900)  
#define PCS_Trina_03size9     (3971-3900+1)    //交流电感瞬时数值过流～ BUS2绝缘阻抗故障时间

#define Power_on_addr      (3001)
#define control_mode_addr      (3003)
#define Activate_power_addr      (3004)

#endif


#ifndef PCS_TAIDA_H
#define PCS_TAIDA_H
void PCS_Taida_Task(const char *arg);


#define PCS_Taida_SLAVE_ADDR   (1)
#define PCS_BUFF_LEN        (300)

#define PCS_Master_Taida_addr1      (30014-30001)  
#define PCS_Master_Taida_size1      (2)    //Local Controller Version 1 ~J2FPGA Version 3 


#define PCS_Master_Taida_addr2      (30100-30001)  
#define PCS_Master_Taida_size2      (30121-30100+1)    //Grid Volt RS~Relay ~Battery Total Current

#define PCS_Master_Taida_addr3      (30125-30001)  
#define PCS_Master_Taida_size3      (30131-30125+1)    //Rocof df/dt ~System State Word3

#define PCS_Master_Taida_addr4      (30136-30001)  
#define PCS_Master_Taida_size4      (30142-30136+1)    //System Efficient Mode1~ MV Integrated Fault Word1

#define PCS_Master_Taida_addr5      (30148-30001)  
#define PCS_Master_Taida_size5      (30159-30148+1)    //MV Status Word1~ Relay Ground Trip OCP Value

#define PCS_Master_Taida_addr6      (30134-30001)  
#define PCS_Master_Taida_size6      (1)    //System Operation Status 

#define PCS_Master_Taida_addr7      (30144-30001)  
#define PCS_Master_Taida_size7      (2)    //System Warning Word1 ~System Warning Word2 

#define PCS_Master_Taida_addr8      (30237-30001) 
#define PCS_Master_Taida_size8      (30249-30237+1)    //Slave System Fault Word1~Slave Controller Warning Word1 

#define PCS_Master_Taida_addr9     (30045-30001) 
#define PCS_Master_Taida_size9      (30051-30045+1)    //PCS Time Year~PCS Time mSec 

#define Each_PCS_Taida_addr1     (30185-30001)  
#define Each_PCS_Taida_size1     (30210-30185+1)   //Module1 Battery Voltage ~PCS Rated Capacity





#define Each_PCS_Taida_addr2     (30478-30001)  
#define Each_PCS_Taida_size2     (30489-30478+1)   //Module1 FPGA Fault Word1 ~Module1 Comm Fault Word1

#define Each_PCS_Taida_addr3      (30578-30001) 
#define Each_PCS_Taida_size3     (30589-30578+1)    //Module2 FPGA Fault Word1~Module2 Comm Fault Word1 


#define Each_PCS_Taida_addr4     (30404-30001) //Controller Fault Word1 
#define Each_PCS_Taida_size4      (1)  

#define Each_PCS_Taida_addr5     (30408-30001) //Controller Warning Word1 
#define Each_PCS_Taida_size5      (1)  

#define Each_PCS_Taida_addr6     (30492-30001) //Module1 Warning Word1 
#define Each_PCS_Taida_size6      (2)  

#define Each_PCS_Taida_addr7     (30592-30001) //Module2 Warning Word1 
#define Each_PCS_Taida_size7      (2) 

#define Each_PCS_Taida_addr8     (30701-30001) //模块1时放电量-高位 ~模块2统计时间-时
#define Each_PCS_Taida_size8      (30764-30701+1) 

#define Each_PCS_Taida_addr9     (30765-30001) //AC Hourly Disc Capacity High Byte ~AC Statistical Hour 
#define Each_PCS_Taida_size9      (30820-30765+1)

#define Each_PCS_Taida_addr10     (30011-30001) //
#define Each_PCS_Taida_size10      (30044-30011+1) //PCS软件版本号

#define Each_PCS_Taida_addr11     (30129-30001) //
#define Each_PCS_Taida_size11      (2)

// #define Each_PCS_Taida_addr12     (30027-30001) //
// #define Each_PCS_Taida_size12      (2)

// #define Each_PCS_Taida_addr13     (30032-30001) //
// #define Each_PCS_Taida_size13      (2)

// #define Each_PCS_Taida_addr14     (30040-30001) //
// #define Each_PCS_Taida_size14      (2)

#define PCS_Master_Taida_addr16      (40100-40001) 
#define PCS_Master_Taida_size16      (40102-40100+1) //参数设置项，只和主机有关，从机不用读取，也不可设置。System Control Word1 ～Island Frequency Reference


#define PCS_Master_Taida_addr13      (40105-40001) 
#define PCS_Master_Taida_size13     (40116-40105+1) //参数设置项，只和主机有关，从机不用读取，也不可设置。System Control Word1 ～Island Frequency Reference

#define PCS_Master_Taida_addr14      (40103-40001) 
#define PCS_Master_Taida_size14     (1) //参数设置项，只和主机有关，从机不用读取，也不可设置。System Control Word1 ～Island Frequency Reference

#define PCS_Master_Taida_addr15      (40104-40001) 
#define PCS_Master_Taida_size15     (1) //参数设置项，只和主机有关，从机不用读取，也不可设置。System Control Word1 ～Island Frequency Reference

#define PCS_Taida_addr10      (40201-40001) 
#define PCS_Taida_size10      (40224-40201+1)  //主机从机都有关系，都需获取，Grid Discharge Power Calibration k ～POI Theta offset Calibration b 

#define PCS_Master_Taida_addr11     (40229-40001) 
#define PCS_Master_Taida_size11      (40234-40229+1) //只和主机有关，从机不用读取，也不可设置。DisDutyPRef ～Discharge power limitation 

#define PCS_Taida_addr12      (40241-40001) //Set PCS Master ～Slave1 Control Enable 
#define PCS_Taida_size12      (2)  //主机从机都有关系，都需获取

#define PCS_Taida_addr13      (40011-40001) //PCS Time Year Set ～PCS Time mSec Set 
#define PCS_Taida_size13      (7)  //主机从机都有关系，都需获取

#define PCS_Taida_addr14      (40522-40001) //PCS certification 
#define PCS_Taida_size14      (1)  //主机从机都有关系，都需获取

#define PCS_Taida_ACPower_addr   (40111-40001)
#define PCS_Taida_REPower_addr   (40112-40001)
#define PCS_PE_OPRATE_addr    (38)
#define PCS_Taida_OPRATE_addr    (40100-40001)//Taida控制字1
#define PCS_Taida_OPRATE_addr2  (40101-40001) //Taida控制字2
#define PCS_Taida_OPRATE_addr3  (40102-40001) //Taida控制字3
#define PCS_Taida_start_addr    (40103-40001)//TaidaPCS开关


#define Master1_Taida_System_State3      (17062) 
#define Master2_Taida_System_State3      (17362) 

#define PCS_Taida_HotStandbyEn      (40231-40001) 
#define Taida_addresslimit     41000

#define PCS_MAX_NUM        4
#define PCS_INPUT_BASE1    2600
#define PCS_INPUT_BASE2    17000
#define PCS_HOLD_BASE      12000
#define PCS_ADDR_STEP      300
// #define sys_status      1003
void Update_Pcs_Diff_Status(uint8_t pcs_total_num);
#endif
#ifndef CEM9000_H
#define CEM9000_H
void Cem9000_Task(const char *arg);


#define Cem9000_SLAVE_ADDR        (1)
#define Rec_BUFF_LEN        (300)

#define Cem9000_Addr1      (0)  
#define cem9000size1      (100)    //cem9000遥测数据点

#define Cem9000_Addr4      (100)  
#define cem9000size4      (45)    //cem9000遥测数据点

#define Cem9000_Addr2      (1000)  
#define cem9000size2      (8)    //cem9000遥信数据点

#define Cem9000_Addr3      (2000)  
#define cem9000size3      (7)    //cem9000遥控数据点

#define high_vol_switch_addr 2001 //远方分闸地址




#endif
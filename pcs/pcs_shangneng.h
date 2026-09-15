#ifndef PCS_SHANGNENG_H
#define PCS_SHANGNENG_H

#include "stdint.h"
void PCS_ShangNeng_Task(void *arg);



#define PCS_ShangNeng_SLAVE_ADDR        (1)
#define PCS_BUFF_LEN        (300)   //pcs收发缓存长度
#define PCS_addr1      (5000)  //整机运行信息 5000~5007
#define PCS_size1      (8)    //


#define PCS_addr2      (5013)  
#define PCS_size2      (3)    

#define PCS_addr3      (5023)  
#define PCS_size3      (1)   

#define PCS_addr4      (5026) 
#define PCS_size4      (5)    

#define PCS_addr5     (5032) 
#define PCS_size5      (1)  

#define PCS_addr6      (5034) 
#define PCS_size6      (1)  


#define PCS_addr7      (5036) 
#define PCS_size7      (1)  

#define PCS_addr8      (5044) 
#define PCS_size8      (16)  

#define PCS_addr9      (5250) 
#define PCS_size9      (43)  


#endif // PCS_SHANGNENG_H
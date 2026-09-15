#ifndef PCS_PE_H
#define PCS_PE_H
void PCS_PE_Task(const char *arg);


#define PCS_PE_SLAVE_ADDR        (2)
#define PCS_BUFF_LEN        (300)

#define PCS_PEaddr1      (1372)  
#define PCS_PEsize1      (4)    //


#define PCS_PEaddr2      (1384)  
#define PCS_PEsize2      (4)    

#define PCS_PEaddr3      (1376)  
#define PCS_PEsize3      (4)   

#define PCS_PEaddr4      (4300) //中压
#define PCS_PEsize4      (10)    

#define PCS_PEaddr5     (4320) //低压
#define PCS_PEsize5      (10)  

#define PCS_PEaddr6      (538) //所选模块信息
#define PCS_PEsize6      (25)  


#define PCS_PEaddr7      (1003)//当前状态
#define PCS_PEsize7      (1)  

#define PCS_PEaddr8      (4331) //电网频率值
#define PCS_PEsize8      (2)  

#define PCS_PEaddr9      (572) //运行时间
#define PCS_PEsize9      (4) 

#define PCS_PEaddr10      (6513) 
#define PCS_PEsize10      (1)  

#define PCS_PEaddr11     (6548) 
#define PCS_PEsize11      (1) 

#define PCS_PEaddr12      (6583) 
#define PCS_PEsize12      (1)  

#define PCS_PEaddr13     (6618) 
#define PCS_PEsize13     (1) 


// #define PCS_PEaddr14     (6532) 
// #define PCS_PEsize14      (1) 

// #define PCS_PEaddr15      (6567) 
// #define PCS_PEsize15     (1)  

// #define PCS_PEaddr16     (6611) 
// #define PCS_PEsize16     (1) 


#define PCS_PEaddr14     (6637) 
#define PCS_PEsize14      (1) 

#define PCS_PEaddr15      (1104) 
#define PCS_PEsize15     (1)  

#define PCS_PEaddr16     (281) 
#define PCS_PEsize16     (1) 


#define PCS_PEaddr17     (281) 
#define PCS_PEsize17     (1) 



#define PCS_PEaddr18     (5) //对时参数，先获取的是秒
#define PCS_PEsize18     (6) 



#define PCS_PEaddr19     (1512) //告警
#define PCS_PEsize19     (32) 

#define PCS_PEaddr20     (38) 
#define PCS_PEsize20     (1) 


#define PCS_PEaddr21     (262) 
#define PCS_PEsize21     (2) 

#define PCS_PEaddr22     (41) //故障复归
#define PCS_PEsize22     (1) 


#define PCS_PE_MAX_POWER    (20000)

#define PCS_PE_ACPower_addr   (262)
#define PCS_PE_OPRATE_addr    (38)

#define addresslimit     8000

#define sys_status      1003
#endif
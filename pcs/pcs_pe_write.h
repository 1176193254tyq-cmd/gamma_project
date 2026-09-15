#ifndef PCS_PE_WRITE_H
#define PCS_PE_WRITE_H

#define PCS_BUFF_LEN        (300)   //PCS收发缓存长度


#define PCS_PE_write_addr1     (5) //对时参数，先获取的是秒
#define PCS_PE_write_size1     (6) 

#define PCS_PE_write_addr2     (38) 
#define PCS_PE_write_size2     (1) 


#define PCS_PE_write_addr3    (262) 
#define PCS_PE_write_size3     (2) 

#define PCS_PE_write_addr4     (41) //故障复归
#define PCS_PE_write_size4     (1) 

#define PCS_PE_write_addr5     (281) //母线阀值
#define PCS_PE_write_size5     (1) 
void  PCS_PE_write_Task(const char *_num);
#endif
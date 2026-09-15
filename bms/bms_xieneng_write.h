#ifndef BMS_XIENENG_WRITE_H
#define BMS_XIENENG_WRITE_H

void  BMS_XieNeng_write_Task(const char *_num);



 //Bank读取保持寄存器
 #define Bank_block_hold_addr0     (0x001)   //一键并网开关～时间-秒
 #define Bank_block_hold_size0     (8)
 
 #define Bank_block_hold_addr1     (0x000B)  //复位
 #define Bank_block_hold_size1     (1)
 
 #define Bank_block_hold_addr2     (0x0100)  //绝缘采样功能开关～客户端3本地端口号
 #define Bank_block_hold_size2     (18)

 #define Bank_block_hold_addr3     (0x3200)  //设置rack1启用/停止
 #define Bank_block_hold_size3     (1)

#endif


 #ifndef BMS_G2PRO_H
 #define BMS_G2PRO_H
#include "system.h"
 
 /**
  * @brief 协能BMS处理线程
  * @param[in] _item 线程入参
  */
void  BMS_G2pro_Task(const char *_num);



 /***************************************BMS通信定义************************************/
 #define Rack_Add_Min        (2)		//簇最小地址
 #define Rack_Add_Max        (16)	//簇最大地址
 
 
 
 
 //RACK读取
 /*Rack Signal*/
 #define G2PRORack_block_addr0     (0x0010)   //功能安全告警详细故障～电池系统一级报警信息（Warning）
 #define G2PRORack_block_size0     (0x0037-0x0010+1)
/********************************* */

/*Rack Detail*/
 #define G2PRORack_block_addr1     (0x2241)  //PACK1正极柱温度～PACK4
 #define G2PRORack_block_size1     (4)
 #define G2PRORack_block_addr2     (0x22C1)  //PACK1负极柱温度～PACK4
 #define G2PRORack_block_size2     (4)

 #define G2PRORack_block_addr3     (0x1401)  //0x1401，RACK单体电池电压，最大支持512
 #define G2PRORack_block_size3     (104)
 #define G2PRORack_block_addr4     (0x1401+104)  //
 #define G2PRORack_block_size4     (104)
 #define G2PRORack_block_addr5     (0x1401+(2*104))  //
 #define G2PRORack_block_size5     (104)
 #define G2PRORack_block_addr6     (0x1401+(3*104))  //
 #define G2PRORack_block_size6     (104)
 #define G2PRORack_block_addr7     (0x1401+(4*104))  //
 #define G2PRORack_block_size7     (96)

 #define G2PRORack_block_addr8     (0x1801)  //0x1801，RACK单体电池温度，最大支持512
 #define G2PRORack_block_size8     (104)
 #define G2PRORack_block_addr9     (0x1801+104)  //
 #define G2PRORack_block_size9     (104)
 #define G2PRORack_block_addr10    (0x1801+(2*104))  //
 #define G2PRORack_block_size10    (104)
 #define G2PRORack_block_addr11    (0x1801+(3*104))  //
 #define G2PRORack_block_size11    (104)
 #define G2PRORack_block_addr12    (0x1801+(4*104))  //
 #define G2PRORack_block_size12    (96)

 #define G2PRORack_block_addr13    (0x1A01)  //铜牌温度
 #define G2PRORack_block_size13    (64)
/*Rack Measure*/

 #define G2PRORack_block_addr14    (0x0210)  //单体最高电压~总累计放电容量低16位
 #define G2PRORack_block_size14    (0x024F-0x0210+1)

 #define G2PRORack_block_addr15    (0x0250)  //报警时最高电压电池编号~模组电压过低停机时电压值
 #define G2PRORack_block_size15    (0x0281-0x0250+1)

/********************* */
/*Rack Control*/

 #define G2PRORack_block_addr16    (0x0401)  //启动绝缘采样~风扇强控开关
 #define G2PRORack_block_size16    (0x0406-0x0401+1)

/**************** */

/*RACK Alarm Parameter Threshold Set & Read*/

 #define G2PRORack_block_addr17    (0x6001)  //Rack单体过压一级报警门限（严重告警）~Rack放电过温三级恢复门限（预警）
 #define G2PRORack_block_size17    (0x6060-0x6001+1)

 #define G2PRORack_block_addr18    (0x6061)  //Rack放电欠温三级报警门限（预警）~预充结束压差
 #define G2PRORack_block_size18    (0x6083-0x6061+1)

 #define G2PRORack_block_addr19    (0x6092)  //总电压过高截止事件恢复阈值~模组温差过高三级告警触发阈值
 #define G2PRORack_block_size19    (0x60A5-0x6092+1)
/**************** */



 //Bank读取

 /*Bank Signal*/
 #define G2PROBank_block_addr0     (0x1001)   //BMS系统故障汇总～协议版本号
 #define G2PROBank_block_size0     (0x1014-0x1001+1)
 

 #define G2PROBank_block_addr1     (0x1028)  //一键并网开关状态~分配ID状态
 #define G2PROBank_block_size1     (0x1035-0x1028+1)

 #define G2PROBank_block_addr2     (0x1036)  //Rack切入失败原因1~24
 #define G2PROBank_block_size2     (24)

 #define G2PROBank_block_addr3     (0x1076)  //Rack未启动切入原因1~24
 #define G2PROBank_block_size3     (24)

 #define G2PROBank_block_addr4     (0x10B9)  //对外复归告警汇总1~对外BAU一级报警（严重告警）
 #define G2PROBank_block_size4     (4)

 #define G2PROBank_block_addr5     (0x10BF)  //RACK_新增严重告警汇总~放电簇间电流差大报警
 #define G2PROBank_block_size5     (0x10C3-0x10BF+1)

/************************************************ */
 
/*Bank Measure*/

 #define G2PROBank_block_addr6    (0x2001)  //系统总电压~堆可用充电功率
 #define G2PROBank_block_size6     (0x2027-0x2001+1)

/********************** */

/*Bank Control*/

 #define G2PROBank_block_addr7    (0x3001)  //一键并网开关~一键复归
 #define G2PROBank_block_size7     (0x3047-0x3001+1)

/********************** */
/*Bank Parameter Set&Read */

 #define G2PROBank_block_addr8    (0x5005)  //Bank簇间充电流差过高三级触发阈值~放电簇间电流差过高一级恢复阈值
 #define G2PROBank_block_size8     (0x5018-0x5005+1)

 #define G2PROBank_block_addr9    (0x522D)  //NTP时区~秒
 #define G2PROBank_block_size9     (0x5233-0x522D+1)
 /********************* */

 /*ENV*/
 #define G2PROBank_block_addr10    (0x600)  //除湿机通讯故障～reserverEnv0001
 #define G2PROBank_block_size10   (0x617-0x600+1)

//  #define G2PROBank_block_addr11    (0x637)  
//  #define G2PROBank_block_size11    (0x664 - 0x637+1) 



//  #define G2PROBank_block_addr12    (0x684)  
//  #define G2PROBank_block_size12    (0x6A8 - 0x684+1)

#define G2PROBank_block_addr13    (0x6C8)  
#define G2PROBank_block_size13    (0x6DE - 0x6C8+1)

#define G2PROBank_block_addr14    (0x6FE)  
#define G2PROBank_block_size14    (2)

#define G2PROBank_block_addr15    (0x700)  
#define G2PROBank_block_size15    (1)

// #define G2PROBank_block_addr16    (0x720)  
// #define G2PROBank_block_size16    (0x72F-0x720+1)


// #define G2PROBank_block_addr17    (0x66A)  //除湿器1除湿设定点 ~除湿器1除湿回差 
// #define G2PROBank_block_size17    (0x66B-0x66A+1)

// #define G2PROBank_block_addr18    (0x66C)  //除湿器2环境温度值 ~除湿器2风机2工作时长低位 
// #define G2PROBank_block_size18    (0x678-0x66C+1)

// #define G2PROBank_block_addr19    (0x67A)  //除湿器2除湿设定点 ~除湿器2除湿回差 
// #define G2PROBank_block_size19    (0x67B-0x67A+1)

// #define G2PROBank_block_addr20    (0x67C)  //除湿器3环境温度值 ~除湿器3风机2工作时长低位 
// #define G2PROBank_block_size20    (0x688-0x67C+1)

// #define G2PROBank_block_addr21    (0x68A)  //除湿器3除湿设定点 ~除湿器3除湿回差 
// #define G2PROBank_block_size21    (0x68B-0x68A+1)

// #define G2PROBank_block_addr22    (0x68C)  //氢气_高位 ~除湿器3除湿模块工作时长_低位 
// #define G2PROBank_block_size22    (0x6A3-0x68C+1)

 #define G2PROBank_block_addr23    (0x1031)   //system_run_mode
 #define G2PROBank_block_size23     (1)

/************************ */



 
 #define Rack_Block_Addr(x)   Rack_block_addr##x
 #define Rack_Block_Size(x)   Rack_block_size##x
 

 
 #define Rack_Set_Addr(x)     Rack_set_addr##x
 #define Rack_Set_Size(x)     Rack_set_size##x
 
 
  //设置
#define BMS_G2proRELAY_CTR_addr  (0x3001)    //接触器控制 
#define BMS_XIENENG_RELAY_OPEN      (1)         //打开
#define BMS_XIENENG_RELAY_CLOSE     (2)         //关闭

#define BMS_G2pro_Heartbit_addr  (0x5234)    //心跳；

#define BMS_G2pro_RELAY_RESET_addr  (0x3047)    //1：复位
 
 
 #define RACK_TEMP_NUM             (Rack_Block_Addr(9)+Rack_Block_Addr(11)+Rack_Block_Addr(10)+Rack_Block_Addr(12)+Rack_Block_Addr(13))   //电芯温度值数量
 #define RACK_VOL_NUM              (Rack_Block_Addr(4)+Rack_Block_Addr(5)+Rack_Block_Addr(6)+Rack_Block_Addr(7)+Rack_Block_Addr(8))    //电芯电压值数量
 
 //读写定义
 #define Rack_XIE_CMD_REPEAT      (30)    //命令尝试次数
 #define Rack_XIE_BUFF_LEN        (300)   //BMS收发缓存长度
 #define CMD_DELAY_50               (50 * 1000) //命令下发后延迟读取
  #define CMD_DELAY_15              (15 * 1000) //命令下发后延迟读取
  #define CMD_DELAY_10               (10 * 1000) //命令下发后延迟读取
//  extern INT8U volatile Bms_Contactor_Lock[MAX_BMS_NUM] = {0};

// //ini中获取的片数据的大小
// extern int Ini_Rack_Blank_Size[MAX_BMS_NUM][14] = {0};
 #endif // BMS_XIENENG

/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : bms_xieneng.h
 * Project : 用于阿拉善60MWh项目、天门通威50MWh项目、茶陵200MWh项目
 * Description : 协能BMS操作
 * File Created : 2022/10/15
 * Author : XuBo
 * ------------------------------------------------------------
 * Last Modified : 2022/12/14
 * Modified By : jia.liu
 * ------------------------------------------------------------
 * HISTORY : 2022/12/14 修改函数声明
 * ------------------------------------------------------------
 * HISTORY : 2022/11/19 修改宏定义、优化代码结构
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改文件名和宏定义
 * ------------------------------------------------------------
 * HISTORY : 2022/10/15 新规
 */

 #ifndef BMS_XIENENG_H
 #define BMS_XIENENG_H
#include "system.h"
 
 /**
  * @brief 协能BMS处理线程
  * @param[in] _item 线程入参
  */
 void BMS_XieNeng_Task(const char *_num);
 void bms_data_init();


 /***************************************BMS通信定义************************************/
 #define Rack_Add_Min        (2)		//簇最小地址
 #define Rack_Add_Max        (16)	//簇最大地址
 
 
 
 
 //簇读取
 #define Rack_block_addr0     (0x2001)   //Rack运行状态～Rack高压箱温度2
 #define Rack_block_size0     (0x2021-0x2001+1)

 #define Rack_block_addr1     (0x2220)  //PACK1正极柱温度～PACK64
 #define Rack_block_size1     (64)
 #define Rack_block_addr2     (0x2260)  //PACK1负极柱温度～PACK64
 #define Rack_block_size2     (64+2)
 #define Rack_block_addr3     (9216)  //0x2400，RACK单体电池电压，最大支持512
 #define Rack_block_size3     (104)
 #define Rack_block_addr4     (9216+104)  //
 #define Rack_block_size4     (104)
 #define Rack_block_addr5     (9216+(2*104))  //
 #define Rack_block_size5     (104)
 #define Rack_block_addr6     (9216+(3*104))  //
 #define Rack_block_size6     (104)
 #define Rack_block_addr7     (9216+(4*104))  //
 #define Rack_block_size7     (96)
 #define Rack_block_addr8     (9728)  //0x2600，RACK单体电池温度，最大支持512
 #define Rack_block_size8     (104)
 #define Rack_block_addr9     (9728+104)  //
 #define Rack_block_size9     (104)
 #define Rack_block_addr10    (9728+(2*104))  //
 #define Rack_block_size10    (104)
 #define Rack_block_addr11    (9728+(3*104))  //
 #define Rack_block_size11    (104)
 #define Rack_block_addr12    (9728+(4*104))  //
 #define Rack_block_size12    (96)

 #define Rack_block_addr13    (0x202C)  //Rack报警2
 #define Rack_block_size13    (3)
 #define Rack_block_addr14    (0x320C)  //均衡开关
 #define Rack_block_size14    (1)
 //Bank读取

  #define Bank_block_hold_addr0     (0x001)   //一键并网开关～时间-秒
 #define Bank_block_hold_size0     (8)
 
 #define Bank_block_hold_addr1     (0x000B)  //复位
 #define Bank_block_hold_size1     (1)
 
 #define Bank_block_hold_addr2     (0x0100)  //绝缘采样功能开关～客户端3本地端口号
 #define Bank_block_hold_size2     (18)

 #define Bank_block_hold_addr3     (0x3200)  //设置rack1启用/停止
 #define Bank_block_hold_size3     (1)


 #define Bank_block_addr0     (0x1001)   //一键并网开关状态～Rack起停状态
 #define Bank_block_size0     (2)
 
 #define Bank_block_addr1     (0x1006)  //Rack在线状态~主控通信故障报警2
 #define Bank_block_size1     (0x102A-0x1006+1)
 
 #define Bank_block_addr2     (0X103D)  //主控通信故障报警3
 #define Bank_block_size2     (1)


 
 #define Bank_block_addr3    (0x1037)  //最大允许放电功率~Rack 故障汇总
 #define Bank_block_size3     (4)
 
//  #define Bank_block_addr4    (0x4090)  //除湿机1控温开启值
//  #define Bank_block_size4    (0x409F-0x4090+1)


//  #define Bank_block_addr5    (0x40A1)  //除湿机2环境温度
//  #define Bank_block_size5     (0x40A9-0x40A1+1)


//  #define Bank_block_addr6    (0x40AB)  //除湿机3环境温度
//  #define Bank_block_size6     (0x40B0-0x40AB+1)


//  #define Bank_block_addr7    (0x40B2)  //监控器监控状态中
//  #define Bank_block_size7     (1)

//  #define Bank_block_addr8   (0x40B6)  //可燃气体探测通讯故障
//  #define Bank_block_size8     (0x40C0-0x40B6+1)

//  #define Bank_block_addr9   (0x40D0)  //TCP扩展IO状态
//  #define Bank_block_size9     (2)

//  #define Bank_block_addr10   (0x102F)  //
//  #define Bank_block_size10     (0x103A-0x102F+1)

 
 #define Rack_Block_Addr(x)   Rack_block_addr##x
 #define Rack_Block_Size(x)   Rack_block_size##x
 
 #define Bank_Block_Addr(x)   Bank_block_addr##x
 #define Bank_Block_Size(x)   Bank_block_size##x
 
 #define Rack_Set_Addr(x)     Rack_set_addr##x
 #define Rack_Set_Size(x)     Rack_set_size##x
 
 
  //设置
#define BMS_XIENENG_RELAY_CTR_addr  (0x0001)    //接触器控制 1-全部断开；2-全部吸合；
#define BMS_XIENENG_RELAY_OPEN      (1)         //打开
#define BMS_XIENENG_RELAY_CLOSE     (2)         //关闭

#define BMS_XIENENG_Heartbit_addr  (0x0002)    //心跳；

#define BMS_XIENENG_RELAY_RESET_addr  (0x000B)    //1：复位
 
 
 #define RACK_TEMP_NUM             (Rack_Block_Addr(9)+Rack_Block_Addr(11)+Rack_Block_Addr(10)+Rack_Block_Addr(12)+Rack_Block_Addr(13))   //电芯温度值数量
 #define RACK_VOL_NUM              (Rack_Block_Addr(4)+Rack_Block_Addr(5)+Rack_Block_Addr(6)+Rack_Block_Addr(7)+Rack_Block_Addr(8))    //电芯电压值数量
 
 //读写定义
 #define Rack_XIE_CMD_REPEAT      (30)    //命令尝试次数
 #define Rack_XIE_BUFF_LEN        (300)   //BMS收发缓存长度
 #define CMD_DELAY_50               (50 * 1000) //命令下发后延迟读取
//  extern INT8U volatile Bms_Contactor_Lock[MAX_BMS_NUM] = {0};

// //ini中获取的片数据的大小
// extern int Ini_Rack_Blank_Size[MAX_BMS_NUM][14] = {0};
 #endif // BMS_XIENENG

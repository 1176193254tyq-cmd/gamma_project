/* ------------------------------------------------------------
* Copyright (C) 2026
* File Name : address_map.c
* Project :
* Description : 设备地址映射
* File Created :tyq

* ------------------------------------------------------------

* ------------------------------------------------------------
* HISTORY : 
*/
#include "main.h"
#include "timed_check.h"

volatile RegistersModel RegM;
volatile RankModel RankM;
pRegistersModel const pRegM = &RegM;
pRankModel const pRankM = &RankM;

/* *****************************************trina写寄存器反向映射**************************************/
int Trina_Hold_Address(int pcs_index, int index)
{
    int base = 27000 + 300 * (pcs_index - 1);
    int offset = index - base;

    if (offset >= 0 && offset < 300)
    {
        if (offset >= 0 && offset < 1)         return 3004; // offset 0
        else if (offset >= 3 && offset < 4)    return 3001; // offset 3
        else if (offset >= 4 && offset < 5)    return 3000; // offset 4
        else if (offset >= 5 && offset < 6)    return 3002; // offset 5
        else if (offset >= 6 && offset < 7)    return 3003; // offset 6
        else if (offset >= 7 && offset < 8)    return 3005; // offset 7
        else if (offset >= 8 && offset < 9)    return 3006; // offset 8

        else if (offset >= 9 && offset < 29)   return 3007 + (offset - 9);   // [3007, 3027)
        else if (offset >= 29 && offset < 36)  return 3050 + (offset - 29);  // [3050, 3057)

        else return -2; // offset不在已知范围
    }

    return -1; // index非法
}
/**************************************END******************************************************** */
 INT32S G2pro_Addr_From_Index(INT8U bms_index, INT32S index)
{
    if (bms_index == 0) return -1;                 
    INT32S base = 300 * (INT32S)(bms_index - 1);   
    INT32S offset = index - base;                     
    // 一键并网开关～清除PCS控制失效
    if (offset >= 35001 && offset <= 35004) {
        return 0x3001 + (offset - 35001);
    }
   //设置Rack启用/停用
    else if (offset >= 35005 && offset <= 35028) {
        return 0x3005 + (offset - 35005);
    }
   //本地/远程控制切换/一键复归
    else if (offset >= 35029 && offset <= 35031) {
        return 0x3045 + (offset - 35029);
    }
    //NTP 时区～BAU-EMS 通讯心跳
    else if (offset >= 35032 && offset <= 35039) {
        return 0x522D + (offset - 35032);
    }

    return -1; // 未命中
}
/**********************************协能BMS，根据EMS或者web发送的地址获得BMS编号******************************** */
 static int Bank_Base_Address[] = {
     35001, 35002, 35003, 35004, 35005, 35006, 35007, 35008, 35009, 35010,
     35011, 35012, 35013, 35014, 35015, 35016, 35017, 35018, 35019, 35020,
     35021, 35022, 35023, 35024, 35025, 35026, 35027, 35028, 35029, 35030,
     35031, 35032, 35033, 35034, 35035, 35036, 35037, 35038, 35039
     
 };
 
 #define NUM_BASES (sizeof(Bank_Base_Address)/sizeof(Bank_Base_Address[0]))
 //获取BANK_NUM
int Find_Bank_N(int addr, int *base_out) {
     for (int i = 0; i < NUM_BASES; i++) {
         int base = Bank_Base_Address[i];
         if ((addr - base) % 300 == 0) {
             int n = (addr - base) / 300 + 1;
             if (n >= 1 && n <= 10) {
                 *base_out = base-35000;
                 return n;
             }
         }
     }
     return -1; // 未找到
 }

 /*******************************************END*************************************************************** */

/**********************************trinaPCS，根据EMS或者web发送的地址获得pcs编号******************************** */
 static int TrinaPCS_Base_Address[] = {
     27009, 27010, 27011, 27012, 27013, 27014, 27015, 27016, 27017,
     27018, 27019, 27020, 27021, 27022, 27023, 27024,
     27025, 27026, 27027, 27028,
     27029, 27030, 27031, 27032,
     27033, 27034, 27035, 27000,27007,27003,27004,27006,27005,27008

 };
 
 #define NUM_BASES (sizeof(TrinaPCS_Base_Address)/sizeof(TrinaPCS_Base_Address[0]))
 //获取BANK_NUM
int Find_Trina_N(int addr, int *base_out) {
     for (int i = 0; i < NUM_BASES; i++) {
         int base = TrinaPCS_Base_Address[i];
         if ((addr - base) % 300 == 0) {
             int n = (addr - base) / 300 + 1;
             if (n >= 1 && n <= 10) {
                 *base_out = base-27000;
                 return n;
             }
         }
     }
     return -1; // 未找到
 }

 /*******************************************END*************************************************************** */

 /* *****************************************Taida写寄存器反向映射**************************************/
/**
 * @brief 根据PCS索引和地址索引计算Modbus寄存器地址
 * 
 * @param pcs_index PCS设备索引（从1开始）
 * @param index 输入地址索引
 * @return INT32S 计算得到的Modbus寄存器地址，若无效则返回-1
 * 
 * @details 该函数实现地址映射功能，将输入索引转换为Modbus寄存器地址：
 *          - 处理特殊地址区间（12000-12025, 12300-12325, 12600-12625, 12900-12925）
 *          - 处理单点映射（27000+103/105/106/46/47/48/49）
 *          - 处理区间映射（27000+107..112, 27000+113..118）
 *          - 支持多个PCS设备的地址偏移计算
 */
INT32S Taida_Addr_From_Index(INT8U pcs_index, INT32S index)
{
    if (pcs_index == 0) return -1;                  
    INT32S base = 300 * (INT32S)(pcs_index - 1);  
    INT32S offset = index - base;                      

    if((index>=12000)&&(index<=12032))
    {

    INT32S base = 300 * (INT32S)(pcs_index - 1);   
    INT32S offset = index - base;      
   
    if (offset >= 12000 + 0 && offset < 12000 + 0 + 24) {
        return 40201-40001 + (offset - (12000 + 0));
    }

    
    if (offset >= 12000 + 24 && offset < 12000 + 24 + 2) {
        return 40241-40001 + (offset - (12000 + 24));
    }

    if (offset >= 12000 + 26 && offset < 12000 + 33) {
        return 40011-40001 + (offset - (12000 + 26));
    }

    }
     else if((index>=12300)&&(index<=12332))
    {

    INT32S base = 300 * (INT32S)(pcs_index - 4);  
    INT32S offset = index - base;      
   
    if (offset >= 12000 + 0 && offset < 12000 + 0 + 24) {
        return 40201-40001 + (offset - (12000 + 0));
    }

    
    if (offset >= 12000 + 24 && offset < 12000 + 24 + 2) {
        return 40241-40001 + (offset - (12000 + 24));
    }

    if (offset >= 12000 + 26 && offset < 12000 + 33) {
        return 40011-40001 + (offset - (12000 + 26));
    }

    }   
     else if((index>=12600)&&(index<=12632))
    {

    INT32S base = 300 * (INT32S)(pcs_index);  
    INT32S offset = index - base;      
 
   
    if (offset >= 12000 + 0 && offset < 12000 + 0 + 24) {
        return 40201-40001 + (offset - (12000 + 0));
    }

   
    if (offset >= 12000 + 24 && offset < 12000 + 24 + 2) {
        return 40241-40001 + (offset - (12000 + 24));
    }

    if (offset >= 12000 + 26 && offset < 12000 + 33) {
        return 40011-40001 + (offset - (12000 + 26));
    }

    } 
     else if((index>=12900)&&(index<=12932))
    {

    INT32S base = 300 * (INT32S)(pcs_index - 3);  
    INT32S offset = index - base;      
    
    if (offset >= 12000 + 0 && offset < 12000 + 0 + 24) {
        return 40201-40001 + (offset - (12000 + 0));
    }

    
    if (offset >= 12000 + 24 && offset < 12000 + 24 + 2) {
        return 40241-40001 + (offset - (12000 + 24));
    }
    if (offset >= 12000 + 26 && offset < 12000 + 33) {
        return 40011-40001 + (offset - (12000 + 26));
    }
    } 
    // === 单点映射（与正向宏中的三个等值判断一致） ===
    if (offset == 27000 + 103) return 40103-40001;          // System ON/OFF Cmd
    if (offset == 27000 + 50) return 40105-40001;          // Master Bat1 Discharge Power Coef
    if (offset == 27000 + 51) return 40108-40001;          // Master Bat1 Charge Power Coef

    if (offset == 27000 + 46) return 40106-40001;          
    if (offset == 27000 + 47) return 40107-40001;          
    if (offset == 27000 + 48) return 40109-40001;          
    if (offset == 27000 + 49) return 40110-40001;  
    if (offset == 27000 + 52) return 40111-40001;          
    if (offset == 27000 + 53) return 40112-40001; 
    
    // === 区间一：40111..40116  → local = 27000+107 .. 27000+112 ===
    if (offset >= 27000 + 109 && offset < 27000 + 109 + 4) {
        return 40113-40001 + (offset - (27000 + 109));
    }

    // === 区间二：40229..40234  → local = 27000+113 .. 27000+18 ===
    if (offset >= 27000 + 113 && offset < 27000 + 113 + 6) {
        return 40229-40001 + (offset - (27000 + 113));
    }

    return -1; // 未命中
}

INT32S Trina_Addr_From_Index(INT8U pcs_index, INT32S index)
{
    if (pcs_index == 0) return -1;                  
    INT32S base = 700 * (INT32S)(pcs_index - 1);  
    INT32S offset = index - base;                      
    //并联模式～支路充电分配系数
    if (offset >= 12000 + 0 && offset <= 12000 + 36) {
        return 3000 + (offset - (12000 + 0));
    }
    //升级信号～PCS出厂设置
    if (offset >= 12000 + 77 && offset <= 12000 + 78) {
        return 3077 + (offset - (12000 + 77));
    }
  //IP地址设置
    if (offset >= 12000 + 86 && offset <=12000 + 89) {
        return 3086 + (offset - (12000 + 86));
    }
  //安规参数设置
    if (offset >= 12000 + 94 && offset <=12000 + 116) {
        return 3094 + (offset - (12000 + 94));
    }
  //安规参数设置
    if (offset >= 12000 + 118 && offset <=12000 + 201) {
        return 3118 + (offset - (12000 + 118));
    }
    
  //安规参数设置
    if (offset >= 12000 + 204 && offset <=12000 + 409) {
        return 3204 + (offset - (12000 + 204));
    }
  //安规参数设置
    if (offset >= 12000 + 440 && offset <=12000 + 461) {
        return 3800 + (offset - (12000 + 440));
    }
    return -1; // 未命中
}
/* 取指定位（返回 0 或 1）*/
static inline INT16U bit_u16(INT16U value, INT8U bit)
{
    return (INT16U)((value >> bit) & 0x1u);
}

/**
 * @brief  根据地址(99/100/101/103)把16位值按位写入到指定的保持寄存器区
 *         规则：
 *         - addr==99 : bit0..bit15 -> SET_HOLD(27000 + i)
 *         - addr==100: bit0..bit15 -> SET_HOLD(27016 + i)
 *         - addr==101: 仅 bit8 -> SET_HOLD(27040), bit9 -> SET_HOLD(27041)
 *         - addr==103: bit0..bit15 -> SET_HOLD(27042 + i)
 *
 * @param addr  地址（只处理 99/100/101/102）
 * @param value 16位值
 * @return 0 成功；-1 地址不支持
 */
int Write_Bits_By_Addr(INT16U addr, INT16U value,INT16U pcs_num)
{
    INT16U base;
    INT16U i;

    switch (addr) {
    case 99:
      
        SET_HOLD(27000+pcs_num*300, bit_u16(value, 0));
        SET_HOLD(27001+pcs_num*300, bit_u16(value, 1));
        // SET_HOLD(27004+pcs_num*300, bit_u16(value, 4));//PCS心跳在写心跳线程中直接写入到SET_HOLD中
        SET_HOLD(27005+pcs_num*300, bit_u16(value, 5));
        return 0;

    case 100:
        base = 27016+pcs_num*300;
        for (i = 0; i < 16; ++i) {
            SET_HOLD((INT16U)(base + i), bit_u16(value, (INT8U)i));
        }
        return 0;

    case 101:
        /* 仅写 bit8 和 bit9 */
        SET_HOLD(27040+pcs_num*300, bit_u16(value, 8));
        SET_HOLD(27041+pcs_num*300, bit_u16(value, 9));
        return 0;

    case 103:
        base = 27042+pcs_num*300;
        for (i = 0; i < 4; ++i) {
            SET_HOLD((INT16U)(base + i), bit_u16(value, (INT8U)i));
        }
        return 0;

    default:
        /* 不支持的地址 */
        return -1;
    }
}

  /*******************************************END*************************************************************** */
  /**********************************TaidaPCS，根据EMS或者web发送的地址获得pcs编号******************************** */
 static int Taida_PCS_Base_Address[] = {
     27036, 27037, 
     27043, 27044, 27045, 27046, 27047, 27048, 27049,27050,
     27051, 27052, 27053, 27054, 27055, 
     27056, 27057, 27058, 27059, 
     27035, 27034, 27033, 27032,27031,27015,27000,27001,27038,27024,

 };
 
 #define NUM_BASES (sizeof(Taida_PCS_Base_Address)/sizeof(Taida_PCS_Base_Address[0]))
 //获取PCS_NUM
int Find_Taida_N(int addr, int *base_out) {
     for (int i = 0; i < NUM_BASES; i++) {
         int base = Taida_PCS_Base_Address[i];
         if ((addr - base) % 300 == 0) {
             int n = (addr - base) / 300 + 1;
             if (n >= 1 && n <= 10) {
                 *base_out = base-27000;
                 return n;
             }
         }
     }
     return -1; // 未找到
 }

 /*******************************************END*************************************************************** */

  /**********************************cem9000******************************** ************************************/
#define CEM9000_SRC_BASE      1000
#define CEM9000_SRC_LAST      1007
#define EMS_DST_BASE      27300
#define BITS_PER_REG  16



/*  目标地址 = 27300 + (src_reg-1000)*16 + bit */
static inline INT16U map_dst_addr(INT16U src_reg, INT8U bit)
{
    return (INT16U)(EMS_DST_BASE + (src_reg - CEM9000_SRC_BASE) * BITS_PER_REG + bit);
}

static inline INT16U map_hold_dst_addr(INT8U bit)
{
    return (INT16U)(33200 + bit);
}

int CEM9000_INPUT(INT16U address, INT16U value)
{
    if (address < CEM9000_SRC_BASE || address > CEM9000_SRC_LAST) return -10;  // 超范围保护

    for (INT8U bit = 0; bit < BITS_PER_REG; ++bit) {
        INT16U v = (INT16U)((value >> bit) & 0x1u);     // 取该位的 0/1
        INT16U dst = map_dst_addr(address, bit);             // 计算目标地址
    //罗马尼亚项目测控PCS故障干节点特殊处理
        if(dst==27355)
        {
            if(v==1)
            {
                v=0;
            }
            else
            {
                v=1;
            }
        }
         if(dst==27356)
        {
            if(v==1)
            {
                v=0;
            }
            else
            {
                v=1;
            }
        }
         if(dst==27357)
        {
            if(v==1)
            {
                v=0;
            }
            else
            {
                v=1;
            }
        }
         if(dst==27358)
        {
            if(v==1)
            {
                v=0;
            }
            else
            {
                v=1;
            }
        }
        SET_INPUT(dst, v);
    
    }
    return 0;
}


int CEM9000_HOLD(INT16S value)
{
    for (INT8U bit = 0; bit < 8; ++bit) {
        INT16U v = (INT16U)((value >> bit) & 0x1u);     // 取该位的 0/1
        INT16U dst = map_hold_dst_addr(bit);             // 计算目标地址
        SET_HOLD(dst, v);
     
    }
    return 0;
}
 /*******************************************END*************************************************************** */
/* ------------------------------------------------------------
* Copyright (C) 2026
* File Name : log_g2_mv.c
* Project :
* Description : MV一体机PCS故障日志
* File Created :tyq

* ------------------------------------------------------------

* ------------------------------------------------------------
* HISTORY : 
*/
#include "main.h"
/*只打印一次 */
static void Alarm_Process(AlarmInstance_t *inst, uint16_t value)
{
    uint16_t last = inst->last_value;

    uint16_t new_set = (uint16_t)(value & (uint16_t)~last); // 0->1
    uint16_t new_clr = (uint16_t)((uint16_t)~value & last); // 1->0

    const AlarmMap_t *map = inst->map;

    if ((new_set | new_clr) == 0) {
        inst->last_value = value;
        return;
    }

    for (uint8_t b = 0; b < 16; b++) {
        const char *name = map->bit_name[b];
        if (!name || name[0] == '\0') continue;

        if (new_set & BIT(b)) {
            LOG_INFO("PCS%d [发生]: %s Bit%u -> %s (val=0x%04X)",
                     inst->num,
           
                     map->word_name ? map->word_name : "UNK",
                     (unsigned)b,
                     name,
                     (unsigned)value);
        }

        if (new_clr & BIT(b)) {
            LOG_INFO(" PCS%d [恢复]: %s Bit%u -> %s (val=0x%04X)",
                     inst->num,
      
                     map->word_name ? map->word_name : "UNK",
                     (unsigned)b,
                     name,
                     (unsigned)value);
        }
    }

    inst->last_value = value;
}



/* SysF1 点表 */
static const AlarmMap_t g_SysF1_Map = {
    "故障字1",
    {
        "故障字1交流电压有效值过压",      // Bit0
        "故障字1电网频率过高",            // Bit1
        "故障字1电网频率过低",            // Bit2
        "故障字1电网电压不平衡",          // Bit3
        "故障字1电网电压瞬时过压",        // Bit4
        "故障字1交流电压有效值欠压",      // Bit5
        "故障字1防孤岛保护",              // Bit6
        "故障字1电压相位参数错误",        // Bit7
        "故障字1无功功率参数错误",        // Bit8
        "故障字1电压调节参数错误",        // Bit9
        "故障字1同步调节参数错误",        // Bit10
        "故障字1电网平均过压",            // Bit11
        "故障字1保留12",                  // Bit12
        "故障字1保留13",                  // Bit13
        "故障字1保留14",                  // Bit14
        "故障字1保留15"                   // Bit15
    }
};

/* SysF2 点表 */
static const AlarmMap_t g_SysF2_Map = {
    "故障字2",
    {
        "控制电源丢失",
        "通信电源丢失",
        "IO电源丢失",
        "SDSP与FPGA间EMIF总线错误",
        "紧急停机",
        "外部输入故障1",
        "控制器心跳丢失",
        "J1 DSP心跳丢失",
        "J2 DSP心跳丢失",
        "烟雾故障",
        "主从机1通信丢失",
        "保留11",
        "外部输入故障2",
        "PCS与EMS通信丢失",
        "保留14",
        "保留15"
    }
};

/* SysF3 点表 */
static const AlarmMap_t g_SysF3_Map = {
    "故障字3",
    {
        "总零序过电流保护",        // Bit0
        "功率反馈偏差",            // Bit1
        "电网电压相序错误",        // Bit2
        "直流霍尔传感器断开",      // Bit3
        "保留4",                  // Bit4
        "ADC校准错误",            // Bit5
        "控制错误",               // Bit6
        "模块1检测到故障",        // Bit7
        "模块2检测到故障",        // Bit8
        "保留9",                  // Bit9
        "保留10",                 // Bit10
        "保留11",                 // Bit11
        "保留12",                 // Bit12
        "保留13",                 // Bit13
        "保留14",                 // Bit14
        "保留15"                  // Bit15
    }
};

static const AlarmMap_t g_MidVolt_Fault1_Map = {
    "中压故障字1",
    {
        "重气体跳闸",            // Bit0
        "轻气体故障",            // Bit1
        "油温过高跳闸",          // Bit2
        "保留3",                 // Bit3
        "低油位跳闸",            // Bit4
        "压力释放",              // Bit5
        "低气压",                // Bit6
        "继电器485通信丢失",     // Bit7
        "Y板通信丢失",           // Bit8
        "变压器油温UTP",         // Bit9
        "保留10",                // Bit10
        "保留11",                // Bit11
        "保留12",                // Bit12
        "保留13",                // Bit13
        "保留14",                // Bit14
        "保留15"                 // Bit15
    }
};

static const AlarmMap_t g_SystemWarn1_Map = {
    "系统警告字1",
    {
        "模块1 IP地址错误",            // Bit0
        "模块2 IP地址错误",            // Bit1
        "由于过温降额",                // Bit2
        "模块1检测到警告",              // Bit3
        "模块2检测到警告",              // Bit4
        "3352 SPI通信丢失",            // Bit5
        "从机1故障",                    // Bit6
        "保留7",                        // Bit7
        "从机1警告",                    // Bit8
        "保留9",                        // Bit9
        "EMS心跳未变化",               // Bit10
        "Web服务器通信丢失",           // Bit11
        "PCS准备开启未满足",           // Bit12
        "RMU检测到跳闸",               // Bit13
        "MVG3断路器跳闸",              // Bit14
        "MVG3断路器未存储能量"          // Bit15
    }
};
static const AlarmMap_t g_SystemWarn2_Map = {
    "系统告警字2",
    {
        "保留0",                       // Bit0
        "保留1",                       // Bit1
        "保留2",                       // Bit2
        "保留3",                       // Bit3
        "MV继电器CT开路",              // Bit4
        "MV RMU环境过温",              // Bit5
        "MV RMU热交换器报警",          // Bit6
        "变压器油温UTP降额报警",       // Bit7
        "变压器油温过高报警",          // Bit8
        "保留9",                       // Bit9
        "保留10",                      // Bit10
        "保留11",                      // Bit11
        "保留12",                      // Bit12
        "保留13",                      // Bit13
        "保留14",                      // Bit14
        "保留15"                       // Bit15
    }
};
static const AlarmMap_t g_ControlWarn1_Map = {
    "控制警告字1",
    {
        "Web服务器丢失",     // Bit0
        "保留1",            // Bit1
        "保留2",            // Bit2
        "保留3",            // Bit3
        "保留4",            // Bit4
        "保留5",            // Bit5
        "保留6",            // Bit6
        "保留7",            // Bit7
        "保留8",            // Bit8
        "保留9",            // Bit9
        "保留10",           // Bit10
        "保留11",           // Bit11
        "保留12",           // Bit12
        "保留13",           // Bit13
        "保留14",           // Bit14
        "保留15"            // Bit15
    }
};

static const AlarmMap_t g_FPGA_Fault1_Map = {
    "FPGA故障字1",
    {
        "R相驱动板故障",        // Bit0
        "S相驱动板故障",        // Bit1
        "T相驱动板故障",        // Bit2
        "R相电感过流",          // Bit3
        "S相电感过流",          // Bit4
        "T相电感过流",          // Bit5
        "电池过流保护",          // Bit6
        "电池过压保护",          // Bit7
        "上半母线过压",          // Bit8
        "下半母线过压",          // Bit9
        "控制器断开R",          // Bit10
        "逐波限流故障S",        // Bit11
        "逐波限流故障T",        // Bit12
        "控制电源丢失",          // Bit13
        "通信电源丢失",          // Bit14
        "IO电源丢失"            // Bit15
    }
};
static const AlarmMap_t g_FPGA_Fault2_Map = {
    "FPGA故障字2",
    {
        "串口光纤错误",    // Bit0
        "保留1",           // Bit1
        "保留2",           // Bit2
        "保留3",           // Bit3
        "保留4",           // Bit4
        "保留5",           // Bit5
        "保留6",           // Bit6
        "保留7",           // Bit7
        "保留8",           // Bit8
        "保留9",           // Bit9
        "保留10",          // Bit10
        "保留11",          // Bit11
        "保留12",          // Bit12
        "保留13",          // Bit13
        "保留14",          // Bit14
        "保留15"           // Bit15
    }
};
static const AlarmMap_t g_FPGA_Fault3_Map = {
    "FPGA故障字3",
    {
        "1管短路",          // Bit0
        "1管欠压",          // Bit1
        "2管短路",          // Bit2
        "2管欠压",          // Bit3
        "3管短路",          // Bit4
        "3管欠压",          // Bit5
        "4管短路",          // Bit6
        "4管欠压",          // Bit7
        "5管短路",          // Bit8
        "5管欠压",          // Bit9
        "6管短路",          // Bit10
        "6管欠压",          // Bit11
        "上管门级错误",      // Bit12
        "下管门级错误",      // Bit13
        "过温保护",          // Bit14
        "窄脉冲错误"         // Bit15
    }
};

static const AlarmMap_t g_FPGA_Fault4_Map = {
    "FPGA故障字4",
    {
        "1管短路",          // Bit0
        "1管欠压",          // Bit1
        "2管短路",          // Bit2
        "2管欠压",          // Bit3
        "3管短路",          // Bit4
        "3管欠压",          // Bit5
        "4管短路",          // Bit6
        "4管欠压",          // Bit7
        "5管短路",          // Bit8
        "5管欠压",          // Bit9
        "6管短路",          // Bit10
        "6管欠压",          // Bit11
        "上管门级错误",      // Bit12
        "下管门级错误",      // Bit13
        "过温保护",          // Bit14
        "窄脉冲错误"         // Bit15
    }
};
static const AlarmMap_t g_FPGA_Fault5_Map = {
    "FPGA故障字5",
    {
        "1管短路",          // Bit0
        "1管欠压",          // Bit1
        "2管短路",          // Bit2
        "2管欠压",          // Bit3
        "3管短路",          // Bit4
        "3管欠压",          // Bit5
        "4管短路",          // Bit6
        "4管欠压",          // Bit7
        "5管短路",          // Bit8
        "5管欠压",          // Bit9
        "6管短路",          // Bit10
        "6管欠压",          // Bit11
        "上管门级错误",      // Bit12
        "下管门级错误",      // Bit13
        "过温保护",          // Bit14
        "窄脉冲错误"         // Bit15
    }
};

static const AlarmMap_t Modle_Warn1_Map = {
    "模块告警字1",
    {
        "支路1绝缘低",               // Bit0
        "支路2绝缘低",               // Bit1
        "湿度高",                   // Bit2
        "除湿器故障",               // Bit3
        "UPS电池电压低",            // Bit4
        "J板过温保护",              // Bit5
        "R相驱动线松动",            // Bit6
        "S相驱动线松动",            // Bit7
        "T相驱动线松动",            // Bit8
        "UPS交流电源丢失",          // Bit9
        "直流浪涌保护器告警",        // Bit10
        "辅助变压器开关馈电",        // Bit11
        "交流浪涌保护器告警",        // Bit12
        "辅助变压器浪涌保护器告警",  // Bit13
        "3352通信丢失",             // Bit14
        "IMD通信丢失"               // Bit15
    }
};

static const AlarmMap_t g_ControllerFault1_Map = {
    "控制器故障字1",
    {
        "SDSP心跳丢失",          // Bit0
        "J1 DSP心跳丢失",        // Bit1
        "J2 DSP心跳丢失",        // Bit2
        "保留位3",               // Bit3
        "保留位4",               // Bit4
        "主从1通信丢失",         // Bit5
        "保留位6",               // Bit6
        "保留位7",               // Bit7
        "保留位8",               // Bit8
        "保留位9",               // Bit9
        "保留位10",              // Bit10
        "保留位11",              // Bit11
        "保留位12",              // Bit12
        "保留位13",              // Bit13
        "保留位14",              // Bit14
        "保留位15"               // Bit15
    }
};

static const AlarmMap_t g_MV_TotalFault1_Map = {
    "中压综合故障字1",
    {
        "重瓦斯跳闸",                 // Bit0
        "轻瓦斯故障",                 // Bit1
        "油温过温保护跳闸",           // Bit2
        "保留位3",                    // Bit3
        "低油位跳闸",                 // Bit4
        "泄压动作（压力释放）",       // Bit5
        "低气压",                     // Bit6
        "继电器485通信丢失",          // Bit7
        "Y板通信丢失",                // Bit8
        "变压器油温过低",             // Bit9
        "保留位10",                   // Bit10
        "保留位11",                   // Bit11
        "保留位12",                   // Bit12
        "保留位13",                   // Bit13
        "保留位14",                   // Bit14
        "保留位15"                    // Bit15
    }
};
static const AlarmMap_t g_SystemStatus2_Map = {
    "系统状态字2",
    {
        "保留0",              // Bit0
        "保留1",              // Bit1
        "岛屿断路器开启",      // Bit2
        "外部故障输入",        // Bit3
        "中压变压器ST",        // Bit4
        "保留5",              // Bit5
        "电池连接正常",        // Bit6
        "电网准备",            // Bit7
        "主模块1运行",         // Bit8
        "主模块2运行",         // Bit9
        "主模块1待机",         // Bit10
        "主模块2待机",         // Bit11
        "主模块1故障",         // Bit12
        "主模块2故障",         // Bit13
        "保留14",             // Bit14
        "保留15"              // Bit15
    }
};
static const AlarmMap_t g_SystemStatus3_Map = {
    "系统状态字3",
    {
        "从机待机",     // Bit0
        "从机故障",     // Bit1
        "从机运行",     // Bit2
        "保留3",        // Bit3
        "保留4",        // Bit4
        "保留5",        // Bit5
        "保留6",        // Bit6
        "保留7",        // Bit7
        "保留8",        // Bit8
        "保留9",        // Bit9
        "保留10",       // Bit10
        "保留11",       // Bit11
        "保留12",       // Bit12
        "保留13",       // Bit13
        "保留14",       // Bit14
        "保留15"        // Bit15
    }
};
static const AlarmMap_t g_SystemMode1_Map = {
    "系统模式字1",
    {
        "PQ模式",           // Bit0
        "PF模式",           // Bit1
        "电压-无功模式",     // Bit2
        "电压-有功模式",     // Bit3
        "频率-有功模式",     // Bit4
        "黑启动模式",        // Bit5
        "保留6",            // Bit6
        "保留7",            // Bit7
        "保留8",            // Bit8
        "保留9",            // Bit9
        "保留10",           // Bit10
        "保留11",           // Bit11
        "保留12",           // Bit12
        "保留13",           // Bit13
        "保留14",           // Bit14
        "保留15"            // Bit15
    }
};

static const AlarmMap_t g_IO_Fault1_Map = {
    "IO故障字1",
    {
        "交流电容继电器反馈异常",     // Bit0
        "直流主断路器反馈异常",       // Bit1
        "交流主断路器反馈异常",       // Bit2
        "主风扇继电器反馈异常",       // Bit3
        "DCSS馈电异常",               // Bit4
        "DC熔断器异常",               // Bit5
        "断路器跳闸",                 // Bit6
        "保留7",                      // Bit7
        "保留8",                      // Bit8
        "主风扇机主体故障",           // Bit9
        "电感过温保护",               // Bit10
        "辅助变压器过温保护",         // Bit11
        "保留12",                     // Bit12
        "保留13",                     // Bit13
        "保留14",                     // Bit14
        "机柜进气水检测",             // Bit15
    }
};

static const AlarmMap_t g_IO_Fault2_Map = {
    "IO故障字2",
    {
        "保留0",                // Bit0
        "保留1",                // Bit1
        "模块风扇故障R",        // Bit2
        "模块风扇故障S",        // Bit3
        "模块风扇故障T",        // Bit4
        "JDSPFPGA内存错误",     // Bit5
        "保留6",                // Bit6
        "保留7",                // Bit7
        "保留8",                // Bit8
        "保留9",                // Bit9
        "保留10",               // Bit10
        "保留11",               // Bit11
        "保留12",               // Bit12
        "保留13",               // Bit13
        "保留14",               // Bit14
        "保留15"                // Bit15
    }
};

static const AlarmMap_t g_SampleFault1_Map = {
    "样本故障字1",
    {
        "交流有效值过流",              // Bit0
        "交流电感过流",                // Bit1
        "零序慢过流",                  // Bit2
        "零序快过流",                  // Bit3
        "交流电容器电流有效值过流",    // Bit4
        "交流电容器电流有效值过低",    // Bit5
        "交流电容器过流",              // Bit6
        "交流电容器电流采样异常",      // Bit7
        "交流电容器容值低",            // Bit8
        "电池过流",                    // Bit9
        "电池过压",                    // Bit10
        "电池欠压",                    // Bit11
        "总线过压",                    // Bit12
        "总线欠压",                    // Bit13
        "总线电池不匹配",              // Bit14
        "总线不平衡"                   // Bit15
    }
};

static const AlarmMap_t g_SampleFault2_Map = {
    "样本故障字2",
    {
        "电池反接",                     // Bit0
        "保留1",                        // Bit1
        "IGBT过温",                     // Bit2
        "交流电容器环境过温",             // Bit3
        "DC电容器环境过温",              // Bit4
        "DC熔断器环境过温",              // Bit5
        "外风扇转速异常保护",             // Bit6
        "外风扇过流",                    // Bit7
        "内部风扇过电流",                 // Bit8
        "内部风扇过流",                   // Bit9
        "交流电容器风扇转速异常保护",      // Bit10
        "交流电容器风扇过流",             // Bit11
        "ADC校准错误",                   // Bit12
        "总线1绝缘低",                   // Bit13
        "总线2绝缘低",                   // Bit14
        "IMD双重启用"                    // Bit15
    }
};


static const AlarmMap_t g_SampleFault3_Map = {
    "样本故障字3",
    {
        "DCSS故障",   // Bit0
        "保留1",      // Bit1
        "保留2",      // Bit2
        "保留3",      // Bit3
        "保留4",      // Bit4
        "保留5",      // Bit5
        "保留6",      // Bit6
        "保留7",      // Bit7
        "保留8",      // Bit8
        "保留9",      // Bit9
        "保留10",     // Bit10
        "保留11",     // Bit11
        "保留12",     // Bit12
        "保留13",     // Bit13
        "保留14",     // Bit14
        "保留15"      // Bit15
    }
};

static const AlarmMap_t g_CommFault1_Map = {
    "通信故障字1",
    {
        "SDSP心跳丢失",     // Bit0
        "控制心跳丢失",     // Bit1
        "保留2",            // Bit2
        "保留3",            // Bit3
        "J板ID错误",        // Bit4
        "保留5",            // Bit5
        "保留6",            // Bit6
        "保留7",            // Bit7
        "保留8",            // Bit8
        "保留9",            // Bit9
        "保留10",           // Bit10
        "保留11",           // Bit11
        "保留12",           // Bit12
        "保留13",           // Bit13
        "保留14",           // Bit14
        "保留15"            // Bit15
    }
};

static const AlarmMap_t g_ModuleWarn1_Map = {
    "模块警告字1",
    {
        "总线1绝缘低",           // Bit0
        "总线2绝缘低",           // Bit1
        "湿度过高",              // Bit2
        "除湿机故障",            // Bit3
        "UPS电池电压低",         // Bit4
        "J板过温",               // Bit5
        "相R驱动线松动",         // Bit6
        "相S驱动线松动",         // Bit7
        "相T驱动线松动",         // Bit8
        "UPS交流电源丢失",       // Bit9
        "DC SPD告警",            // Bit10
        "辅助变压器SW馈电",      // Bit11
        "AC SPD告警",            // Bit12
        "辅助SPD告警",           // Bit13
        "3352通信丢失",          // Bit14
        "IMD通信丢失"            // Bit15
    }
};
static const AlarmMap_t g_ModuleWarn2_Map = {
    "模块警告字2",
    {
        "CBC警告R",          // Bit0
        "CBC警告S",          // Bit1
        "CBC警告T",          // Bit2
        "电池欠压",   // Bit3
        "外风扇欠流", // Bit4
        "保留5",             // Bit5
        "保留6",             // Bit6
        "保留7",             // Bit7
        "保留8",             // Bit8
        "保留9",             // Bit9
        "保留10",            // Bit10
        "保留11",            // Bit11
        "保留12",            // Bit12
        "保留13",            // Bit13
        "保留14",            // Bit14
        "保留15"             // Bit15
    }
};
//天合PCS故障表
static const AlarmMap_t g_MsCtrlDspFaultInfo1_Map =
{
    "MS控制DSP故障信息表1",
    {
        "安规国家无效",      // Bit0
        "交流10分钟过压",    // Bit1
        "交流1级过压",       // Bit2
        "交流2级过压",       // Bit3
        "交流3级过压",       // Bit4
        "交流4级过压",       // Bit5
        "交流5级过压",       // Bit6
        "交流1级欠压",       // Bit7
        "交流2级欠压",       // Bit8
        "交流3级欠压",       // Bit9
        "交流4级欠压",       // Bit10
        "交流5级欠压",       // Bit11
        "交流1级过频",       // Bit12
        "交流2级过频",       // Bit13
        "交流3级过频",       // Bit14
        "交流4级过频"        // Bit15
    }
};

static const AlarmMap_t g_MsCtrlDspFaultInfo2_Map =
{
    "MS控制DSP故障信息表2",
    {
        "交流5级过频",        // Bit0
        "交流1级欠频",        // Bit1
        "交流2级欠频",        // Bit2
        "交流3级欠频",        // Bit3
        "交流4级欠频",        // Bit4
        "交流5级欠频",        // Bit5
        "频率变化率过高",      // Bit6
        "交流电压不平衡",      // Bit7
        "交流瞬时过压",        // Bit8
        "DC1级保护",         // Bit9
        "DC2级保护",         // Bit10
        "DC3级保护",         // Bit11
        "DCV保护",          // Bit12
        "过调制故障",         // Bit13
        "J1板故障",         // Bit14
        "J2板故障"          // Bit15
    }
};


static const AlarmMap_t g_MsCtrlDspFaultInfo3_Map =
{
    "MS控制DSP故障信息表3",
    {
        "交流断路器故障",         // Bit0
        "J1直流软起故障",        // Bit1
        "J1直流预充继电器故障",   // Bit2
        "J1直流断路器故障",      // Bit3
        "J1滤波电容断路器故障",   // Bit4
        "J2直流软起故障",        // Bit5
        "J2直流预充继电器故障",   // Bit6
        "J2直流断路器故障",      // Bit7
        "J2滤波电容断路器故障",   // Bit8
        "交流预同步故障",        // Bit9
        "EMIF写入故障",        // Bit10
        "EMIF读取故障",        // Bit11
        "FPGA心跳故障",       // Bit12
        "保留",              // Bit13
        "保留",              // Bit14
        "保留"               // Bit15
    }
};

static const AlarmMap_t g_MsCtrlDspFaultInfo4_Map =
{
    "MS控制DSP故障信息表4",
    {
        "EMS失联",            // Bit0
        "读写FeRam故障",      // Bit1
        "网络通信故障",        // Bit2
        "中压变压器故障",      // Bit3
        "BMS故障",           // Bit4
        "与FPGA通信故障",     // Bit5
        "J1快速保护(IO)",    // Bit6
        "J2快速保护(IO)",    // Bit7
        "J3快速保护(IO)",    // Bit8
        "J4快速保护(IO)",    // Bit9
        "24V供电-IO",       // Bit10
        "24V供电-控制",      // Bit11
        "24V供电-通信",      // Bit12
        "EPO",             // Bit13
        "从机故障",          // Bit14
        "保留"             // Bit15
    }
};
static const AlarmMap_t g_MsCtrlFpgaFaultInfo1_Map =
{
    "MS控制FPGA故障信息表1",
    {
        "J1光纤线缆异常",           // Bit0
        "J1光纤数据校验CRC错误故障", // Bit1
        "J2光纤线缆异常",           // Bit2
        "J2光纤数据校验CRC错误故障", // Bit3
        "J3光纤线缆异常",           // Bit4
        "J3光纤数据校验CRC错误故障", // Bit5
        "保留",                    // Bit6
        "保留",                    // Bit7
        "保留",                    // Bit8
        "保留",                    // Bit9
        "保留",                    // Bit10
        "保留",                    // Bit11
        "保留",                    // Bit12
        "保留",                    // Bit13
        "保留",                    // Bit14
        "保留"                     // Bit15
    }
};
static const AlarmMap_t g_J1DspFaultInfo1_Map =
{
    "J1DSP故障信息表1",
    {
        "电感瞬时过流",         // Bit0
        "电感过流",            // Bit1
        "零序瞬时过流",        // Bit2
        "零序过流",            // Bit3
        "滤波电容过流",        // Bit4
        "滤波电容交流",        // Bit5
        "滤波电容瞬时过流",    // Bit6
        "电池瞬时过流",        // Bit7
        "电池过压",            // Bit8
        "电池欠压",            // Bit9
        "母线瞬时过压",        // Bit10
        "母线过压",            // Bit11
        "母线瞬时欠压",        // Bit12
        "母线欠压",            // Bit13
        "母线与电池电压偏差大", // Bit14
        "母线瞬时中偏"         // Bit15
    }
};
static const AlarmMap_t g_J1DspFaultInfo2_Map =
{
    "J1DSP故障信息表2",
    {
        "母线中偏",           // Bit0
        "电池反接",           // Bit1
        "IGBT过温",          // Bit2
        "交流电容过温",       // Bit3
        "直流电容过温",       // Bit4
        "直流熔丝过温",       // Bit5
        "主风机欠流",         // Bit6
        "主风机过流",         // Bit7
        "扰流风机欠流",       // Bit8
        "扰流风机过流",       // Bit9
        "交流电容风机欠流",   // Bit10
        "交流电容风机过流",   // Bit11
        "网侧电流瞬时过流",   // Bit12
        "网侧电流过流",       // Bit13
        "内部环温过温",       // Bit14
        "外部环温过温"        // Bit15
    }
};
static const AlarmMap_t g_J1DspFaultInfo3_Map =
{
    "J1DSP故障信息表3",
    {
        "BUS1绝缘阻抗低故障",     // Bit0
        "BUS2绝缘阻抗低故障",     // Bit1
        "直流熔丝故障",           // Bit2
        "交流断路器脱扣故障",     // Bit3
        "主机状态反馈故障",       // Bit4
        "电抗器过温故障",         // Bit5
        "辅助变压器过温故障",     // Bit6
        "机柜进水故障",           // Bit7
        "R相模块风扇故障",        // Bit8
        "S相模块风扇故障",        // Bit9
        "T相模块风扇故障",        // Bit10
        "24V IO供电故障",        // Bit11
        "24V通信供电故障",       // Bit12
        "24V控制供电故障",       // Bit13
        "主风机供电故障",         // Bit14
        "EMIF总线写入故障"       // Bit15
    }
};
static const AlarmMap_t g_J1DspFaultInfo4_Map =
{
    "J1DSP故障信息表4",
    {
        "EMIF总线读取故障",   // Bit0
        "FPGA心跳故障",      // Bit1
        "网络通信故障",       // Bit2
        "IMD板通信故障",     // Bit3
        "保留",             // Bit4
        "保留",             // Bit5
        "保留",             // Bit6
        "保留",             // Bit7
        "保留",             // Bit8
        "保留",             // Bit9
        "保留",             // Bit10
        "保留",             // Bit11
        "保留",             // Bit12
        "保留",             // Bit13
        "保留",             // Bit14
        "保留"              // Bit15
    }
};
static const AlarmMap_t g_J1FpgaFaultInfo1_Map =
{
    "J1FPGA故障信息表1",
    {
        "负母线过压",        // Bit0
        "正母线过压",        // Bit1
        "电池过压",          // Bit2
        "T相驱动板故障",     // Bit3
        "S相驱动板故障",     // Bit4
        "R相驱动板故障",     // Bit5
        "T相逐波限流故障",   // Bit6
        "S相逐波限流故障",   // Bit7
        "R相逐波限流故障",   // Bit8
        "电池过流",          // Bit9
        "T相过流",           // Bit10
        "S相过流",           // Bit11
        "R相过流",           // Bit12
        "R相过流",           // Bit13
        "保留",             // Bit14
        "保留"              // Bit15
    }
};
static const AlarmMap_t g_J1FpgaFaultInfo2_Map =
{
    "J1FPGA故障信息表2",
    {
        "管1(上管)短路故障",      // Bit0
        "管1(上管)电源欠压",      // Bit1
        "管2(下管)短路故障",      // Bit2
        "管2(下管)电源欠压",      // Bit3
        "管3短路故障",           // Bit4
        "管3电源欠压",           // Bit5
        "管4短路故障",           // Bit6
        "管4电源欠压",           // Bit7
        "管5短路故障",           // Bit8
        "管5电源欠压",           // Bit9
        "管6短路故障",           // Bit10
        "管6电源欠压",           // Bit11
        "上管门级检测",          // Bit12
        "下管门级检测",          // Bit13
        "过温故障",             // Bit14
        "窄脉冲"               // Bit15
    }
};
static const AlarmMap_t g_J1FpgaFaultInfo3_Map =
{
    "J1FPGA故障信息表3",
    {
        "管1(上管)短路故障",    // Bit0
        "管1(上管)电源欠压",    // Bit1
        "管2(下管)短路故障",    // Bit2
        "管2(下管)电源欠压",    // Bit3
        "管3短路故障",         // Bit4
        "管3电源欠压",         // Bit5
        "管4短路故障",         // Bit6
        "管4电源欠压",         // Bit7
        "管5短路故障",         // Bit8
        "管5电源欠压",         // Bit9
        "管6短路故障",         // Bit10
        "管6电源欠压",         // Bit11
        "上管门级检测",        // Bit12
        "下管门级检测",        // Bit13
        "过温故障",           // Bit14
        "窄脉冲"             // Bit15
    }
};
static const AlarmMap_t g_J1FpgaFaultInfo4_Map =
{
    "J1FPGA故障信息表4",
    {
        "管1(上管)短路故障",    // Bit0
        "管1(上管)电源欠压",    // Bit1
        "管2(下管)短路故障",    // Bit2
        "管2(下管)电源欠压",    // Bit3
        "管3短路故障",         // Bit4
        "管3电源欠压",         // Bit5
        "管4短路故障",         // Bit6
        "管4电源欠压",         // Bit7
        "管5短路故障",         // Bit8
        "管5电源欠压",         // Bit9
        "管6短路故障",         // Bit10
        "管6电源欠压",         // Bit11
        "上管门级检测",        // Bit12
        "下管门级检测",        // Bit13
        "过温故障",           // Bit14
        "窄脉冲"             // Bit15
    }
};
static const AlarmMap_t g_J1Fpga2FaultInfo1_Map =
{
    "J1FPGA2故障信息表1",
    {
        "光纤线缆异常故障",        // Bit0
        "光纤数据校验CRC错误故障", // Bit1
        "R相驱动解码异常故障",     // Bit2
        "S相驱动解码异常故障",     // Bit3
        "T相驱动解码异常故障",     // Bit4
        "R相NTC采样数据异常故障",  // Bit5
        "S相NTC采样数据异常故障",  // Bit6
        "T相NTC采样数据异常故障",  // Bit7
        "光纤检测到快速故障",      // Bit8
        "保留",                  // Bit9
        "保留",                  // Bit10
        "保留",                  // Bit11
        "保留",                  // Bit12
        "保留",                  // Bit13
        "保留",                  // Bit14
        "保留"                   // Bit15
    }
};
static const AlarmMap_t g_J2DspFaultInfo1_Map =
{
    "J2DSP故障信息表1",
    {
        "电感瞬时过流",         // Bit0
        "电感过流",            // Bit1
        "零序瞬时过流",        // Bit2
        "零序过流",            // Bit3
        "滤波电容过流",        // Bit4
        "滤波电容交流",        // Bit5
        "滤波电容瞬时过流",    // Bit6
        "电池瞬时过流",        // Bit7
        "电池过压",            // Bit8
        "电池欠压",            // Bit9
        "母线瞬时过压",        // Bit10
        "母线过压",            // Bit11
        "母线瞬时欠压",        // Bit12
        "母线欠压",            // Bit13
        "母线与电池电压偏差大", // Bit14
        "母线瞬时中偏"         // Bit15
    }
};

static const AlarmMap_t g_J2DspFaultInfo2_Map =
{
    "J2DSP故障信息表2",
    {
        "母线中偏",           // Bit0
        "电池反接",           // Bit1
        "IGBT过温",          // Bit2
        "交流电容过温",       // Bit3
        "直流电容过温",       // Bit4
        "直流熔丝过温",       // Bit5
        "主风机欠流",         // Bit6
        "主风机过流",         // Bit7
        "扰流风机欠流",       // Bit8
        "扰流风机过流",       // Bit9
        "交流电容风机欠流",   // Bit10
        "交流电容风机过流",   // Bit11
        "网侧电流瞬时过流",   // Bit12
        "网侧电流过流",       // Bit13
        "内部环温过温",       // Bit14
        "外部环温过温"        // Bit15
    }
};
static const AlarmMap_t g_J2DspFaultInfo3_Map =
{
    "J2DSP故障信息表3",
    {
        "BUS1绝缘阻抗低故障",     // Bit0
        "BUS2绝缘阻抗低故障",     // Bit1
        "直流熔丝故障",           // Bit2
        "交流断路器脱扣故障",     // Bit3
        "主机状态反馈故障",       // Bit4
        "电抗器过温故障",         // Bit5
        "辅助变压器过温故障",     // Bit6
        "机柜进水故障",           // Bit7
        "R相模块风扇故障",        // Bit8
        "S相模块风扇故障",        // Bit9
        "T相模块风扇故障",        // Bit10
        "24V IO供电故障",        // Bit11
        "24V通信供电故障",       // Bit12
        "24V控制供电故障",       // Bit13
        "主风机供电故障",         // Bit14
        "EMIF总线写入故障"       // Bit15
    }
};
static const AlarmMap_t g_J2DspFaultInfo4_Map =
{
    "J2DSP故障信息表4",
    {
        "EMIF总线读取故障",   // Bit0
        "FPGA心跳故障",      // Bit1
        "网络通信故障",       // Bit2
        "IMD板通信故障",     // Bit3
        "保留",             // Bit4
        "保留",             // Bit5
        "保留",             // Bit6
        "保留",             // Bit7
        "保留",             // Bit8
        "保留",             // Bit9
        "保留",             // Bit10
        "保留",             // Bit11
        "保留",             // Bit12
        "保留",             // Bit13
        "保留",             // Bit14
        "保留"              // Bit15
    }
};
static const AlarmMap_t g_J2FpgaFaultInfo1_Map =
{
    "J2FPGA故障信息表1",
    {
        "负母线过压",        // Bit0
        "正母线过压",        // Bit1
        "电池过压",          // Bit2
        "T相驱动板故障",     // Bit3
        "S相驱动板故障",     // Bit4
        "R相驱动板故障",     // Bit5
        "T相逐波限流故障",   // Bit6
        "S相逐波限流故障",   // Bit7
        "R相逐波限流故障",   // Bit8
        "电池过流",          // Bit9
        "T相过流",           // Bit10
        "S相过流",           // Bit11
        "R相过流",           // Bit12
        "R相过流",           // Bit13
        "保留",             // Bit14
        "保留"              // Bit15
    }
};
static const AlarmMap_t g_J2FpgaFaultInfo2_Map =
{
    "J2FPGA故障信息表2",
    {
        "管1(上管)短路故障",    // Bit0
        "管1(上管)电源欠压",    // Bit1
        "管2(下管)短路故障",    // Bit2
        "管2(下管)电源欠压",    // Bit3
        "管3短路故障",         // Bit4
        "管3电源欠压",         // Bit5
        "管4短路故障",         // Bit6
        "管4电源欠压",         // Bit7
        "管5短路故障",         // Bit8
        "管5电源欠压",         // Bit9
        "管6短路故障",         // Bit10
        "管6电源欠压",         // Bit11
        "上管门级检测",        // Bit12
        "下管门级检测",        // Bit13
        "过温故障",           // Bit14
        "窄脉冲"             // Bit15
    }
};

static const AlarmMap_t g_J2Fpga2FaultInfo1_Map =
{
    "J2FPGA2故障信息表1",
    {
        "光纤线缆异常故障",        // Bit0
        "光纤数据校验CRC错误故障", // Bit1
        "R相驱动解码异常故障",     // Bit2
        "S相驱动解码异常故障",     // Bit3
        "T相驱动解码异常故障",     // Bit4
        "R相NTC采样数据异常故障",  // Bit5
        "S相NTC采样数据异常故障",  // Bit6
        "T相NTC采样数据异常故障",  // Bit7
        "光纤检测到快速故障",      // Bit8
        "保留",                  // Bit9
        "保留",                  // Bit10
        "保留",                  // Bit11
        "保留",                  // Bit12
        "保留",                  // Bit13
        "保留",                  // Bit14
        "保留"                   // Bit15
    }
};
static const AlarmMap_t g_ArmCtrlDspCommFault1_Map =
{
    "ARM与控制DSP间通讯故障1",
    {
        "socket创建失败",      // Bit0
        "绑定端口失败",        // Bit1
        "监听端口失败",        // Bit2
        "接受客户端连接失败",   // Bit3
        "连接意外断开",        // Bit4
        "数据发送失败",        // Bit5
        "数据接收失败",        // Bit6
        "数据校验错误",        // Bit7
        "通信超时",           // Bit8
        "硬件故障",           // Bit9
        "保留",              // Bit10
        "保留",              // Bit11
        "保留",              // Bit12
        "保留",              // Bit13
        "保留",              // Bit14
        "保留"               // Bit15
    }
};
static const AlarmMap_t g_ArmSystemInternalFault_Map =
{
    "ARM系统内部故障(memory,file,device)",
    {
        "内存分配故障",      // Bit0
        "文件打开故障",      // Bit1
        "保留",            // Bit2
        "保留",            // Bit3
        "保留",            // Bit4
        "保留",            // Bit5
        "保留",            // Bit6
        "保留",            // Bit7
        "保留",            // Bit8
        "保留",            // Bit9
        "保留",            // Bit10
        "保留",            // Bit11
        "保留",            // Bit12
        "保留",            // Bit13
        "保留",            // Bit14
        "保留"             // Bit15
    }
};
static const AlarmMap_t g_SystemStatusWord_Map =
{
    "系统状态字",
    {
        "运行状态",        // Bit0 Running
        "手动停机状态",    // Bit1 Manual stop
        "故障停机状态",    // Bit2 Fault stop
        "急停状态",        // Bit3 Emergency stop
        "启动过程状态",    // Bit4 Start up
        "软件关机状态",    // Bit5 Soft shutdown
        "绝缘低状态",      // Bit6 Iso low
        "降额状态",        // Bit7 Derating
        "保留",           // Bit8
        "保留",           // Bit9
        "保留",           // Bit10
        "保留",           // Bit11
        "保留",           // Bit12
        "保留",           // Bit13
        "保留",           // Bit14
        "保留"            // Bit15
    }
};
static const AlarmMap_t g_ProcessStatusWord_Map =
{
    "过程状态字",
    {
        "DC预充过程状态",      // Bit0 DC precharge
        "DC合闸过程状态",      // Bit1 DC Closing
        "AC预同步过程状态",    // Bit2 AC presynchronization
        "AC合闸过程状态",      // Bit3 AC Closing
        "闭环状态",           // Bit4 Closed Loop
        "保留",              // Bit5
        "保留",              // Bit6
        "保留",              // Bit7
        "保留",              // Bit8
        "保留",              // Bit9
        "保留",              // Bit10
        "保留",              // Bit11
        "保留",              // Bit12
        "保留",              // Bit13
        "保留",              // Bit14
        "保留"               // Bit15
    }
};
/*
 * 需求示例：
 * - GET_INPUT(201), GET_INPUT(501), GET_INPUT(801) 复用同一张 SysF1 点表
 * - 202 对应 SysF2，203 对应 SysF3
 */
static AlarmInstance_t g_alarm_instances[] = {
    /* 复用 SysF1 点表的多个地址 */

    { 17061, &g_SystemStatus2_Map, 0,1 },
    { 17361, &g_SystemStatus2_Map, 0,3 },


    { 17062, &g_SystemStatus3_Map, 0,1 },
    { 17362, &g_SystemStatus3_Map, 0,3 },


    { 17064, &g_SystemMode1_Map, 0,1 },
    { 17364, &g_SystemMode1_Map, 0,3 },


    { 17066, &g_SysF1_Map, 0,1 },
    { 17366, &g_SysF1_Map, 0,3 },


    { 17067, &g_SysF2_Map, 0,1 },
    { 17367, &g_SysF2_Map, 0,3 },


    { 17068, &g_SysF3_Map, 0,1 },
    { 17368, &g_SysF3_Map, 0,3 },


    { 17070, &g_MidVolt_Fault1_Map, 0,1 },
    { 17370, &g_MidVolt_Fault1_Map, 0,3 },


    { 17071, &g_SystemWarn1_Map, 0,1 },
    { 17371, &g_SystemWarn1_Map, 0,3 },


    { 17072, &g_SystemWarn2_Map, 0,1 },
    { 17372, &g_SystemWarn2_Map, 0,3 },
   


    { 3100, &g_SysF1_Map, 0,2 },
    { 3700, &g_SysF1_Map, 0,4 },

    { 3101, &g_SysF2_Map, 0,2 },
    { 3701, &g_SysF2_Map, 0,4 },
    
    { 3102, &g_SysF3_Map, 0,2 },
    { 3702, &g_SysF3_Map, 0,4 },    

    { 3110, &g_SystemWarn1_Map, 0,2 },
    { 3710, &g_SystemWarn1_Map, 0,4 },   

    { 3111, &g_SystemWarn2_Map, 0,2 },
    { 3711, &g_SystemWarn2_Map, 0,4 }, 

    { 2817, &g_ControllerFault1_Map, 0,1 },
    { 3117, &g_ControllerFault1_Map, 0,2 },
    { 3417, &g_ControllerFault1_Map, 0,3 }, 
    { 3717, &g_ControllerFault1_Map, 0,4 },

    { 2818, &g_ControlWarn1_Map, 0,1 },
    { 3118, &g_ControlWarn1_Map, 0,2 },
    { 3418, &g_ControlWarn1_Map, 0,3 },
    { 3718, &g_ControlWarn1_Map, 0,4 }, 

    { 2819, &g_FPGA_Fault1_Map, 0,1 },
    { 3119, &g_FPGA_Fault1_Map, 0,2 },
    { 3419, &g_FPGA_Fault1_Map, 0,3 },
    { 3719, &g_FPGA_Fault1_Map, 0,4 }, 

    { 2820, &g_FPGA_Fault2_Map, 0,1 },
    { 3120, &g_FPGA_Fault2_Map, 0,2 },
    { 3420, &g_FPGA_Fault2_Map, 0,3 },
    { 3720, &g_FPGA_Fault2_Map, 0,4 }, 

    { 2821, &g_FPGA_Fault3_Map, 0,1 },
    { 3121, &g_FPGA_Fault3_Map, 0,2 },
    { 3421, &g_FPGA_Fault3_Map, 0,3 },
    { 3721, &g_FPGA_Fault3_Map, 0,4 }, 

    { 2822, &g_FPGA_Fault4_Map, 0,1 },
    { 3122, &g_FPGA_Fault4_Map, 0,2 },
    { 3422, &g_FPGA_Fault4_Map, 0,3 },
    { 3722, &g_FPGA_Fault4_Map, 0,4 }, 

    { 2823, &g_FPGA_Fault5_Map, 0,1 },
    { 3123, &g_FPGA_Fault5_Map, 0,2 },
    { 3423, &g_FPGA_Fault5_Map, 0,3 },
    { 3723, &g_FPGA_Fault5_Map, 0,4 }, 

    { 2824, &g_IO_Fault1_Map, 0,1 },
    { 3124, &g_IO_Fault1_Map, 0,2 },
    { 3424, &g_IO_Fault1_Map, 0,3 },
    { 3724, &g_IO_Fault1_Map, 0,4 },     

    { 2825, &g_IO_Fault2_Map, 0,1 },
    { 3125, &g_IO_Fault2_Map, 0,2 },
    { 3425, &g_IO_Fault2_Map, 0,3 },
    { 3725, &g_IO_Fault2_Map, 0,4 },    
    
    { 2827, &g_SampleFault1_Map, 0,1 },
    { 3127, &g_SampleFault1_Map, 0,2 },
    { 3427, &g_SampleFault1_Map, 0,3 },
    { 3727, &g_SampleFault1_Map, 0,4 },   

    { 2828, &g_SampleFault2_Map, 0,1 },
    { 3128, &g_SampleFault2_Map, 0,2 },
    { 3428, &g_SampleFault2_Map, 0,3 },
    { 3728, &g_SampleFault2_Map, 0,4 },  
    
    { 2829, &g_SampleFault3_Map, 0,1 },
    { 3129, &g_SampleFault3_Map, 0,2 },
    { 3429, &g_SampleFault3_Map, 0,3 },
    { 3729, &g_SampleFault3_Map, 0,4 },     

    { 2830, &g_CommFault1_Map, 0,1 },
    { 3130, &g_CommFault1_Map, 0,2 },
    { 3430, &g_CommFault1_Map, 0,3 },
    { 3730, &g_CommFault1_Map, 0,4 }, 


    { 2831, &g_ModuleWarn1_Map, 0,1 },
    { 3131, &g_ModuleWarn1_Map, 0,2 },
    { 3431, &g_ModuleWarn1_Map, 0,3 },
    { 3731, &g_ModuleWarn1_Map, 0,4 },  
    
    { 2832, &g_ModuleWarn2_Map, 0,1 },
    { 3132, &g_ModuleWarn2_Map, 0,2 },
    { 3432, &g_ModuleWarn2_Map, 0,3 },
    { 3732, &g_ModuleWarn2_Map, 0,4 },  


//////
    { 2833, &g_FPGA_Fault1_Map, 0,1 },
    { 3133, &g_FPGA_Fault1_Map, 0,2 },
    { 3433, &g_FPGA_Fault1_Map, 0,3 },
    { 3733, &g_FPGA_Fault1_Map, 0,4 }, 

    { 2834, &g_FPGA_Fault2_Map, 0,1 },
    { 3134, &g_FPGA_Fault2_Map, 0,2 },
    { 3434, &g_FPGA_Fault2_Map, 0,3 },
    { 3734, &g_FPGA_Fault2_Map, 0,4 }, 

    { 2835, &g_FPGA_Fault3_Map, 0,1 },
    { 3135, &g_FPGA_Fault3_Map, 0,2 },
    { 3435, &g_FPGA_Fault3_Map, 0,3 },
    { 3735, &g_FPGA_Fault3_Map, 0,4 }, 

    { 2836, &g_FPGA_Fault4_Map, 0,1 },
    { 3136, &g_FPGA_Fault4_Map, 0,2 },
    { 3436, &g_FPGA_Fault4_Map, 0,3 },
    { 3736, &g_FPGA_Fault4_Map, 0,4 }, 

    { 2837, &g_FPGA_Fault5_Map, 0,1 },
    { 3137, &g_FPGA_Fault5_Map, 0,2 },
    { 3437, &g_FPGA_Fault5_Map, 0,3 },
    { 3737, &g_FPGA_Fault5_Map, 0,4 }, 

    { 2838, &g_IO_Fault1_Map, 0,1 },
    { 3138, &g_IO_Fault1_Map, 0,2 },
    { 3438, &g_IO_Fault1_Map, 0,3 },
    { 3738, &g_IO_Fault1_Map, 0,4 },     

    { 2839, &g_IO_Fault2_Map, 0,1 },
    { 3139, &g_IO_Fault2_Map, 0,2 },
    { 3439, &g_IO_Fault2_Map, 0,3 },
    { 3739, &g_IO_Fault2_Map, 0,4 },    
    
    { 2841, &g_SampleFault1_Map, 0,1 },
    { 3141, &g_SampleFault1_Map, 0,2 },
    { 3441, &g_SampleFault1_Map, 0,3 },
    { 3741, &g_SampleFault1_Map, 0,4 },   

    { 2842, &g_SampleFault2_Map, 0,1 },
    { 3142, &g_SampleFault2_Map, 0,2 },
    { 3442, &g_SampleFault2_Map, 0,3 },
    { 3742, &g_SampleFault2_Map, 0,4 },  
    
    { 2843, &g_SampleFault3_Map, 0,1 },
    { 3143, &g_SampleFault3_Map, 0,2 },
    { 3443, &g_SampleFault3_Map, 0,3 },
    { 3743, &g_SampleFault3_Map, 0,4 },     

    { 2844, &g_CommFault1_Map, 0,1 },
    { 3144, &g_CommFault1_Map, 0,2 },
    { 3444, &g_CommFault1_Map, 0,3 },
    { 3744, &g_CommFault1_Map, 0,4 }, 


    { 2845, &g_ModuleWarn1_Map, 0,1 },
    { 3145, &g_ModuleWarn1_Map, 0,2 },
    { 3445, &g_ModuleWarn1_Map, 0,3 },
    { 3745, &g_ModuleWarn1_Map, 0,4 },  
    
    { 2846, &g_ModuleWarn2_Map, 0,1 },
    { 3146, &g_ModuleWarn2_Map, 0,2 },
    { 3446, &g_ModuleWarn2_Map, 0,3 },
    { 3746, &g_ModuleWarn2_Map, 0,4 },  

    
};

static AlarmInstance_t trina_g_alarm_instances[] = {

    { 2623, &g_MsCtrlDspFaultInfo1_Map, 0,1 },
    { 2923, &g_MsCtrlDspFaultInfo1_Map, 0,2 },
    { 3223, &g_MsCtrlDspFaultInfo1_Map, 0,3 }, 
    { 3523, &g_MsCtrlDspFaultInfo1_Map, 0,4 },

    { 2624, &g_MsCtrlDspFaultInfo2_Map, 0,1 },
    { 2924, &g_MsCtrlDspFaultInfo2_Map, 0,2 },
    { 3224, &g_MsCtrlDspFaultInfo2_Map, 0,3 },
    { 3524, &g_MsCtrlDspFaultInfo2_Map, 0,4 }, 

    { 2625, &g_MsCtrlDspFaultInfo3_Map, 0,1 },
    { 2925, &g_MsCtrlDspFaultInfo3_Map, 0,2 },
    { 3225, &g_MsCtrlDspFaultInfo3_Map, 0,3 },
    { 3525, &g_MsCtrlDspFaultInfo3_Map, 0,4 }, 

    { 2626, &g_MsCtrlDspFaultInfo4_Map, 0,1 },
    { 2926, &g_MsCtrlDspFaultInfo4_Map, 0,2 },
    { 3226, &g_MsCtrlDspFaultInfo4_Map, 0,3 },
    { 3526, &g_MsCtrlDspFaultInfo4_Map, 0,4 }, 

    { 2631, &g_MsCtrlFpgaFaultInfo1_Map, 0,1 },
    { 2931, &g_MsCtrlFpgaFaultInfo1_Map, 0,2 },
    { 3231, &g_MsCtrlFpgaFaultInfo1_Map, 0,3 },
    { 3531, &g_MsCtrlFpgaFaultInfo1_Map, 0,4 }, 

    { 2650, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2950, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3250, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3550, &g_ArmCtrlDspCommFault1_Map, 0,4 }, 

    { 2651, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2951, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3251, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3551, &g_ArmCtrlDspCommFault1_Map, 0,4 }, 


    { 2652, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2952, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3252, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3552, &g_ArmCtrlDspCommFault1_Map, 0,4 }, 
   
    { 2653, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2953, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3253, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3553, &g_ArmCtrlDspCommFault1_Map, 0,4 }, 

    { 2654, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2954, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3254, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3554, &g_ArmCtrlDspCommFault1_Map, 0,4 },    
    
    { 2655, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2955, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3255, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3555, &g_ArmCtrlDspCommFault1_Map, 0,4 },  

    { 2656, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2956, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3256, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3556, &g_ArmCtrlDspCommFault1_Map, 0,4 },  

    { 2657, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2957, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3257, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3557, &g_ArmCtrlDspCommFault1_Map, 0,4 },  
    
    { 2658, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2958, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3258, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3558, &g_ArmCtrlDspCommFault1_Map, 0,4 },  

    { 2659, &g_ArmCtrlDspCommFault1_Map, 0,1 },
    { 2959, &g_ArmCtrlDspCommFault1_Map, 0,2 },
    { 3259, &g_ArmCtrlDspCommFault1_Map, 0,3 },
    { 3559, &g_ArmCtrlDspCommFault1_Map, 0,4 },  

    { 2660, &g_ArmSystemInternalFault_Map, 0,1 },
    { 2960, &g_ArmSystemInternalFault_Map, 0,2 },
    { 3260, &g_ArmSystemInternalFault_Map, 0,3 },
    { 3560, &g_ArmSystemInternalFault_Map, 0,4 },  

    { 2752, &g_J1DspFaultInfo1_Map, 0,1 },
    { 3052, &g_J1DspFaultInfo1_Map, 0,2 },
    { 3352, &g_J1DspFaultInfo1_Map, 0,3 },
    { 3652, &g_J1DspFaultInfo1_Map, 0,4 }, 

    { 2753, &g_J1DspFaultInfo2_Map, 0,1 },
    { 3053, &g_J1DspFaultInfo2_Map, 0,2 },
    { 3353, &g_J1DspFaultInfo2_Map, 0,3 },
    { 3653, &g_J1DspFaultInfo2_Map, 0,4 }, 

    { 2754, &g_J1DspFaultInfo3_Map, 0,1 },
    { 3054, &g_J1DspFaultInfo3_Map, 0,2 },
    { 3354, &g_J1DspFaultInfo3_Map, 0,3 },
    { 3654, &g_J1DspFaultInfo3_Map, 0,4 },     

    { 2755, &g_J1DspFaultInfo4_Map, 0,1 },
    { 3055, &g_J1DspFaultInfo4_Map, 0,2 },
    { 3355, &g_J1DspFaultInfo4_Map, 0,3 },
    { 3655, &g_J1DspFaultInfo4_Map, 0,4 }, 

    { 2756, &g_J1FpgaFaultInfo1_Map, 0,1 },
    { 3056, &g_J1FpgaFaultInfo1_Map, 0,2 },
    { 3356, &g_J1FpgaFaultInfo1_Map, 0,3 },
    { 3656, &g_J1FpgaFaultInfo1_Map, 0,4 }, 

    { 2757, &g_J1FpgaFaultInfo2_Map, 0,1 },
    { 3057, &g_J1FpgaFaultInfo2_Map, 0,2 },
    { 3357, &g_J1FpgaFaultInfo2_Map, 0,3 },
    { 3657, &g_J1FpgaFaultInfo2_Map, 0,4 },    
    
    { 2758, &g_J1FpgaFaultInfo3_Map, 0,1 },
    { 3058, &g_J1FpgaFaultInfo3_Map, 0,2 },
    { 3358, &g_J1FpgaFaultInfo3_Map, 0,3 },
    { 3658, &g_J1FpgaFaultInfo3_Map, 0,4 },  

    { 2759, &g_J1FpgaFaultInfo4_Map, 0,1 },
    { 3059, &g_J1FpgaFaultInfo4_Map, 0,2 },
    { 3359, &g_J1FpgaFaultInfo4_Map, 0,3 },
    { 3659, &g_J1FpgaFaultInfo4_Map, 0,4 }, 

    { 2760, &g_J1Fpga2FaultInfo1_Map, 0,1 },
    { 3060, &g_J1Fpga2FaultInfo1_Map, 0,2 },
    { 3360, &g_J1Fpga2FaultInfo1_Map, 0,3 },
    { 3660, &g_J1Fpga2FaultInfo1_Map, 0,4 },       
    
    { 2700, &g_SystemStatusWord_Map, 0,1 },
    { 3000, &g_SystemStatusWord_Map, 0,2 },
    { 3300, &g_SystemStatusWord_Map, 0,3 },
    { 3600, &g_SystemStatusWord_Map, 0,4 },   
    
    { 2701, &g_ProcessStatusWord_Map, 0,1 },
    { 3001, &g_ProcessStatusWord_Map, 0,2 },
    { 3301, &g_ProcessStatusWord_Map, 0,3 },
    { 3601, &g_ProcessStatusWord_Map, 0,4 }, 
};

void Alarm_ParseAndLog_All(void)
{
    LOG_INFO("台达PCS故障日志线程已创建！");
    while(1)
    {
         for (size_t i = 0; i < (sizeof(g_alarm_instances) / sizeof(g_alarm_instances[0])); i++) {
        uint16_t alarm_value = GET_INPUT(g_alarm_instances[i].addr);
        Alarm_Process(&g_alarm_instances[i], alarm_value);
    }   

        usleep(50*1000);
    }


}
void Trina_Alarm_ParseAndLog_All(void)
{
    LOG_INFO("天合PCS故障日志线程已创建！");
    while(1)
    {
         for (size_t i = 0; i < (sizeof(trina_g_alarm_instances) / sizeof(trina_g_alarm_instances[0])); i++) {
        uint16_t alarm_value = GET_INPUT(trina_g_alarm_instances[i].addr);
        Alarm_Process(&trina_g_alarm_instances[i], alarm_value);
    }   

        usleep(50*1000);
    }


}
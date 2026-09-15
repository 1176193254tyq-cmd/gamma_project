#include "log_printf.h"

char bms_lv1_alarm[32][64]=
{
        "簇充电电流过大",         // bit0
        "簇放电电流过大",         // bit1
        "簇总电压过压",           // bit2
        "簇总电压欠压",           // bit3
        "SOC低",                  // bit4
        "SOH低",                  // bit5
        "绝缘阻值低",             // bit6
        "单体过压",               // bit7
        "单体欠压",               // bit8
        "单体过温",               // bit9
        "单体低温",               // bit10
        "单体瞬时温升",           // bit11
        "簇内单体温差过大",       // bit12
        "簇内单体压差过大",       // bit13
        "簇间压差过大",           // bit14
        "簇间电流差值过大",       // bit15
        "Pack模组过压",           // bit0
        "Pack模组欠压",           // bit1
        "Pakc模组内单体温差过大", // bit2
        "工作电池簇就为数量不足", // bit3
        "Pack极柱温度过高",       // bit4
        "高压箱连接器温度过高",   // bit5
        "SOC高",                // bit6
        "预留",                  // bit7
        "预留",                   // bit8
        "预留",                   // bit9
        "预留",                   // bit10
        "预留",                   // bit11
        "预留",                   // bit12
        "预留",                   // bit13
        "预留",                   // bit14
        "预留",                   // bit15
};

char bms_lv2_alarm[32][64]=
{
        "簇充电电流过大",         // bit0
        "簇放电电流过大",         // bit1
        "簇总电压过压",           // bit2
        "簇总电压欠压",           // bit3
        "SOC低",                  // bit4
        "SOH低",                  // bit5
        "绝缘阻值低",             // bit6
        "单体过压",               // bit7
        "单体欠压",               // bit8
        "单体过温",               // bit9
        "单体低温",               // bit10
        "单体瞬时温升",           // bit11
        "簇内单体温差过大",       // bit12
        "簇内单体压差过大",       // bit13
        "簇间压差过大",           // bit14
        "预留",                   // bit15
        "Pack模组过压",           // bit0
        "Pack模组欠压",           // bit1
        "Pakc模组内单体温差过大", // bit2
        "簇间电流差值过大",       // bit3
        "高压箱过温",             // bit4
        "Pack极柱温度过高",       // bit5
        "高压箱连接器温度过高",   // bit6
        "SOC高",                  // bit7
        "预留",                   // bit8
        "预留",                   // bit9
        "预留",                   // bit10
        "预留",                   // bit11
        "预留",                   // bit12
        "预留",                   // bit13
        "预留",                   // bit14
        "预留",                   // bit15
};

char bms_lv3_alarm[32][64]=
{
        "簇充电电流过大",         // bit0
        "簇放电电流过大",         // bit1
        "簇总电压过压",           // bit2
        "簇总电压欠压",           // bit3
        "SOC低",                  // bit4
        "SOC高",                  // bit5
        "绝缘阻值低",             // bit6
        "单体过压",               // bit7
        "单体欠压",               // bit8
        "单体过温",               // bit9
        "单体低温",               // bit10
        "单体瞬时温升",           // bit11
        "簇内单体温差过大",       // bit12
        "簇内单体压差过大",       // bit13
        "簇间压差过大",           // bit14
        "预留",                   // bit15
        "pack模组过压",           // bit0
        "pack模组欠压",           // bit1
        "pack模组内单体温差过大", // bit2
        "BCU内部有BMU通讯",       // bit3
        "BCU内部有BAU通讯",       // bit4
        "电池簇接触器故障",       // bit5
        "BMU内部温度传感器",      // bit6
        "BCU内部电流传感器",      // bit7
        "BAU与DIDO通讯",          // bit8
        "BAU与PCS通信",           // bit9
        "BAU与EMS/SACADA通讯",    // bit10
        "簇间电流不平衡",         // bit11
        "电池簇接触器黏连",       // bit12
        "预充故障",               // bit13
        "整机故障",               // bit14
        "检测到消防报警",         // bit15
};
char bms_lv4_alarm[32][64]=
{
        "Pack极柱温度过高",     // bit0
        "高压箱连接器温度过高",  // bit1
        "SOC高",                // bit2
        "预留",                 // bit3
        "预留",                 // bit4
        "预留",                 // bit5
        "预留",                 // bit6
        "预留",                 // bit7
        "预留",                 // bit8
        "预留",                 // bit9
        "预留",                 // bit10
        "预留",                 // bit11
        "预留",                 // bit12
        "预留",                 // bit13
        "预留",                 // bit14
        "预留",                 // bit15
        "预留",                 // bit0
        "预留",                 // bit1
        "预留",                 // bit2
        "预留",                 // bit3
        "预留",                 // bit4
        "预留",                 // bit5
        "预留",                 // bit6
        "预留",                 // bit7
        "预留",                 // bit8
        "预留",                 // bit9
        "预留",                 // bit10
        "预留",                 // bit11
        "预留",                 // bit12
        "预留",                 // bit13
        "预留",                 // bit14
        "预留",                 // bit15
};

char pcs_kehua_alarm[16*MAX_GROUP_NUM][64]=
{
    //PCS-7200  LC-5127
    "绝缘阻抗异常",            //bit0
    "漏电流异常",              //bit1
    "直流过压告",              //bit2
    "电网过压异常",            //bit3
    "电网欠压异常",            //bit4
    "电网过频异常",            //bit5
    "电网欠频异常",            //bit6
    "预留",                   //bit7
    "电网相序异常",            //bit8
    "预留",                   //bit9
    "预留",                   //bit10
    "直流隔离开关异常",        //bit11
    "交流风机异常",            //bit12
    "交流主开关异常",          //bit13
    "预留"                    //bit14
    "预留",                   //bit15
    //PCS-7201  LC-5128    
    "预留",                   //bit0
    "预留",                   //bit1
    "热交换机故障",            //bit2
    "交流防雷器异常",          //bit3
    "内部急停故障",            //bit4
    "预留",                   //bit5
    "母线电压不符合开机条件",   //bit6
    "预留",                   //bit7
    "预留",                   //bit8
    "门禁告警",               //bit9
    "锁相异常",               //bit10
    "直流防雷器异常",         //bit11
    "预留",                  //bit12
    "预留",                  //bit13
    "预留",                  //bit14
    "ID冲突",                //bit15
    //PCS-7202  LC-5129 
    "预留",                  //bit0
    "预留",                  //bit1
    "预留",                  //bit2
    "预留",                  //bit3
    "预留",                  //bit4
    "市电不平衡",            //bit5
    "烟雾告警",              //bit6
    "并机CAN通讯异常",       //bit7
    "HMI CAN通讯异常",       //bit8
    "机型设置错误",          //bit9
    "HMI 485通讯异常",       //bit10
    "预留",                  //bit11
    "预留",                  //bit12
    "预留",                  //bit13
    "预留",                  //bit14
    "预留",                  //bit15
    //PCS-7203  LC-5130
    "预留",                  //bit0
    "低电压穿越运行",         //bit1
    "高电压穿越运行",         //bit2
    "直流风机异常",           //bit3
    "预留",                  //bit4
    "预留",                  //bit5
    "预留",                  //bit6
    "预留",                  //bit7
    "正极接地异常",           //bit8
    "负极接地异常",           //bit9
    "交流接地异常",           //bit10
    "并网接地异常",           //bit11
    "电感风机异常",           //bit12
    "预留",                  //bit13
    "预留",                  //bit14
    "预留",                  //bit15
    //PCS-7204  LC-5131
    "电池过压",               //bit0
    "电池欠压",               //bit1
    "直流过流",               //bit2
    "输出电压异常",           //bit3
    "输出电压不符合离网条件",  //bit4
    "过载保护",               //bit5
    "短路保护",               //bit6
    "预留",                   //bit7
    "预留",                   //bit8
    "电池重载欠压",            //bit9
    "电池低压告警",            //bit10
    "预留",                   //bit11
    "电池反接",               //bit12
    "电池电压不符合充电条件",   //bit13
    "过载告警",                //bit14
    "预留",                    //bit15    
    //PCS-7205  LC-5132
    "BMS通讯故障",             //bit0
    "BMS通讯异常",             //bit1
    "BMS干结点异常",           //bit2
    "BMS禁充",                //bit3
    "BMS禁放",                //bit4
    "BMS待机",                //bit5
    "BMS告警",                //bit6
    "预留",                   //bit7
    "预留",                   //bit8
    "预留",                   //bit9
    "预留",                   //bit10
    "预留",                   //bit11
    "预留",                   //bit12
    "预留",                   //bit13
    "预留",                   //bit14
    "预留",                   //bit15   
    //PCS-7206  LC-5133
    "防PID模块异常",          //bit0
    "相位同步异常",           //bit1
    "直流路数配置异常",       //bit2
    "防孤岛异常",            //bit3
    "预留",                  //bit4
    "预留",                  //bit5
    "接地回路异常",          //bit6
    "中点接触器异常",        //bit7
    "交流缓冲异常",          //bit8
    "预留",                 //bit9
    "系统过温",             //bit10
    "系统过湿",             //bit11
    "PV极性反接",           //bit12
    "箱变超温",             //bit13
};

char pcs_kehua_fault[16*MAX_GROUP_NUM][64]=
{
    //PCS-7200  LC-5137
    "预留",                 //bit0
    "预留",                 //bit1
    "预留",                 //bit2
    "预留",                 //bit3
    "预留",                 //bit4
    "预留",                 //bit5
    "预留",                 //bit6
    "功率模块过温",          //bit7
    "预留",                 //bit8
    "逆变软件过流",          //bit9
    "直流软启动异常",        //bit10
    "预留",                 //bit11
    "预留",                 //bit12
    "预留",                 //bit13
    "预留"                  //bit14
    "内部异常",             //bit15
    //PCS-7201  LC-5138    
    "机内过温",             //bit0
    "交流软启动异常",       //bit1
    "预留",                //bit2
    "预留",                //bit3
    "预留",                //bit4
    "外部急停故障",         //bit5
    "预留",                //bit6
    "母线电流过流",         //bit7
    "预留",                //bit8
    "预留",                //bit9
    "预留",                //bit10
    "预留",                //bit11
    "预留",                //bit12
    "逆变硬件过流",         //bit13
    "驱动故障",             //bit14
    "预留",                //bit15
    //PCS-7202  LC-5139 
    "预留",                //bit0
    "预留",                //bit1
    "预留",                //bit2
    "预留",                //bit3
    "预留",                //bit4
    "预留",                //bit5
    "预留",                //bit6
    "预留",                //bit7
    "预留",                //bit8
    "预留",                //bit9
    "预留",                //bit10
    "远程通讯故障",         //bit11
    "预留",                //bit12
    "预留",                //bit13
    "预留",                //bit14
    "预留",                //bit15
    //PCS-7203  LC-5140
    "预留",                //bit0
    "预留",                //bit1
    "预留",                //bit2
    "预留",                //bit3
    "散热器温度开关异常",   //bit4
    "外部温度开关异常",     //bit5
    "辅助变压器温度开关异常",  //bit6
    "电感温度开关异常",     //bit7
    "预留",                //bit8
    "预留",                //bit9
    "预留",                //bit10
    "预留",                //bit11
    "预留",                //bit12
    "预留",                //bit13
    "预留",                //bit14
    "预留",                //bit15
    //PCS-7204  LC-5141
    "预留",                //bit0
    "预留",                //bit1
    "预留",                //bit2
    "预留",                //bit3
    "预留",                //bit4
    "预留",                //bit5
    "预留",                //bit6
    "预留",                //bit7
    "直流保险丝异常",       //bit8
    "预留",                //bit9
    "预留",                //bit10
    "预留",                //bit11
    "预留",                //bit12
    "预留",                //bit13
    "预留",                //bit14
    "预留",                //bit15    
    //PCS-7205  LC-5142
    "预留",                //bit0
    "预留",                //bit1
    "预留",                //bit2
    "预留",                //bit3
    "预留",                //bit4
    "预留",                //bit5
    "预留",                //bit6
    "预留",                //bit7
    "预留",                //bit8
    "预留",                //bit9
    "预留",                //bit10
    "预留",                //bit11
    "预留",                //bit12
    "预留",                //bit13
    "预留",                //bit14
    "预留",                //bit15   
    //PCS-7206  LC-5143
    "预留",                //bit0
    "预留",                //bit1
    "预留",                //bit2
    "预留",                //bit3
    "预留",                //bit4
    "预留",                //bit5
    "预留",                //bit6
    "预留",                //bit7
    "预留",                //bit8
    "预留",                //bit9
    "预留",                //bit10
    "预留",                //bit11
    "预留",                //bit12
    "预留",                //bit13
    "预留",                //bit14
    "预留",                //bit15 
};


char pcs_trina_alarm[16*MAX_GROUP_NUM][64]=
{
    //PCS-3427  LC-5127
    "温度异常",            //bit0
    "预留",                    //bit1
    "GFRT运行",           //bit2
    "预留",                    //bit3
    "直流熔丝异常",        //bit4
    "BMS电池告警",         //bit5
    "直流传感器异常",      //bit6
    "预留",                    //bit7
    "预留",                    //bit8
    "充电条件不满足",      //bit9
    "放电条件不满足",      //bit10
    "预留",                    //bit11
    "直流开关异常",        //bit12
    "风机异常",            //bit13
    "预留",                    //bit14
    "预留",                    //bit15    
    //PCS-3428  LC-5128
    "支路板通讯异常",      //bit0
    "直流开关异常",        //bit1
    "预留",                    //bit2
    "交流接触器异常",      //bit3
    "消防干节点异常停机",   //bit4
    "BMS干节点异常停机",   //bit5
    "预留",                    //bit6
    "预留",                    //bit7
    "风扇通信异常",        //bit8
    "ISO检测仪通信异常",   //bit9
    "预留",                    //bit10
    "预留",                    //bit11
    "预留",                    //bit12
    "预留",                    //bit13
    "预留",                    //bit14
    "预留",                    //bit15
};

char pcs_trina_fault[16*MAX_GROUP_NUM][64]=
{
    //PCS-3429  LC-5137
    "直流欠压",            //bit0
    "直流过压",            //bit1
    "交流欠压",            //bit2
    "交流过压",            //bit3
    "交流欠频",            //bit4
    "交流过频",            //bit5
    "交流接触器故障",      //bit6
    "孤岛保护",            //bit7
    "直流开关过温",        //bit8
    "PDP保护（模块故障）", //bit9
    "模块过温",            //bit10
    "电抗器过温",          //bit11
    "变压器过温",          //bit12
    "漏电流保护",          //bit13
    "交流电容温度过高",    //bit14
    "过载保护",            //bit15
    //PCS-3430  LC-5138
    "直流熔丝温度过高",    //bit0
    "风机故障",            //bit1
    "直流熔断器故障",      //bit2
    "预留",                    //bit3
    "直流过流",            //bit4
    "交流过流",            //bit5
    "预留",                    //bit6
    "环境异常",            //bit7
    "硬件故障",            //bit8
    "预留",                    //bit9
    "预留",                    //bit10
    "预留",                    //bit11
    "预留",                    //bit12
    "预留",                    //bit13
    "预留",                    //bit14
    "预留",                    //bit15
    //PCS-3431  LC-5139
    "绝缘阻抗故障",        //bit0
    "交流防雷故障",        //bit1
    "采样故障",            //bit2
    "电池极性反接",        //bit3
    "电池电压过高",        //bit4
    "计量板通讯异常",       //bit5
    "交流电流不平衡1",      //bit6
    "主机故障",            //bit7
    "直流防雷故障",        //bit8
    "直流软启故障",        //bit9
    "直流分量故障",        //bit10
    "直流开关故障",        //bit11
    "机器码重复故障",      //bit12
    "并机通讯故障",        //bit13
    "控制柜温度故障",      //bit14
    "电池电压过低",        //bit15  
    //PCS-3432  LC-5140
    "交流电压不平衡",      //bit0
    "电池通讯故障",        //bit1
    "BMS电池故障",         //bit2
    "交流开关故障",        //bit3
    "交流软启故障",        //bit4
    "直流电压采用故障",    //bit5
    "预留",                    //bit6
    "交流不平衡2",         //bit7
    "交流不平衡3",         //bit8
    "驱动板故障",          //bit9
    "中点电网偏移",        //bit10
    "载波同步故障",        //bit11
    "相序错误",            //bit12
    "预留",                    //bit13
    "预留",                    //bit14
    "预留",                    //bit15      

};



// void BMS_Warn_Printf(INT16U lv_lbit,INT16U lv_hbit,INT8U bms_num,INT8U level,char generated_buf[][64],char removal_buf[][64])
void BMS_Warn_Printf(INT16U lv_lbit,INT16U lv_hbit,INT8U bms_num,INT8U level)
{
    static INT32U lv_fualt[MAX_BMS_NUM][3]={0};
    INT32U lv_bit = 0;
    INT32U resault = 0;
    lv_bit=(lv_hbit<<16)|(lv_lbit<<0);
    resault=lv_fualt[bms_num][level]^lv_bit;
    if (resault)
    {
        for (INT8U i=0;i<32;i++)
        {
            if ((resault>>i) & 0x01)
            {
                if((lv_bit>>i) & 0x01)
                {
                    switch (level)
                    {
                        case 0:
                            LOG_INFO("BMS-%d %s-一级告警",bms_num,bms_lv1_alarm[i]);
                            break;
                        case 1:
                            LOG_INFO("BMS-%d %s-二级告警",bms_num,bms_lv2_alarm[i]);
                            break;
                        case 2:
                            LOG_INFO("BMS-%d %s-三级告警",bms_num,bms_lv3_alarm[i]);
                            break;        
                        case 3:
                            LOG_INFO("BMS-%d %s-三级告警",bms_num,bms_lv4_alarm[i]);
                            break;                                                                   
                        default:
                            break;
                    }
                }
                else
                {
                    switch (level)
                    {
                        case 0:
                            LOG_INFO("BMS-%d %s-告警恢复",bms_num,bms_lv1_alarm[i]);
                            break;
                        case 1:
                            LOG_INFO("BMS-%d %s-告警恢复",bms_num,bms_lv1_alarm[i]);
                            break;
                        case 2:
                            LOG_INFO("BMS-%d %s-告警恢复",bms_num,bms_lv3_alarm[i]);
                            break;  
                        case 3:
                            LOG_INFO("BMS-%d %s-告警恢复",bms_num,bms_lv4_alarm[i]);
                            break;                                                                         
                        default:
                            break;
                    }
                }
            }
        }
        lv_fualt[bms_num][level]=lv_bit;
    }
}

void PCS_KEHUA_Warn_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level)
{
    static INT16U lv_fualt[MAX_PCS_NUM][10]={0};
    INT16U resault = 0;
    resault=lv_fualt[pcs_num][level]^lv_bit;
    if (resault)
    {
        for (INT8U i=0;i<16;i++)
        {
            if ((resault>>i) & 0x01)
            {
                if((lv_bit>>i) & 0x01)
                {   
                    LOG_INFO("PCS-%d %s-告警",pcs_num,pcs_kehua_alarm[level*16+i]);
                }
                else
                {
                    LOG_INFO("PCS-%d %s-恢复",pcs_num,pcs_kehua_alarm[level*16+i]);
                }
            }
        }
        lv_fualt[pcs_num][level]=lv_bit;
    }
}

void PCS_KEHUA_Fault_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level)
{
    static INT16U lv_fualt[MAX_PCS_NUM][10]={0};
    INT16U resault = 0;
    resault=lv_fualt[pcs_num][level]^lv_bit;
    if (resault)
    {
        for (INT8U i=0;i<16;i++)
        {
            if ((resault>>i) & 0x01)
            {
                if((lv_bit>>i) & 0x01)
                {   
                    LOG_INFO("PCS-%d %s-故障",pcs_num,pcs_kehua_fault[level*16+i]);
                }
                else
                {
                    LOG_INFO("PCS-%d %s-恢复",pcs_num,pcs_kehua_fault[level*16+i]);
                }
            }
        }
        lv_fualt[pcs_num][level]=lv_bit;
    }
}

void PCS_TRINA_Warn_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level)
{
    static INT16U lv_fualt[MAX_PCS_NUM][10]={0};
    INT16U resault = 0;
    resault=lv_fualt[pcs_num][level]^lv_bit;
    if (resault)
    {
        for (INT8U i=0;i<16;i++)
        {
            if ((resault>>i) & 0x01)
            {
                if((lv_bit>>i) & 0x01)
                {   
                    LOG_INFO("PCS-%d %s-告警",pcs_num,pcs_trina_alarm[level*16+i]);
                }
                else
                {
                    LOG_INFO("PCS-%d %s-恢复",pcs_num,pcs_trina_alarm[level*16+i]);
                }
            }
        }
        lv_fualt[pcs_num][level]=lv_bit;
    }
}

void PCS_TRINA_Fault_Printf(INT16U lv_bit,INT8U pcs_num,INT8U level)
{
    static INT16U lv_fualt[MAX_PCS_NUM][10]={0};
    INT16U resault = 0;
    resault=lv_fualt[pcs_num][level]^lv_bit;
    if (resault)
    {
        for (INT8U i=0;i<16;i++)
        {
            if ((resault>>i) & 0x01)
            {
                if((lv_bit>>i) & 0x01)
                {   
                    LOG_INFO("PCS-%d %s-告警",pcs_num,pcs_trina_fault[level*16+i]);
                }
                else
                {
                    LOG_INFO("PCS-%d %s-恢复",pcs_num,pcs_trina_fault[level*16+i]);
                }
            }
        }
        lv_fualt[pcs_num][level]=lv_bit;
    }
}


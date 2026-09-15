/* ------------------------------------------------------------
 * Copyright (C) 2019
 * File Name : app_config.c
 * Project :
 * Description : ini文件配置内容读写，本进程的配置
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 修改宏定义，调整配置文件读取结构
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改解析的内容 - server改成scada
 * ------------------------------------------------------------
 * HISTORY : 2022/10/26 修改解析的内容
 */

#include "main.h"
#include "ini.h"
#include "app_config.h"

/* 配置变量 */
static sysPara sysConf = {0};

/* 默认配置文件名 */
#define MATCH_SECTION(s)  (strcmp(section, s) == 0)                         //匹配段名宏
#define MATCH(s, n)  ((strcmp(section, s) == 0) && (strcmp(name, n) == 0))  //匹配段名和项名宏
//匹配字符串制作宏
#define MATCH_SPRINT(match_str, format, ...)    do {    \
    memset(match_str, 0, sizeof(match_str));            \
    sprintf(match_str, format, ##__VA_ARGS__);          \
}while(0)
//IP拷贝制作宏
#define IP_COPY(ip_str, value, size)    do {    \
    memset(ip_str, 0, MAX_IP_LEN);              \
    memcpy(ip_str, value, size);                \
}while(0)


/**
 * @brief 时段设置
 * @param[in] str - 时段字符串
 * @param[out] period - 需要赋值的时段
 * @return -1-赋值失败; 0-赋值成功
 */
int fill_period(const char* str, period* p_period)
{
    INT16U bh;
    INT16U bm;
    INT16U eh;
    INT16U em;
    int active_power;
    int reactive_power;
    int ret = 0;
    ret = sscanf(str, "%hd:%hd:%hd:%hd:%d:%d", &bh, &bm, &eh, &em, &active_power, &reactive_power);
    if (ret != 6) {
        return -1;
    }

    p_period->bh = bh;
    p_period->bm = bm;
    p_period->eh = eh;
    p_period->em = em;
    p_period->active_power = active_power;
    p_period->reactive_power = reactive_power;

    return 0;
}

/**
 * @brief 系统配置内容解析
 * @param[in] pconfig 配置表
 * @param[in] section 配置段标识
 * @param[in] name 配置项名称
 * @param[in] value 配置项数值
 * @return 结果，-1错误，0未匹配，1匹配成功
 */
static int System_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    int match_res = 1;
    //本地控制器从地址
    if (MATCH("SYSTEM", "lc_slave_addr")) {
        pconfig->lc_slave_addr = atoi(value);
    } 
    //子系统个数
    else if (MATCH("SYSTEM", "subNum")) {
        pconfig->subNum = atoi(value);
    }
    //子系统类型
    else if (MATCH("SYSTEM", "subSysType")) {
        pconfig->subSysType = atoi(value);
    }
    //PCS个数
    else if (MATCH("SYSTEM", "pcsNum")) {
        pconfig->pcsNum = atoi(value);
    }
    //BMS个数
    else if (MATCH("SYSTEM", "bmsNum")) {
        pconfig->bmsNum = atoi(value);
    }
    //系统个数
    else if (MATCH("SYSTEM", "sysNum")) {
        pconfig->sysNum = atoi(value);
    }
    //BMS客户端台数
    else if (MATCH("SYSTEM", "bmsClientNum")) {
        pconfig->bmsClientNum = atoi(value);
    }
    //E1210(DI)个数
    else if (MATCH("SYSTEM", "diNum")) {
        pconfig->diNum = atoi(value);
    }
    //E1214(DIDO)个数
    else if (MATCH("SYSTEM", "didoNum")) {
        pconfig->didoNum = atoi(value);
    }
    //U8410自带串口个数
    else if (MATCH("SYSTEM", "serialNum")) {
        pconfig->serialNum = atoi(value);
    }
    //以太网转485口个数
    else if (MATCH("SYSTEM", "eth485_portNum")) {
        pconfig->eth485_portNum = atoi(value);
    }
    //运行模式
    else if (MATCH("SYSTEM", "RunMode")) {
        pconfig->run_mode = atoi(value);
    }
    //Gateway个数
    else if (MATCH("SYSTEM", "gwNum")) {
        pconfig->gwNum = atoi(value);
    }
    //AC meter个数
    else if (MATCH("SYSTEM", "ACmeterNum")) {
        pconfig->ACmeterNum = atoi(value);
    }
    //AC meterCT个数
    else if (MATCH("SYSTEM", "ACmeterCT")) {
        pconfig->ACmeterCT = atoi(value);
    }
    //DC meter个数
    else if (MATCH("SYSTEM", "DCmeterNum")) {
        pconfig->DCmeterNum = atoi(value);
    }
    //空调个数
    else if (MATCH("SYSTEM", "airNum")) {
        pconfig->airNum = atoi(value);
    }
    //温湿度传感器个数
    else if (MATCH("SYSTEM", "tempNum")) {
        pconfig->tempNum = atoi(value);
    }
    //UPS个数
    else if (MATCH("SYSTEM", "upsNum")) {
        pconfig->upsNum = atoi(value);
    }
    //测控装置个数
    else if (MATCH("SYSTEM", "measuNum")) {
        pconfig->measuNum = atoi(value);
    }
    //气体传感器个数
    else if (MATCH("SYSTEM", "gasNum")) {
        pconfig->gasNum = atoi(value);
    }
    //消防控制器或消防&可燃气体控制器数量
    else if (MATCH("SYSTEM", "fireNum")) {
        pconfig->fireNum = atoi(value);
    }
    //消防探头的数量
    else if (MATCH("SYSTEM", "sensorNum")) {
        pconfig->sensorNum = atoi(value);
    }
    //di输入信号个数
    else if (MATCH("SYSTEM", "diSigNum")) {
        pconfig->diSigNum = atoi(value);
    }
    //do输出信号个数
    else if (MATCH("SYSTEM", "doSigNum")) {
        pconfig->doSigNum = atoi(value);
    }
    //20230926无线测温模块数量
    else if (MATCH("SYSTEM", "wirelessTempNum")) {
        pconfig->wirelessTempNum = atoi(value);
    }    
    //20230801新增调试标志
    else if (MATCH("SYSTEM", "debugFlag")) {
        pconfig->debugFlag = atoi(value);
    }       
    //202309114新增绝缘电阻日志记录标志
    else if (MATCH("SYSTEM", "zlogFlag")) {
        pconfig->zlogFlag = atoi(value);
    }   
        //EMS服务器使能
    else if(MATCH("SYSTEM", "emsEnable"))
       {
            pconfig->emsEnable = atoi(value);
        }
        //EMSP1服务器port值
    else if(MATCH("SYSTEM", "localEmsPort"))
        {
            pconfig->localEmsPort = atoi(value);
        }
        //EMSP1服务器使能
    else if(MATCH("SYSTEM", "emsP1Enable"))
        {
            pconfig->emsP1Enable = atoi(value);
        }
        //EMS服务器port值
    else if(MATCH("SYSTEM", "localEmsP1Port"))
        {
            pconfig->localEmsP1Port = atoi(value);
        }
        //数据中心链接主控程序的端口port值
    else if(MATCH("SYSTEM", "localScadaPort"))
        {
            pconfig->localScadaPort = atoi(value);
        }
        //子系统个数
    else if(MATCH("SYSTEM", "subSysNum"))
        {
            pconfig->subSysNum = atoi(value);
        }   
        //数据库存储时间
    else if (MATCH("SYSTEM", "dbHoldDays"))
        {
            pconfig->dbHoldDays = atoi(value);
        }
        //数据库存储周期
    else if (MATCH("SYSTEM", "dbSaveTime"))
        {
            pconfig->dbSaveTime = atoi(value);
        }
        //
    else if (MATCH("SYSTEM", "webEmsPort"))
        {
            pconfig->webEmsPort = atoi(value);
        }
        
        //设备白名单端口号
    else if (MATCH("SYSTEM", "whitedeviceport"))
        {
            pconfig->whitedeviceport = atoi(value);
        }
      //对点模式端口号
    else if (MATCH("SYSTEM", "P2P_PORT"))
        {
            pconfig->P2P_PORT = atoi(value);
        }  
       //对点模式使能
    else if (MATCH("SYSTEM", "P2P_EN"))
        {
            pconfig->P2P_EN = atoi(value);
        }    
    else if (MATCH("SYSTEM", "pcsReactiveLimit"))
    {
        pconfig->reactiverate = atoi(value);
    }     
    //所有系统项都没匹配成功
    else
    {
        match_res = 0;
    }

    return match_res;
}


/**
 * @brief BMS网络配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int BMS_Network_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_BMS_NUM; i++) {
        //BMS类型
        MATCH_SPRINT(match_str, "bms%d_brand", i + 1);
        if (MATCH("BMS_NETWORK", match_str)) {
            pconfig->bms_brand[i] = atoi(value);
            return 1;
        }
        //BMS网络地址
        MATCH_SPRINT(match_str, "bms%d_ip", i + 1);
        if (MATCH("BMS_NETWORK", match_str)) {
            IP_COPY(pconfig->bms_ip[i], value, strlen(value));
            return 1;
        }
        //BMS网络端口
        MATCH_SPRINT(match_str, "bms%d_port", i + 1);
        if (MATCH("BMS_NETWORK", match_str)) {
            pconfig->bms_port[i] = atoi(value);
            return 1;
        }
        //BMS从站地址
        MATCH_SPRINT(match_str, "bms%d_slave_addr", i + 1);
        if (MATCH("BMS_NETWORK", match_str)) {
            pconfig->bms_slave_addr[i] = atoi(value);
            return 1;
        }
        //BMS簇数量
        MATCH_SPRINT(match_str, "bms%d_cluster", i + 1);
        if (MATCH("BMS_NETWORK", match_str)) {
            pconfig->bms_cluster[i] = atoi(value);
            return 1;
        }
        //BMS客户端网络地址
        MATCH_SPRINT(match_str, "bms%d_client_lan", i + 1);
        if (MATCH("BMS_NETWORK", match_str)) {
            pconfig->bms_client_lan[i] = atoi(value);
            return 1;
        }
        //单体电压数量
        memset(match_str, 0, sizeof(match_str));
        sprintf(match_str, "bms%d_vol_num", i + 1);
        if(MATCH("BMS_NETWORK", match_str))
        {
        pconfig->bms_vol_num[i] = atoi(value);
        return 1;
        }

        //单体温度数量
        memset(match_str, 0, sizeof(match_str));
        sprintf(match_str, "bms%d_temp_num", i + 1);
        if(MATCH("BMS_NETWORK", match_str))
        {
        pconfig->bms_temp_num[i] = atoi(value);
        return 1;
        }


        //液冷机数量
        memset(match_str, 0, sizeof(match_str));
        sprintf(match_str, "bms%d_cooler_num", i + 1);
        if(MATCH("BMS_NETWORK", match_str))
        {
        pconfig->bms_cooler_num[i] = atoi(value);
        return 1;
        }



    }

    return 0;
}




/**
 * @brief DI网络配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int DI_Network_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_DI_NUM; i++) {
        //DI网络地址
        MATCH_SPRINT(match_str, "di%d_ip", i + 1);
        if (MATCH("DI_NETWORK", match_str)) {
            IP_COPY(pconfig->di_ip[i], value, strlen(value));
            return 1;
        }
        //DI网络端口
        MATCH_SPRINT(match_str, "di%d_port", i + 1);
        if (MATCH("DI_NETWORK", match_str)) {
            pconfig->di_port[i] = atoi(value);
            return 1;
        }
        //DI项目类型
        MATCH_SPRINT(match_str, "di%d_item", i + 1);
        if (MATCH("DI_NETWORK", match_str)) {
            pconfig->di_item[i] = atoi(value);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief DIDO网络配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int DIDO_Network_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_DIDO_NUM; i++) {
        //DIDO网络地址
        MATCH_SPRINT(match_str, "dido%d_ip", i + 1);
        if (MATCH("DIDO_NETWORK", match_str)) {
            IP_COPY(pconfig->dido_ip[i], value, strlen(value));
            return 1;
        }
        //DIDO网络端口
        MATCH_SPRINT(match_str, "dido%d_port", i + 1);
        if (MATCH("DIDO_NETWORK", match_str)) {
            pconfig->dido_port[i] = atoi(value);
            return 1;
        }
        //DIDO项目类型
        MATCH_SPRINT(match_str, "dido%d_item", i + 1);
        if (MATCH("DIDO_NETWORK", match_str)) {
            pconfig->dido_item[i] = atoi(value);
            return 1;
        }
    }

    return 0;
}

/**
 * @brief PCS网络配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int PCS_Network_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_PCS_NUM; i++) {
        //PCS类型
        MATCH_SPRINT(match_str, "pcs%d_brand", i + 1);
        if (MATCH("PCS_NETWORK", match_str)) {
            pconfig->pcs_brand[i] = atoi(value);
            return 1;
        }
        //PCS网络地址
        MATCH_SPRINT(match_str, "pcs%d_ip", i + 1);
        if (MATCH("PCS_NETWORK", match_str)) {
            IP_COPY(pconfig->pcs_ip[i], value, strlen(value));
            return 1;
        }
        //PCS网络端口
        MATCH_SPRINT(match_str, "pcs%d_port", i + 1);
        if (MATCH("PCS_NETWORK", match_str)) {
            pconfig->pcs_port[i] = atoi(value);
            return 1;
        }
        //PCS从站地址
        MATCH_SPRINT(match_str, "pcs%d_slave_addr", i + 1);
        if (MATCH("PCS_NETWORK", match_str)) {
            pconfig->pcs_slave_addr[i] = atoi(value);
            return 1;
        }
        //PCS的分支数
        MATCH_SPRINT(match_str, "pcs%d_branch", i + 1);
        if (MATCH("PCS_NETWORK", match_str)) {
            pconfig->pcs_branch[i] = atoi(value);
            return 1;
        }
    }
    return 0;
}

/**
 * @brief 网关网络配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int Gateway_Network_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_GATEWAY_NUM; i++) {
        //Gateway网络地址
        MATCH_SPRINT(match_str, "gw%d_ip", i + 1);
        if (MATCH("GW_NETWORK", match_str)) {
            IP_COPY(pconfig->gw_ip[i], value, strlen(value));
            return 1;
        }
        //Gateway网络端口
        MATCH_SPRINT(match_str, "gw%d_port", i + 1);
        if (MATCH("GW_NETWORK", match_str)) {
            pconfig->gw_port[i] = atoi(value);
            return 1;
        }
    }

    return 0;
}

/**
 * @brief 测控装置网络配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int Measure_Network_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_MEASURE_NUM; i++) {
        //Measure网络地址
        MATCH_SPRINT(match_str, "measure%d_ip", i + 1);
        if (MATCH("MEASURE_NETWORK", match_str)) {
            IP_COPY(pconfig->measure_ip[i], value, strlen(value));
            return 1;
        }
        //Measure网络端口
        MATCH_SPRINT(match_str, "measure%d_port", i + 1);
        if (MATCH("MEASURE_NETWORK", match_str)) {
            pconfig->measure_port[i] = atoi(value);
            return 1;
        }
    }

    return 0;
}

/**
 * @brief 串口转网口配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int ETH_Serial_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_ETH485_NUM; i++) {
        //ETH转485链路网络地址
        MATCH_SPRINT(match_str, "eth_port%d_ip", i + 1);
        if (MATCH("ETH_SERIAL", match_str)) {
            IP_COPY(pconfig->eth485_ip[i], value, strlen(value));
            return 1;
        }
        //ETH转485链路网络端口
        MATCH_SPRINT(match_str, "eth_port%d_port", i + 1);
        if (MATCH("ETH_SERIAL", match_str)) {
            pconfig->eth485_port[i] = atoi(value);
            return 1;
        }
        //ETH转485链路存在设备组数量
        MATCH_SPRINT(match_str, "eth_port%d_group", i + 1);
        if (MATCH("ETH_SERIAL", match_str)) {
            pconfig->eth485_group[i] = atoi(value);
            return 1;
        }
        for (int j = 0; j < MAX_DEV_GROUP_NUM; j++) {
            //ETH转485链路设备port口序号
            pconfig->eth485_list[i].group_list[j].dev_port = i;
            //ETH转485链路设备类型
            MATCH_SPRINT(match_str, "eth_port%d_dev%d_type", i + 1, j + 1);
            if (MATCH("ETH_SERIAL", match_str)) {
                pconfig->eth485_list[i].group_list[j].dev_type = atoi(value);
                return 1;
            }
            //ETH转485链路设备起始从地址
            MATCH_SPRINT(match_str, "eth_port%d_dev%d_saddr", i + 1, j + 1);
            if (MATCH("ETH_SERIAL", match_str)) {
                pconfig->eth485_list[i].group_list[j].dev_saddr = atoi(value);
                return 1;
            }
            //ETH转485链路同种顺序设备数量
            MATCH_SPRINT(match_str, "eth_port%d_dev%d_num", i + 1, j + 1);
            if (MATCH("ETH_SERIAL", match_str)) {
                pconfig->eth485_list[i].group_list[j].dev_num = atoi(value);
                return 1;
            }
        }
    }

    return 0;
}

/**
 * @brief 串口配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int Serial_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_SERIAL_NUM; i++) {
        //串口类型
        MATCH_SPRINT(match_str, "serial%d_type", i + 1);
        if (MATCH("SERIAL", match_str)) {
            pconfig->serial_type[i] = atoi(value);
            return 1;
        }
        //波特率
        MATCH_SPRINT(match_str, "serial%d_baud", i + 1);
        if (MATCH("SERIAL", match_str)) {
            pconfig->serial_baud[i] = atoi(value);
            return 1;
        }
        //数据位
        MATCH_SPRINT(match_str, "serial%d_databit", i + 1);
        if (MATCH("SERIAL", match_str)) {
            pconfig->serial_databit[i] = atoi(value);
            return 1;
        }
        //停止位
        MATCH_SPRINT(match_str, "serial%d_stopbit", i + 1);
        if (MATCH("SERIAL", match_str)) {
            pconfig->serial_stopbit[i] = atoi(value);
            return 1;
        }
        //校验位
        MATCH_SPRINT(match_str, "serial%d_parity", i + 1);
        if (MATCH("SERIAL", match_str)) {
            pconfig->serial_parity[i] = value[0];
            return 1;
        }
        //单链路存在设备组数量
        MATCH_SPRINT(match_str, "serial%d_group", i + 1);
        if (MATCH("SERIAL", match_str)) {
            pconfig->serial_group[i] = atoi(value);
            return 1;
        }
        for (int j = 0; j < MAX_DEV_GROUP_NUM; j++) {
            //设备类型
            MATCH_SPRINT(match_str, "serial%d_dev%d_type", i + 1, j + 1);
            if (MATCH("SERIAL", match_str)) {
                pconfig->serial_list[i].group_list[j].dev_type = atoi(value);
                return 1;
            }
            //设备起始从地址
            MATCH_SPRINT(match_str, "serial%d_dev%d_saddr", i + 1, j + 1);
            if (MATCH("SERIAL", match_str)) {
                pconfig->serial_list[i].group_list[j].dev_saddr = atoi(value);
                return 1;
            }
            //同种顺序设备数量
            MATCH_SPRINT(match_str, "serial%d_dev%d_num", i + 1, j + 1);
            if (MATCH("SERIAL", match_str)) {
                pconfig->serial_list[i].group_list[j].dev_num = atoi(value);
                return 1;
            }
            //其他数据的初始化
            pconfig->serial_list[i].group_list[j].dev_port = i;
            pconfig->serial_list[i].group_list[j].dev_fd = 0;
            pconfig->serial_list[i].group_list[j].dev_start = 0;
        }
    }

    return 0;
}

/**
 * @brief DI通道配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int DI_Channels_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    //DI通道项目类型
    if (MATCH("DI_CHANNELS", "diSigType")) {
        pconfig->diSigType = atoi(value);
        return 1;
    }

    char match_str[64] = {0};
    for (int i = 0; i < MAX_DI_IGNORE_NUM; i++) {
        //DI信号屏蔽值
        MATCH_SPRINT(match_str, "di_ignore%d", i + 1);
        if (MATCH("DI_CHANNELS", match_str)) {
            pconfig->di_ignore[i] = atoi(value);
            return 1;
        }
    }
    for (int i = 0; i < MAX_DISIG_NUM; i++) {
        //DI通道有效性
        MATCH_SPRINT(match_str, "di_sig%d", i + 1);
        if (MATCH("DI_CHANNELS", match_str)) {
            pconfig->di_sig[i] = atoi(value);
            return 1;
        }
    }

    return 0;
}

/**
 * @brief DO通道配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int DO_Channels_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    //DO通道项目类型
    if (MATCH("DO_CHANNELS", "doSigType")) {
        pconfig->doSigType = atoi(value);
        return 1;
    }

    char match_str[64] = {0};
    for (int i = 0; i < MAX_DOSIG_NUM; i++) {
        //DO通道有效性
        MATCH_SPRINT(match_str, "do_sig%d", i + 1);
        if (MATCH("DO_CHANNELS", match_str)) {
            pconfig->do_sig[i] = atoi(value);
            return 1;
        }
    }

    return 0;
}

/**
 * @brief 时段配置内容解析
 * @note 参考函数System_parsing的参数说明
 */
static int Period_parsing(sysPara* pconfig, const char* section, const char* name, const char* value)
{
    char match_str[64] = {0};
    for (int i = 0; i < MAX_PERIOD_NUM; i++) {
        //时段配置
        MATCH_SPRINT(match_str, "period%d", i + 1);
        if (MATCH("PERIOD", match_str)) {
            fill_period(value, &(pconfig->run_period[i]));
            return 1;
        }
    }

    return 0;
}

/**
 * @brief config.ini文件分析回调函数，每一行调用一次
 * @param[in] user - 用户数据(未使用)
 * @param[in] section - config.ini中段标识[]里的内容
 * @param[in] name - 变量名
 * @param[in] value -  变量值
 * @return -1-赋值失败; 0-赋值成功
 */
static int handler(void* user, const char* section, const char* name, const char* value)
{
    sysPara* pconfig = (sysPara*)user;
    if (pconfig == NULL || section == NULL || name == NULL || value == NULL) {
        return -1;
    }

    int parsing_res = 1;
    //系统配置内容解析
    if (MATCH_SECTION("SYSTEM")) {
        parsing_res = System_parsing(pconfig, section, name, value);
    }
    //BMS网络配置内容解析
    else if (MATCH_SECTION("BMS_NETWORK")) {
        parsing_res = BMS_Network_parsing(pconfig, section, name, value);
    }
    //DI网络配置内容解析
    else if (MATCH_SECTION("DI_NETWORK")) {
        parsing_res = DI_Network_parsing(pconfig, section, name, value);
    }
    //DIDO网络配置内容解析
    else if (MATCH_SECTION("DIDO_NETWORK")) {
        parsing_res = DIDO_Network_parsing(pconfig, section, name, value);
    }
    //PCS网络配置内容解析
    else if (MATCH_SECTION("PCS_NETWORK")) {
        parsing_res = PCS_Network_parsing(pconfig, section, name, value);
    }
    //网关网络配置内容解析
    else if (MATCH_SECTION("GW_NETWORK")) {
        parsing_res = Gateway_Network_parsing(pconfig, section, name, value);
    }
    //测控网络配置内容解析
    else if (MATCH_SECTION("MEASURE_NETWORK")) {
        parsing_res = Measure_Network_parsing(pconfig, section, name, value);
    }
    //串口转网口配置内容解析
    else if (MATCH_SECTION("ETH_SERIAL")) {
        parsing_res = ETH_Serial_parsing(pconfig, section, name, value);
    }
    //串口配置内容解析
    else if (MATCH_SECTION("SERIAL")) {
        parsing_res = Serial_parsing(pconfig, section, name, value);
    }
    //DI通道配置内容解析
    else if (MATCH_SECTION("DI_CHANNELS")) {
        parsing_res = DI_Channels_parsing(pconfig, section, name, value);
    }
    //DO通道配置内容解析
    else if (MATCH_SECTION("DO_CHANNELS")) {
        parsing_res = DO_Channels_parsing(pconfig, section, name, value);
    }
    //时段配置内容解析
    else if (MATCH_SECTION("PERIOD")) {
        parsing_res = Period_parsing(pconfig, section, name, value);
    }
    else {
       // LOG_DBG("unknown section:%s", section);
        return 0; /* unknown section/name, error */
    }

    if (parsing_res == 0) {
       // LOG_DBG("unknown section:%s  name:%s", section, name);
        return 0; /* unknown section/name, error */
    }

    return 1;
}

int AppConf_Init()
{
    sysPara* config = &sysConf;
    memset(config, '\0', sizeof(sysPara));

    char fileName[50];
    MATCH_SPRINT(fileName, CONFIG_PATH);

    if (ini_parse(fileName, handler, config) < 0) {
        LOG_INFO("Can't load '%s', please check the synax.", fileName);
        AppConf_UnInit();
        return -1;
    }

    return 0;
}

void AppConf_UnInit()
{
    sysPara* config = &sysConf;
    memset(config, '\0', sizeof(sysPara));
}

sysPara* SysConf_GetInfo()
{
    return &sysConf;
}


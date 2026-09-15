/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : ip_config_update.c
 * Project : 用于中车项目
 * Description : 软件的IP配置更新处理
 * File Created : 2022/11/25
 * Author : zhuoying.wang
 * ------------------------------------------------------------
 * Last Modified : 2022/12/02
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/02 修改代码规范，代码结构，合并到base版本中
 * ------------------------------------------------------------
 * HISTORY : 2022/11/25 新规
 */

#include "main.h"
#include "ini.h"
#include "log.h"
#include "ip_config_update.h"


#define LOCAL_TIME_LEN  (32)            //本地时间字符长度
#define Addr(addr)      (addr + i * 2)  //网址数据位置计算宏，只能应用于本文件的循环体中

static unsigned int BMS_IP[MAX_BMS_NUM][4] = {{0}};     //所有BMS的IP数组
static unsigned int PCS_IP[MAX_PCS_NUM][4] = {{0}};     //所有PCS的IP数组
static unsigned int E1210_IP[MAX_DI_NUM][4] = {{0}};    //所有DI-E1210的IP数组
static unsigned int E1214_IP[MAX_DIDO_NUM][4] = {{0}};  //所有DIDO-E1214的IP数组
static unsigned int LOCAL_IP[MAX_ETH_NUM][4] = {{0}};   //所有本地的IP数组
static unsigned int DI_IGNORE[FAULT_IGNORE_NUM] = {0}; //所有DI忽略标志的数组

/**
 * @brief 获取系统时间
 * @param[out] CalendarTime_string_turned 时间出参指针
 */
static void Get_SystemTime(char* CalendarTime_string_turned)
{
    if(CalendarTime_string_turned == NULL)
    {
        return;
    }

    time_t CalendarTime = 0;
    time(&CalendarTime);
    struct tm* time_st = localtime(&CalendarTime);

    sprintf(CalendarTime_string_turned, "%d%02d%02d-%02d%02d%02d",
            (time_st->tm_yday + 1900), (time_st->tm_mon + 1), time_st->tm_mday,
            time_st->tm_hour, time_st->tm_min, time_st->tm_sec);
}

/**
 * @brief 获取DI信号使用状态
 * @param[in] DIsignal DI信号
 * @return 使用状态：0是没有，1是在使用
 */
static int GetDIsignalUsedStatus(int DIsignal)
{
    if(( DIsignal != 53001 ) &&
       ( DIsignal != 53003 ) &&
       ( DIsignal != 53004 ) &&
       ( DIsignal != 53005 ) &&
       ( DIsignal != 53006 ) &&
       ( DIsignal != 53008 ) &&
       ( DIsignal != 53010 ) &&
       ( DIsignal != 53011 ) &&
       ( DIsignal != 53012 ) &&
       ( DIsignal != 53013 ) &&
       ( DIsignal != 53014 ) &&
       ( DIsignal != 53015 ) &&
       ( DIsignal != 53016 ) &&
       ( DIsignal != 53017 ) &&
       ( DIsignal != 53018 ) &&
       ( DIsignal != 53019 ) &&
       ( DIsignal != 53020 ) &&
       ( DIsignal != 53022 ) &&
       ( DIsignal != 53023 ) &&
       ( DIsignal != 53024 ) &&
       ( DIsignal != 53025 ) &&
       ( DIsignal != 53026 ) &&
       ( DIsignal != 53061 ) &&
       ( DIsignal != 53063 ) &&
       ( DIsignal != 53065 ) &&
       ( DIsignal != 53066 ) &&
       ( DIsignal != 53068 ) &&
       ( DIsignal != 53079 ) &&
       ( DIsignal != 53080 ) )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/**
 * @brief interface文件更新程序
 * @param fragment 更新网卡序号
 * @param file 更新文件
 * @return 更新结果：小于0失败，大于0成功
 */
static int interfaces_update(int fragment, const char* file)
{
    //入参判断
    if(fragment < 1 || fragment > 3)
    {
        return -1;
    }

    //打开interface文件
    int fd = open(NET_PATH, O_RDONLY);
    if( fd < 0 )
    {
        LOG_INFO("NET_PATH open for updating failed!");
        close(fd);
        return -1;
    }
    lseek(fd, 0, SEEK_SET);

    //初始化缓存空间
    char file_buffer[MAX_INTERFACES_LENGTH];
    char tail_buffer[MAX_INTERFACES_LENGTH];
    char keyword[MXA_KEYWORD_LENGTH];
    memset(keyword, '\0', sizeof(keyword));
    memset(file_buffer, '\0', sizeof(file_buffer));
    memset(tail_buffer, '\0', sizeof(file_buffer));
    char* pstr0 = NULL;
    char* pstr1 = NULL;

    //读取interface文件内容
    int ret = read(fd, file_buffer, sizeof(file_buffer));
    if(ret < 0)
    {
        LOG_INFO("NET_PATH read for updating failed!");
        close(fd);
        return -1;
    }
    close(fd);

    switch(fragment)
    {
    case 1:
        sprintf(keyword, "iface eth0 inet static");
        pstr0 = strstr(file_buffer, keyword);
        sprintf(keyword, "iface eth1 inet static");
        pstr1 = strstr(file_buffer, keyword);
        strcpy(tail_buffer, pstr1);
        memset(pstr0, '\0', strlen(pstr0));

        sprintf(pstr0, INTERFACES_eth1,
        NetworkTotalData[80].D8[1], NetworkTotalData[80].D8[0], NetworkTotalData[81].D8[1], NetworkTotalData[81].D8[0],
        NetworkTotalData[80].D8[1], NetworkTotalData[80].D8[0], NetworkTotalData[81].D8[1],
        NetworkTotalData[80].D8[1], NetworkTotalData[80].D8[0], NetworkTotalData[81].D8[1],
        NetworkTotalData[80].D8[1], NetworkTotalData[80].D8[0], NetworkTotalData[81].D8[1]);
        strcat(pstr0, tail_buffer);
        break;
    case 2:
        sprintf(keyword, "iface eth1 inet static");
        pstr0 = strstr(file_buffer, keyword);
        sprintf(keyword, "iface eth2 inet static");
        pstr1 = strstr(file_buffer, keyword);
        strcpy(tail_buffer, pstr1);
        memset(pstr0, '\0', strlen(pstr0));

        sprintf(pstr0, INTERFACES_eth2,
        NetworkTotalData[82].D8[1], NetworkTotalData[82].D8[0], NetworkTotalData[83].D8[1], NetworkTotalData[83].D8[0],
        NetworkTotalData[82].D8[1], NetworkTotalData[82].D8[0], NetworkTotalData[83].D8[1],
        NetworkTotalData[82].D8[1], NetworkTotalData[82].D8[0], NetworkTotalData[83].D8[1]);
        strcat(pstr0, tail_buffer);
        break;
    case 3:
        sprintf(keyword, "iface eth2 inet static");
        pstr0 = strstr(file_buffer, keyword);
        memset(pstr0, '\0', strlen(pstr0));

        sprintf(pstr0, INTERFACES_eth3,
        NetworkTotalData[84].D8[1], NetworkTotalData[84].D8[0], NetworkTotalData[85].D8[1], NetworkTotalData[85].D8[0],
        NetworkTotalData[84].D8[1], NetworkTotalData[84].D8[0], NetworkTotalData[85].D8[1],
        NetworkTotalData[84].D8[1], NetworkTotalData[84].D8[0], NetworkTotalData[85].D8[1]);
        break;
    default:
        break;
    }

    //将变更后的数据回写到文件中
    FILE *fp = fopen(file, "w");
    if(fp == NULL)
    {
        LOG_INFO("open %s error", file);
        return -1;
    }
    ret = fprintf(fp, "%s", file_buffer);
    fclose(fp);

    if(ret < 0)
    {
        LOG_INFO("NET_PATH write for updating failed! Sleeping 10s before next action...");
        sleep(10);
        return -1;
    }
    else
    {
        LOG_INFO("NET_PATH updating success! Going to reboot ...");
    }

    return 1;
}

void IP_Config_Update_Task()
{
    //初始化
    int i = 0;
    int ret = 0;
    sysPara* sys_cfg = SysConf_GetInfo();

    //!读取BMS地址信息
    for(i = 0; (i < sys_cfg->bmsNum) && (i < MAX_BMS_NUM); i++)
    {
        if((NetworkTotalData[Addr(0)].D16 == 0) && (NetworkTotalData[Addr(1)].D16 == 0))
        {
            sscanf((char*)sys_cfg->bms_ip[i], "%d.%d.%d.%d", &BMS_IP[i][0], &BMS_IP[i][1], &BMS_IP[i][2], &BMS_IP[i][3]);
            NetworkTotalData[Addr(0)].D8[1] = BMS_IP[i][0];
            NetworkTotalData[Addr(0)].D8[0] = BMS_IP[i][1];
            NetworkTotalData[Addr(1)].D8[1] = BMS_IP[i][2];
            NetworkTotalData[Addr(1)].D8[0] = BMS_IP[i][3];
        }
    }

    //!读取PCS地址信息
    for(i = 0; (i < sys_cfg->pcsNum) && (i < MAX_PCS_NUM); i++)
    {
        if( NetworkTotalData[Addr(20)].D16 == 0 && NetworkTotalData[Addr(21)].D16 == 0 )
        {
            sscanf((char*)sys_cfg->pcs_ip[i], "%d.%d.%d.%d", &PCS_IP[i][0], &PCS_IP[i][1], &PCS_IP[i][2], &PCS_IP[i][3]);
            NetworkTotalData[Addr(20)].D8[1] = PCS_IP[i][0];
            NetworkTotalData[Addr(20)].D8[0] = PCS_IP[i][1];
            NetworkTotalData[Addr(21)].D8[1] = PCS_IP[i][2];
            NetworkTotalData[Addr(21)].D8[0] = PCS_IP[i][3];
        }
    }

    //!读取DI地址信息
    for(i = 0; (i < sys_cfg->diNum) && (i < MAX_DI_NUM); i++)
    {
        if( NetworkTotalData[Addr(40)].D16 == 0 && NetworkTotalData[Addr(41)].D16 == 0 )
        {
            sscanf((char*)sys_cfg->di_ip[i], "%d.%d.%d.%d", &E1210_IP[i][0], &E1210_IP[i][1], &E1210_IP[i][2], &E1210_IP[i][3]);
            NetworkTotalData[Addr(40)].D8[1] = E1210_IP[i][0];
            NetworkTotalData[Addr(40)].D8[0] = E1210_IP[i][1];
            NetworkTotalData[Addr(41)].D8[1] = E1210_IP[i][2];
            NetworkTotalData[Addr(41)].D8[0] = E1210_IP[i][3];
        }
    }

    //!读取DIDO地址信息
    for(i = 0; (i < sys_cfg->didoNum) && (i < MAX_DIDO_NUM); i++)
    {
        if( NetworkTotalData[Addr(60)].D16 == 0 && NetworkTotalData[Addr(61)].D16 == 0 )
        {
            sscanf((char*)sys_cfg->dido_ip[i], "%d.%d.%d.%d", &E1214_IP[i][0], &E1214_IP[i][1], &E1214_IP[i][2], &E1214_IP[i][3]);
            NetworkTotalData[Addr(60)].D8[1] = E1214_IP[i][0];
            NetworkTotalData[Addr(60)].D8[0] = E1214_IP[i][1];
            NetworkTotalData[Addr(61)].D8[1] = E1214_IP[i][2];
            NetworkTotalData[Addr(61)].D8[0] = E1214_IP[i][3];
        }
    }

    //!读取工控机物理端口地址信息 (/etc/network/interfaces 网络配置文件)
    int fd = open(NET_PATH, O_RDONLY);
    if(fd < 0)
    {
        LOG_INFO("%s open for reading failed! Sleeping 60s before next action...", NET_PATH);
        sleep(1);
        return;
    }
    lseek(fd, 0, SEEK_SET);

    static char file_buf[MAX_INTERFACES_LENGTH] = {0};
    memset(file_buf, 0, sizeof(file_buf));
    ret = read(fd, file_buf, sizeof(file_buf));
    if(ret < 0)
    {
        LOG_INFO("%s read for reading failed! Sleeping 10s before next action...", NET_PATH);
        sleep(1);
        return;
    }
    close(fd);

    char keyword[MXA_KEYWORD_LENGTH] = {0};
    char iface_buf[MAX_INTERFACES_LENGTH] = {0};
    char addr_buff[MAX_INTERFACES_LENGTH] = {0};
    for(i = 0; i < MAX_ETH_NUM; i++) //几个ETH就几个
    {
        if((NetworkTotalData[Addr(80)].D16 == 0) && (NetworkTotalData[Addr(81)].D16 == 0))
        {
            memset(keyword, 0, sizeof(keyword));
            memset(iface_buf, 0, sizeof(iface_buf));
            memset(addr_buff, 0, sizeof(addr_buff));
            //定位IP的位置
            sprintf(keyword, "iface eth%d inet static", i);
            sprintf(iface_buf, "%s", strstr(file_buf, keyword));
            sprintf(addr_buff, "%s", strstr(iface_buf, "address"));
            //提取IP的数据
            sscanf(addr_buff, "address %d.%d.%d.%d", &LOCAL_IP[i][0], &LOCAL_IP[i][1], &LOCAL_IP[i][2], &LOCAL_IP[i][3]);
            NetworkTotalData[Addr(80)].D8[1] = LOCAL_IP[i][0];
            NetworkTotalData[Addr(80)].D8[0] = LOCAL_IP[i][1];
            NetworkTotalData[Addr(81)].D8[1] = LOCAL_IP[i][2];
            NetworkTotalData[Addr(81)].D8[0] = LOCAL_IP[i][3];
        }
    }

    //!读取DI故障屏蔽信息
    for(i = 0; i < MAX_DISIG_NUM; i++)
    {
        DI_IGNORE[i] = sys_cfg->fault_ignore[i];
    }

    //初始化更新缓存
    static char section[64] = {0};
    static char field[64] = {0};
    static char value[64] = {0};
    char fileName[64] = {0};
    memset(fileName, 0, sizeof(fileName));
    sprintf(fileName, CONFIG_PATH);

    //!更新配置信息
    while(1)
    {
        //!更新BMS的IP配置信息
        memset(section, 0, sizeof(section));
        sprintf(section, "BMS_NETWORK");
        for(i = 0; (i < sys_cfg->bmsNum) && (i < MAX_BMS_NUM); i++)
        {
            if((NetworkTotalData[Addr(0)].D16 != 0) || (NetworkTotalData[Addr(1)].D16 != 0))
            {
                if((NetworkTotalData[Addr(0)].D8[1] != BMS_IP[i][0]) || (NetworkTotalData[Addr(0)].D8[0] != BMS_IP[i][1]) ||
                   (NetworkTotalData[Addr(1)].D8[1] != BMS_IP[i][2]) || (NetworkTotalData[Addr(1)].D8[0] != BMS_IP[i][3]))
                {
                    memset(field, 0, sizeof(field));
                    memset(value, 0, sizeof(value));

                    BMS_IP[i][0] = NetworkTotalData[Addr(0)].D8[1];
                    BMS_IP[i][1] = NetworkTotalData[Addr(0)].D8[0];
                    BMS_IP[i][2] = NetworkTotalData[Addr(1)].D8[1];
                    BMS_IP[i][3] = NetworkTotalData[Addr(1)].D8[0];

                    sprintf(field, "bms%d_ip", i + 1);
                    sprintf(value, "%d.%d.%d.%d", BMS_IP[i][0], BMS_IP[i][1], BMS_IP[i][2], BMS_IP[i][3]);

                    ret = ini_update(section, field, value, fileName);
                    if(ret < 0)
                    {
                        LOG_INFO("BMS IP address updating failed! Sleeping 10s before next action...");
                        sleep(10);
                        continue;
                    }
                }
            }
        }

        //!更新PCS的IP配置信息
        memset(section, 0, sizeof(section));
        sprintf(section, "PCS_NETWORK");
        for(i = 0; (i < sys_cfg->pcsNum) && (i < MAX_PCS_NUM); i++)
        {
            if( NetworkTotalData[Addr(20)].D16 != 0 || NetworkTotalData[Addr(21)].D16 != 0 )
            {
                if((NetworkTotalData[Addr(20)].D8[1] != PCS_IP[i][0]) || (NetworkTotalData[Addr(20)].D8[0] != PCS_IP[i][1]) ||
                   (NetworkTotalData[Addr(21)].D8[1] != PCS_IP[i][2]) || (NetworkTotalData[Addr(21)].D8[0] != PCS_IP[i][3]))
                {
                    memset(field, 0, sizeof(field));
                    memset(value, 0, sizeof(value));

                    PCS_IP[i][0] = NetworkTotalData[Addr(20)].D8[1];
                    PCS_IP[i][1] = NetworkTotalData[Addr(20)].D8[0];
                    PCS_IP[i][2] = NetworkTotalData[Addr(21)].D8[1];
                    PCS_IP[i][3] = NetworkTotalData[Addr(21)].D8[0];

                    sprintf(field, "pcs%d_ip", i + 1);
                    sprintf(value, "%d.%d.%d.%d", PCS_IP[i][0], PCS_IP[i][1], PCS_IP[i][2], PCS_IP[i][3]);

                    ret = ini_update(section, field, value, fileName);
                    if(ret < 0)
                    {
                        LOG_INFO("PCS IP address updating failed! Sleeping 10s before next action...");
                        sleep(10);
                        continue;
                    }
                }
            }
        }

        //!更新DI的IP配置信息
        memset(section, 0, sizeof(section));
        sprintf(section, "DI_NETWORK");
        for(i = 0; (i < sys_cfg->diNum) && (i < MAX_DI_NUM); i++)
        {
            if( NetworkTotalData[Addr(40)].D16 != 0 || NetworkTotalData[Addr(41)].D16 != 0 )
            {
                if((NetworkTotalData[Addr(40)].D8[1] != E1210_IP[i][0]) || (NetworkTotalData[Addr(40)].D8[0] != E1210_IP[i][1]) ||
                   (NetworkTotalData[Addr(41)].D8[1] != E1210_IP[i][2]) || (NetworkTotalData[Addr(41)].D8[0] != E1210_IP[i][3]))
                {
                    memset(field, 0, sizeof(field));
                    memset(value, 0, sizeof(value));

                    E1210_IP[i][0] = NetworkTotalData[Addr(40)].D8[1];
                    E1210_IP[i][1] = NetworkTotalData[Addr(40)].D8[0];
                    E1210_IP[i][2] = NetworkTotalData[Addr(41)].D8[1];
                    E1210_IP[i][3] = NetworkTotalData[Addr(41)].D8[0];

                    sprintf(field, "di%d_ip", i + 1);
                    sprintf(value, "%d.%d.%d.%d", E1210_IP[i][0], E1210_IP[i][1], E1210_IP[i][2], E1210_IP[i][3]);

                    ret = ini_update(section, field, value, fileName);
                    if(ret < 0)
                    {
                        LOG_INFO("DI-E1210 IP address updating failed! Sleeping 10s before next action...");
                        sleep(10);
                        continue;
                    }
                }
            }
        }

        //!更新DIDO的IP配置信息
        memset(section, 0, sizeof(section));
        sprintf(section, "DIDO_NETWORK");
        for(i = 0; (i < sys_cfg->didoNum) && (i < MAX_DIDO_NUM); i++)
        {
            if( NetworkTotalData[Addr(60)].D16 != 0 || NetworkTotalData[Addr(61)].D16 != 0 )
            {
                if((NetworkTotalData[Addr(60)].D8[1] != E1214_IP[i][0]) || (NetworkTotalData[Addr(60)].D8[0] != E1214_IP[i][1]) ||
                   (NetworkTotalData[Addr(61)].D8[1] != E1214_IP[i][2]) || (NetworkTotalData[Addr(61)].D8[0] != E1214_IP[i][3]))
                {
                    memset(field, 0, sizeof(field));
                    memset(value, 0, sizeof(value));


                    E1214_IP[i][0] = NetworkTotalData[Addr(60)].D8[1];
                    E1214_IP[i][1] = NetworkTotalData[Addr(60)].D8[0];
                    E1214_IP[i][2] = NetworkTotalData[Addr(61)].D8[1];
                    E1214_IP[i][3] = NetworkTotalData[Addr(61)].D8[0];

                    sprintf(field, "dido%d_ip", i + 1);
                    sprintf(value, "%d.%d.%d.%d", E1214_IP[i][0], E1214_IP[i][1], E1214_IP[i][2], E1214_IP[i][3]);

                    ret = ini_update(section, field, value, fileName);
                    if(ret < 0)
                    {
                        LOG_INFO("E1214 IP address updating failed! Sleeping 10s before next action...");
                        sleep(10);
                        continue;
                    }
                }
            }
        }

        //!更新物理端口配置信息
        static int is_update_local_ip = 0;
        for(i = 0; i < MAX_ETH_NUM; i++)
        {
            if((NetworkTotalData[Addr(80)].D16 != 0) && NetworkTotalData[Addr(81)].D16 != 0 )
            {
                if((NetworkTotalData[Addr(80)].D8[1] != LOCAL_IP[i][0]) || (NetworkTotalData[Addr(80)].D8[0] != LOCAL_IP[i][1]) ||
                   (NetworkTotalData[Addr(81)].D8[1] != LOCAL_IP[i][2]) || (NetworkTotalData[Addr(81)].D8[0] != LOCAL_IP[i][3]) )
                {
                    is_update_local_ip++;
                }
            }
        }
        //备份interface文件
        if(is_update_local_ip != 0)
        {
            //获取当前系统时间
            char system_time[LOCAL_TIME_LEN];
            memset(system_time, 0, LOCAL_TIME_LEN);
            Get_SystemTime(system_time);
            //拼接文件名称
            char network_bak[64];
            memset(network_bak, 0, sizeof(network_bak));
            sprintf(network_bak, "%s_%s", NET_PATH_BAK, system_time);
            //设置系统执行命令
            char backup_cmd[128];
            memset(backup_cmd, 0, sizeof(backup_cmd));
            sprintf(backup_cmd, "cp %s %s", NET_PATH, network_bak);
            //执行备份操作
            Do_System(backup_cmd, 1);
        }
        //更新interface的IP配置
        for(i = 0; i < MAX_ETH_NUM; i++)
        {
            if((NetworkTotalData[Addr(80)].D16 != 0) && NetworkTotalData[Addr(81)].D16 != 0 )
            {
                if((NetworkTotalData[Addr(80)].D8[1] != LOCAL_IP[i][0]) || (NetworkTotalData[Addr(80)].D8[0] != LOCAL_IP[i][1]) ||
                   (NetworkTotalData[Addr(81)].D8[1] != LOCAL_IP[i][2]) || (NetworkTotalData[Addr(81)].D8[0] != LOCAL_IP[i][3]) )
                {
                    ret = interfaces_update(i + 1, NET_PATH);
                    if(ret > 0)
                    {
                        LOCAL_IP[i][0] = NetworkTotalData[Addr(80)].D8[1];
                        LOCAL_IP[i][1] = NetworkTotalData[Addr(80)].D8[0];
                        LOCAL_IP[i][2] = NetworkTotalData[Addr(81)].D8[1];
                        LOCAL_IP[i][3] = NetworkTotalData[Addr(81)].D8[0];
                    }
                }
            }
        }

/*         //!更新DI屏蔽配置信息
        memset(section, 0, sizeof(section));
        sprintf(section, "DI_CHANNELS");
        for(i = 0; i < MAX_DISIG_NUM; i++)
        {
            if(DI_IGNORE[i] != DITotalIgnoration[i])
            {
                memset(field, 0, sizeof(field));
                memset(value, 0, sizeof(value));

                DI_IGNORE[i] = DITotalIgnoration[i];
                sprintf(field, "fault_ignore%d", i + 1);
                sprintf(value, "%d", DI_IGNORE[i]);

                ret = ini_update(section, field, value, fileName);
                if(ret < 0)
                {
                    LOG_INFO("DI_CHANNELS fault_ignore%d updating failed! Sleeping 10s before next action...", i + 1);
                    sleep(10);
                    continue;
                }
            }
        } */
        //轮询周期
        sleep(1);
    }
}

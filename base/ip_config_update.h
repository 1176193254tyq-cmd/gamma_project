/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : ip_config_update.h
 * Project : 用于中车项目
 * Description : 软件的IP配置更新处理
 * File Created : 2022/11/25
 * Author : zhuoying.wang
 * ------------------------------------------------------------
 * Last Modified : 2022/12/22
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/22 新规interface相关宏定义
 * ------------------------------------------------------------
 * HISTORY : 2022/12/02 修改代码规范，代码结构，合并到base版本中
 * ------------------------------------------------------------
 * HISTORY : 2022/11/25 新规
 */

#ifndef IP_CONFIG_UPDATE_H
#define IP_CONFIG_UPDATE_H


#define MAX_NETWORK_DATA_LENGTH  1024
#define MAX_INTERFACES_LENGTH    1024
#define MXA_KEYWORD_LENGTH       64

#define INTERFACES_eth1  "iface eth0 inet static\n\
        address %d.%d.%d.%d\n\
        network %d.%d.%d.0\n\
        netmask 255.255.255.0\n\
        broadcast %d.%d.%d.255\n\
        gateway %d.%d.%d.1\n"

#define INTERFACES_eth2  "iface eth1 inet static\n\
        address %d.%d.%d.%d\n\
        network %d.%d.%d.0\n\
        netmask 255.255.255.0\n\
        broadcast %d.%d.%d.255\n"

#define INTERFACES_eth3  "iface eth2 inet static\n\
        address %d.%d.%d.%d\n\
        network %d.%d.%d.0\n\
        netmask 255.255.255.0\n\
        broadcast %d.%d.%d.255\n"


/**
 * @brief 更新App配置信息任务线程
 */
void IP_Config_Update_Task();

#endif //IP_CONFIG_UPDATE_H

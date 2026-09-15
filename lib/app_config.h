/* ------------------------------------------------------------
 * Copyright (C) 2019
 * File Name : app_config.h
 * Project :
 * Description : ini文件配置内容读写，本进程的配置
 * File Created :
 * Author:  wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 修改宏定义，修改配置信息结构体，增加通威项目的宏定义，添加cpp适配宏
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改宏定义
 */

#ifndef APPCONF_H_
#define APPCONF_H_

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>
#include "system.h"

//单个设备可能的最大数量
#define MAX_DEVICE_NUM          (500)   //设备的最大数量
#define MAX_IP_LEN              (16)    //ip字符串的长度

//相关设备的最大数量:
#define MAX_GROUP_NUM           (10)    //子系统数量
#define MAX_PCS_NUM             (4)    //PCS设备数量
#define MAX_BMS_NUM             (10)    //BMS设备数量
#define MAX_DI_NUM              (5)     //DI-1210设备数量
#define MAX_DIDO_NUM            (5)     //DIDO-1214设备数量
#define MAX_GATEWAY_NUM         (5)     //网关设备数量
#define MAX_DC_METER_NUM        (20)    //直流电表设备数量
#define MAX_AC_METER_NUM        (20)    //交流电表设备数量
#define MAX_UPS_NUM             (10)    //UPS设备数量
#define MAX_AIR_NUM             (50)    //空调设备数量
#define MAX_TEMP_NUM            (50)    //温度传感器设备数量
#define MAX_MEASU_NUM           (10)    //测控设备数量
#define MAX_GAS_NUM             (10)    //气体传感器设备数量
#define MAX_WT_NUM              (10)    //无线传感器数量
#define MAX_PERIOD_NUM          (24)    //定时时段数量
#define MAX_ETH485_NUM          (12)    //ETH485口，以太网转485口
#define MAX_SERIAL_NUM          (8)     //串口数量，包含485口和232口
#define MAX_DEV_GROUP_NUM       (8)     //串口上连接的设备分组数量
#define MAX_SERIAL_DEV_NUM      (127)   //串口上连接的设备数量
#define MAX_DISIG_NUM           (300)   //300个输入信号
#define MAX_DOSIG_NUM           (100)   //100个输出信号
#define MAX_MEASURE_NUM         (10)    //测控装置
#define MAX_ETH_NUM             (3)     //设备网口数量
#define MAX_DI_IGNORE_NUM       (30)    //30个DI屏蔽信号
#define MAX_SERVER_NUM          (10)    //10个Modbus服务端，对应不同的端口
#define MAX_SENSOR_NUM          (128)   //消防探头的数量
#define FAULT_IGNORE_NUM        (10)
#define LOGIC_IGNORE_NUM        (10)
//lj 20260107 此处需要核对
//#define MAX_SUB_NUM             (2)    //子系统数量
#define MAX_SUB_NUM             (4)    //子系统数量
#define MAX_SYS_NUM             (2)     //总系统数量
#define MAX_CLIENT_NUM          (20)    //同时最多链接客户端数量

//运行模式
#define REMOTE_HANDLE           (0)     //手动遥控
#define LOCAL_AUTO              (1)     //本地自动
#define REMOTE_AUTO             (2)     //远程自动
#define LOCAL_HANDLE            (3)     //本地手动

//子系统类型
#define SUB_SYSTEM_BASE         (0)     //基础版子系统
#define SUB_SYSTEM_V1           (1)     //中核版子系统


//PCS品牌标识
#define PCS_XUJI_CBL220     (1)    //许继PCS CBL220-2500
#define PCS_TRINA       (2)    //天合自研PCS 500
#define PCS_KEHUA_BCS1725K  (3)    //科华PCS BCS1725K_B_HUB
#define PCS_ShangNeng       (4)     //上能PCS
#define PCS_PE              (5)     //PE_PCS
#define PCS_Taida           (6)     //Taida_PCS

//BMS品牌标识
#define BMS_TRINA_V1            (1)     //天合自研BMS
#define BMS_GOLD_V27            (2)     //高特BMS
#define BMS_XIENENG             (3)     //协能BMS
#define BMS_XIENENG_G2pro       (4)     //G2pro
#define BMS_XIENENG_V20         (5)     //协能BMS-V2.0版本，增加了液冷机信息





//特殊设备标志
#define U3_EC                   (1)     //英维克E3空调
#define RS_WS_N01               (2)     //建大仁科温湿度传感器
#define MHC_UPS                 (3)     //山特UPS
#define ACREL_PZ                (4)     //安科瑞PZ系列交流电能表
#define SGA                     (5)     //深国安气体传感器
#define HUA_SHENG               (6)     //华盛变测控智能箱
#define LANG_RUI                (7)     //朗睿可燃气体控制器
#define SAN_JIANG               (8)     //三江消防控制器
#define CHUANG_WEI              (9)     //烟台创为消防系统
#define WIRELESS_TEMP           (10)    //无线测温装置
#define MV_UPS                  (11)    //MVUPS
//通信方式标志
#define RS232                   (0)     //232通信方式
#define RS485                   (1)     //485通信方式
#define RS485SERVER             (2)     //485通信服务器

//DI_E1210的项目类型
#define DI_ITEM_ZHONGHE         (1)     //中核项目DI处理模式
#define DI_ITEM_ZHONGCHE        (2)     //中车雄里坡项目DI处理模式
#define DI_ITEM_SHAHE           (3)     //沙河项目E1210
#define DI_ITEM_TIANMEN50       (4)     //湖北天门_通威50MWh项目
#define DI_ITEM_ALARSHAM        (5)     //阿拉善项目DI处理模式
#define DI_ITEM_YELENG_V2       (6)     //国内液冷V2项目E1210
#define DI_ITEM_BAOSHAN         (7)     //华能保山项目E1210
#define DI_ITEM_FEICHENG        (8)     //山东肥城项目E1210

//DI_MOXA的项目类型
#define DI_TYPE_TIANMEN50       (1)     //湖北天门_通威50MWh项目
#define DI_TYPE_YELENG_PCS      (2)     //液冷-PCS测试项目
#define DI_TYPE_BAOSHAN80       (3)     //华能保山项目MOXA
#define DI_TYPE_CHALING200      (4)     //中车茶陵项目200MWh项目
#define DI_TYPE_XINLE40         (5)     //四方新乐40MWh
#define DI_TYPE_SHENZHOU20      (6)     //中南深州20MWh
//DI_MOXA的项目类型
#define DO_TYPE_CHALING200      (1)     //中车茶陵项目200MWh项目

//DIDO的项目类型
#define DIDO_ITEM_ZHONGHE1      (1)     //中核项目1号E1214设备DIDO处理模式
#define DIDO_ITEM_ZHONGHE2      (2)     //中核项目2号E1214设备DIDO处理模式
#define DIDO_ITEM_ZHONGCHE      (3)     //中车雄里坡项目DIDO处理模式
#define DIDO_ITEM_YELENG        (4)     //国内液冷项目DIDO处理模式
#define DIDO_ITEM_ALARSHAM1     (5)     //阿拉善项目1号E1214设备DIDO处理模式
#define DIDO_ITEM_ALARSHAM2     (6)     //阿拉善项目2号E1214设备DIDO处理模式
#define DIDO_ITEM_SHAHE1        (7)     //沙河项目1号E1214
#define DIDO_ITEM_SHAHE2        (8)     //沙河项目2号E1214
#define DIDO_ITEM_TIANMEN50     (9)     //湖北天门_通威50MWh项目
#define DIDO_ITEM_YELENG_V2     (10)    //国内液冷V2项目E1214
#define DIDO_ITEM_BAOSHAN1      (11)    //华能保山项目1号E1214
#define DIDO_ITEM_BAOSHAN2      (12)    //华能保山项目2号E1214
#define DIDO_ITEM_FEICHENG1     (13)    //山东肥城项目1号E1214
#define DIDO_ITEM_FEICHENG2     (14)    //山东肥城项目2号E1214
#define DIDO_ITEM_CHALING200    (15)    //中车茶陵200MWh项目E1214
#define DIDO_ITEM_XINLE40       (16)    //XINLE40MWh项目E1214
#define DIDO_ITEM_SHENZHOU20    (17)    //XINLE40MWh项目E1214
#define DIDO_ITEM_FEICHENG4     (18)    //华能保山项目2号E1214
//SERVER的类型
#define SERVER_TCP_SCADA        (1)     //TCP SCADA服务器
#define SERVER_RTU_SCADA        (2)     //RTU SCADA服务器
#define SERVER_TCP_EMS          (3)     //EMS服务器
#define SERVER_TCP_BMS          (4)     //BMS服务器


// 按字节对齐
#pragma pack(push, 1)

//时段设置
typedef struct {
    INT16U bh;        //开始时段的小时数据位
    INT16U bm;        //开始时段的分钟数据位
    INT16U eh;        //结束时段的小时数据位
    INT16U em;        //结束时段的分钟数据位
    int active_power;   //充电功率
    int reactive_power; //放电功率
} period;

//串口通信设备组属性
typedef struct {
    INT8U dev_type;   //设备类型
    INT8U dev_saddr;  //设备起始从地址
    INT8U dev_num;    //顺序设备数量
    INT8U dev_port;   //设备port口序号
    INT8U dev_fd;     //设备文件描述符
    INT8U dev_start;  //设备任务启动标志
} DEVICE_GROUP_T;

//设备组数据缓存
typedef struct {
    DEVICE_GROUP_T group_list[MAX_DEV_GROUP_NUM];       //单个串口可连接设备组
} SERIAL_LIST_T;


//参数设置
typedef struct {
//[SYSTEM]
    INT8U   lc_slave_addr;                            //LC在EMS中作为从设备，默认地址为1
    INT8U   subNum;                                   //子系统个数
    INT8U   subSysType;                               //子系统类型，例如PCS和BMS一对一子系统，或者一对二子系统
    INT8U   pcsNum;                                   //PCS台数
    INT8U   bmsNum;                                   //BMS台数
    INT8U   sysNum;                                   //系统个数
    INT8U   smaNum;                                   //小系统个数
    INT8U   dcdcNum;                                  //DCDC个数
    INT8U   bmsClientNum;                             //BMS客户端台数
    INT8U   diNum;                                    //DI设备E1210数量
    INT8U   didoNum;                                  //DIDO设备E1214数量
    INT8U   serialNum;                                //使用的实际串口数量
    INT8U   eth485_portNum;                           //485转TCP的链路数
    INT32U  run_mode;                                 //运行模式
    INT8U   gwNum;                                    //modbusRTU转TCP网关设备数量
    INT8U   ACmeterNum;                               //交流电表数量
    INT8U   ACmeterCT;                                //交流电表CT值
    INT8U   DCmeterNum;                               //直流电表数量
    INT8U   airNum;                                   //空调数量
    INT8U   tempNum;                                  //温湿度传感器数量
    INT8U   upsNum;                                   //UPS设备数量
    INT8U   measuNum;                                 //测控装置数量
    INT8U   gasNum;                                   //可燃气体传感器数量
    INT8U   fireNum;                                  //消防控制器或消防&可燃气体控制器数量
    INT8U   sensorNum;                                //消防探头的数量
    INT8U   diSigNum;                                 //本地控制器直连控制的DI信号数量
    INT8U   doSigNum;                                 //本地控制器直连控制的DO信号数量
    INT8U   wirelessTempNum;                          //
    INT8U   debugFlag;                                //20230801新增调试功能标志  
    INT8U   zlogFlag;                                 //20230914新增绝缘电阻日志记录标志  
    unsigned char emsEnable;
    unsigned int localEmsPort;
    unsigned int webEmsPort;
    unsigned int whitedeviceport;
    unsigned int P2P_PORT;
    unsigned int P2P_EN;
    unsigned char emsP1Enable;
    unsigned int localEmsP1Port;
    unsigned int localScadaPort;
    unsigned char subSysNum;

    unsigned int  dbHoldDays; //
    unsigned int  dbSaveTime;
    unsigned int  reactiverate;

//[BMS_NETWORK]
    INT32U  bms_brand[MAX_BMS_NUM];                   //BMS型号
    INT8U   bms_ip[MAX_BMS_NUM][MAX_IP_LEN];          //BMS的IP地址
    INT32U  bms_port[MAX_BMS_NUM];                    //BMS的网络端口号
    INT32U  bms_slave_addr[MAX_BMS_NUM];              //BMS的从地址
    INT32U  bms_cluster[MAX_BMS_NUM];                 //BMS的簇个数
    INT8U   bms_client_lan[MAX_BMS_NUM];              //BMS客户端的网口
    unsigned int bms_vol_num[MAX_BMS_NUM];
    unsigned int bms_temp_num[MAX_BMS_NUM];
    unsigned int bms_pole_num[MAX_BMS_NUM];
    unsigned int bms_cooler_num[MAX_BMS_NUM];


//[DI_NETWORK]
    INT8U   di_ip[MAX_DI_NUM][MAX_IP_LEN];            //E1210的IP地址
    INT32U  di_port[MAX_DI_NUM];                      //E1210的网络端口号
    INT32U  di_item[MAX_DI_NUM];                      //DI信号项目编号
//[DIDO_NETWORK]
    INT8U   dido_ip[MAX_DIDO_NUM][MAX_IP_LEN];        //E1214的IP地址
    INT32U  dido_port[MAX_DIDO_NUM];                  //E1214的网络端口号
    INT32U  dido_item[MAX_DIDO_NUM];                  //DIDO的项目编号
//[PCS_NETWORK]
    INT32U  pcs_brand[MAX_PCS_NUM];                   //PCS型号
    INT8U   pcs_ip[MAX_PCS_NUM][MAX_IP_LEN];          //PCS的IP地址
    INT32U  pcs_port[MAX_PCS_NUM];                    //PCS的网络端口号
    INT32U  pcs_slave_addr[MAX_PCS_NUM];              //PCS的从地址
    INT32U  pcs_branch[MAX_PCS_NUM];                  //PCS的分支数
//[GW_NETWORK]
    INT8U   gw_ip[MAX_GATEWAY_NUM][MAX_IP_LEN];       //网关的IP地址
    INT32U  gw_port[MAX_GATEWAY_NUM];                 //网关的网络端口号
//[MEASURE_NETWORK]
    INT8U   measure_ip[MAX_MEASURE_NUM][MAX_IP_LEN];  //测控装置IP地址
    INT32U  measure_port[MAX_MEASURE_NUM];            //测控装置网络端口号
//[ETH_SERIAL]
    INT8U   eth485_ip[MAX_ETH485_NUM][MAX_IP_LEN];    //485转TCP的IP地址
    INT32U  eth485_port[MAX_ETH485_NUM];              //485转TCP的端口号
    INT8U   eth485_group[MAX_ETH485_NUM];             //单链路存在设备组数量
    SERIAL_LIST_T eth485_list[MAX_ETH485_NUM];          //设备组容器
//[SERIAL]
    INT8U   serial_type[MAX_SERIAL_NUM];              //串口类型
    INT32U  serial_baud[MAX_SERIAL_NUM];              //波特率
    INT32U  serial_databit[MAX_SERIAL_NUM];           //数据位
    INT32U  serial_stopbit[MAX_SERIAL_NUM];           //停止位
    INT8U   serial_parity[MAX_SERIAL_NUM];            //校验位
    INT8U   serial_group[MAX_SERIAL_NUM];             //单链路存在设备组数量
    SERIAL_LIST_T serial_list[MAX_SERIAL_NUM];          //设备组容器
//[DI_CHANNELS]
    INT8U   diSigType;                                //di输入信号项目类型
    INT8U   di_sig[MAX_DISIG_NUM];                    //di输入通道，1是有效，0是无效
    INT8U   di_ignore[MAX_DISIG_NUM];                 //di输入信号，1是有效，0是无效
//[DO_CHANNELS]
    INT8U   doSigType;                                //di输入信号项目类型
    INT8U   do_sig[MAX_DOSIG_NUM];                    //do输出信号，1是有效，0是无效
//[PERIOD]
    period    run_period[MAX_PERIOD_NUM];               //时段设置数据
    INT16U  fault_ignore[FAULT_IGNORE_NUM];
    INT16U  logic_ignore[LOGIC_IGNORE_NUM];   

    INT16U  fault_ignore_num;
    INT16U  logic_ignore_num;
} sysPara;

// 按字节对齐
#pragma pack(pop)


/**
 * @brief App配置的取得
 * @return App配置
 */
sysPara* SysConf_GetInfo();

/**
 * @brief App配置数据初始化
 * @return 结果，1是成功
 */
int AppConf_Init();

/**
 * @brief 释放App配置空间
 */
void AppConf_UnInit();

#ifdef __cplusplus
}
#endif

#endif //APPCONF_H_

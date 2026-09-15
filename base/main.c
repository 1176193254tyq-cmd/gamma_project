/* ------------------------------------------------------------
* Copyright (C) 2026
* File Name : main.c
* Project :
* Description :

* File Created :
* Author : tyq
* ------------------------------------------------------------

* ------------------------------------------------------------

*/

#include "main.h"
#include <linux/netlink.h>
INT8U BusType = 0;//用于定义PCS支路的类型
int SubNumGlobe = 0;//状态机子系统的数量
//系统数据缓存 (System data cache)
volatile u16_conv SystemTotalData[MAX_SYSTEM_TOTAL_DATA_NUM]; //点表数据 (Point table data)
volatile u16_conv NetworkTotalData[MAX_NETWORK_DATA_NUM]; //网址数据 (Network data)
//线程序号 (Thread index)
static int pthread_index[MAX_DEVICE_NUM] = {0};
//交流电表CT (AC meter CT)
INT8U MS_AC_METER_CT = 0;

//线程函数结构体数组 (Thread function structure array)
PthreadFuncPool THREAD_FUNC_POOL[MAX_THREAD_FUNC_POOL_NUM] = {0};
//线程池 (Thread pool)
INT16U thread_func_pool_pos = 0;

Subsystem_State_ENUM system_work_state[MAX_GROUP_NUM]={0};

INT16U e1210_di_data[MAX_DI_NUM]={0};                     //0~9
INT16U e1214_di_data[MAX_DIDO_NUM]={0};                   //10~19
INT16U e1214_do_set[MAX_DIDO_NUM]={0};                    //20~29
INT16U e1214_do_read[MAX_DIDO_NUM]={0};                   //30~39
extern volatile INT16U g_ems_mask_bms_raw;
pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_last_nvme_state = -1;

/*检测固态硬盘是否被挂载*/

static int Is_MountPoint_Mounted(const char *mount_point)
{
    FILE *fp;
    char line[512];

    fp = fopen("/proc/mounts", "r");
    if (fp == NULL)
    return 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char device[128];
        char mount[128];

        if (sscanf(line, "%127s %127s", device, mount) == 2)
        {
            if (strcmp(mount, mount_point) == 0)
            {
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);
    return 0;
}
/*创建目录，如果目录不存在*/

static int Create_Dir_If_Not_Exist(const char *dir)
{
    if (access(dir, F_OK) == 0)
    return 0;

    if (mkdir(dir, 0777) != 0)
    {
        perror("mkdir failed");
        return -1;
    }

    return 0;
}
/*根据固态硬盘的挂载情况切换软连接的指向*/

static int Switch_Log_Symlink(int nvme_mounted)
{
    const char *target_dir;

    if (nvme_mounted)
    {
        target_dir = NVME_LOG_DIR;

        if (Create_Dir_If_Not_Exist(NVME_LOG_DIR) != 0)
        return -1;
    }
    else
    {
        target_dir = FLASH_LOG_DIR;

        if (Create_Dir_If_Not_Exist(FLASH_LOG_DIR) != 0)
        return -1;
    }

    unlink(LOG_LINK_DIR);

    if (symlink(target_dir, LOG_LINK_DIR) != 0)
    {
        perror("symlink failed");
        return -1;
    }

    printf("日志软链接切换成功: %s -> %s\n", LOG_LINK_DIR, target_dir);

    return 0;
}
/*轮询动态检测固态硬盘挂载情况*/

void *Nvme_Log_Monitor_Thread(void *arg)
{
    int current_state;

    int fd,ret,size=2048;
    char online,buf[2048] = {0};
    //创建一个netlink socket，协议类型为NETLINK_KOBJECT_UEVENT
    int s = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if(s < 0)
    return -1;
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, & size, sizeof(size));  //设置接收缓冲区大小
    struct sockaddr_nl  sockaddr;
    sockaddr.nl_family = AF_NETLINK;   //必须设置为 AF_NETLINK 或者 PF_NETLINK
    sockaddr.nl_pid = getpid();           //唯一标识符，类似于网络编程中的端口号
    sockaddr.nl_groups = 1;             //组播掩码
    ret = bind(s, (struct sockaddr *)& sockaddr, sizeof(struct sockaddr_nl));  //绑定socket地址
    if(ret < 0)
    {
        close(s);
        return -1;
    }
    fd = open(DEVPATH,O_RDONLY);  //打开供电状态文件
    if(fd < 0)
    {
        close(s);
        LOG_INFO("供电检测文件打开失败");
        return -1;
    }
    while (1)
    {
        current_state = Is_MountPoint_Mounted(NVME_MOUNT_POINT);
        recv(s, &buf, sizeof(buf), 0);        //接收uevent消息，实际应用中一般开一个线程用select或者poll、epoll监测可读事件
        if(!strcmp(buf, MSG))          //判断是否供电状态变化事件
        {
            lseek(fd, 0, SEEK_SET);
            read(fd,&online,1);       //读取供电状态文件
            LOG_INFO("power change,%c\n",online);
            if(online == '1')
            LOG_INFO("power on\n"); //供电正常
            else if(online == '0')
            {
                LOG_INFO("power off\n"); //系统掉电
                // sync();           //同步缓冲区数据到磁盘
            }
        }
        if ((current_state != g_last_nvme_state)||(online == '0'))
        {
            pthread_mutex_lock(&g_log_mutex);

            printf("NVMe 状态变化: %d -> %d\n",g_last_nvme_state, current_state);
            zlog_fini();//关闭日志
            if (Switch_Log_Symlink(current_state) == 0)
            {
                if (dzlog_init(LOG_CONFIG_FILE, "my_cat") == 0)
                {
                    g_last_nvme_state = current_state;
                    printf("日志切换成功\n");
                }
                else
                {
                    printf("dzlog_init 失败\n");
                }
            }
            else
            {
                printf("日志软链接切换失败\n");
            }

            pthread_mutex_unlock(&g_log_mutex);
        }

        usleep(50);
    }
    close(fd);
    close(s);
    return NULL;
}

bool Do_System(const char* cmd, int times)
{
    bool res = false;
    int ret = 0;
    int i = 0;

    for (i = 0; i < times; i++) {
        ret = system(cmd);
        if (1 == ret) {
            printf("Do_System:程序命令为空\n");
            printf("Do_System:system ret=%d\n", ret);
        }
        else if (-1 == ret) {
            printf("Do_System:创建命令子进程失败\n");
            printf("Do_System:system ret=%d\n",ret);
        }
        else if (0x7F00 == ret) {
            printf("Do_System:命令错误，无法执行\n");
            printf("Do_System:system ret =%d\n",ret);
        }
        else {
            if (WIFEXITED(ret)) {
                if (WEXITSTATUS(ret) != 0) {
                    printf("Do_System:程序结束，返回值：%d\n", WEXITSTATUS(ret));
                    printf("Do_System:system ret=%d\n", ret);
                }
                else {
                    res = true;
                    break;
                }
            }
            else if (WIFSIGNALED(ret)) {
                printf("Do_System:程序被信号杀死，信号值：%d\n", WTERMSIG(ret));
                printf("Do_System:system ret=%d\n", ret);
            }
            else if (WSTOPSIG(ret)) {
                printf("Do_System:程序被信号暂停，信号值：%d\n", WSTOPSIG(ret));
                printf("Do_System:system ret=%d\n", ret);
            }
        }
    }

    return res;
}

int log_Init(void)
{
    int mounted;

    mounted = Is_MountPoint_Mounted(NVME_MOUNT_POINT);

    if (Switch_Log_Symlink(mounted) != 0)
    {
        return -1;
    }

    if (dzlog_init(LOG_CONFIG_FILE, "my_cat") != 0)
    {
        zlog_fini();
        return -1;
    }

    if (mounted)
    printf("NVMe 已挂载，日志写入固态硬盘\n");
    else
    printf("NVMe 未挂载，日志写入 Flash\n");

    return 0;
}
/**
* @brief 以log形式输出init文件的内容 Output the contents of the init file in log form
*/

static void Output_Init_Log()
{
    sysPara* sys_cfg = SysConf_GetInfo();
    int i = 0;
    //[SYSTEM]
    LOG_INFO("lc_slave_addr: %d", sys_cfg->lc_slave_addr);
    LOG_INFO("SubSystem num: %d", sys_cfg->subNum);
    LOG_INFO("System num: %d", sys_cfg->sysNum);
    LOG_INFO("PCS num: %d", sys_cfg->pcsNum);
    LOG_INFO("BMS num: %d", sys_cfg->bmsNum);

    LOG_INFO("Web port: %d", sys_cfg->webEmsPort);
    LOG_INFO("EMS port: %d", sys_cfg->localEmsPort);
    LOG_INFO("reactiverate: %d", sys_cfg->reactiverate);
    for (i = 0; (i < sys_cfg->bmsNum) && (i < MAX_BMS_NUM); i++) {
        LOG_INFO("BMS-%d --> brand: %d, ip: %s, port: %d, slave_addr: %d, cluster: %d, client LAN:%d", (i+1), sys_cfg->bms_brand[i],
        sys_cfg->bms_ip[i], sys_cfg->bms_port[i], sys_cfg->bms_slave_addr[i], sys_cfg->bms_cluster[i], sys_cfg->bms_client_lan[i]);
    }
    for (i = 0; (i < sys_cfg->pcsNum) && (i < MAX_PCS_NUM); i++) {
        LOG_INFO("PCS-%d --> brand: %d, ip: %s, port: %d, slave_addr: %d, branch: %d", (i+1),
        sys_cfg->pcs_brand[i], sys_cfg->pcs_ip[i], sys_cfg->pcs_port[i], sys_cfg->pcs_slave_addr[i], sys_cfg->pcs_branch[i]);
    }
    for (i = 0; (i < sys_cfg->gwNum) && (i < MAX_GATEWAY_NUM); i++) {
        LOG_INFO("Gateway-%d ip: %s, port: %d", (i+1), sys_cfg->gw_ip[i], sys_cfg->gw_port[i]);
    }
    for (i = 0; (i < sys_cfg->measuNum) && (i < MAX_MEASURE_NUM); i++) {
        LOG_INFO("Measure-%d ip: %s, port: %d", (i+1), sys_cfg->measure_ip[i], sys_cfg->measure_port[i]);
    }

    for (i = 0; i < MAX_SERIAL_NUM; i++) {
        LOG_INFO("Serial-%d --> type: %d, baud: %d, databit: %d, stopbit: %d, parity: %c, group: %d", (i+1), sys_cfg->serial_type[i],
        sys_cfg->serial_baud[i], sys_cfg->serial_databit[i], sys_cfg->serial_stopbit[i], sys_cfg->serial_parity[i], sys_cfg->serial_group[i]);
        for (int j = 0; (j < sys_cfg->serial_group[i]) && (j < MAX_DEV_GROUP_NUM); j++) {
            LOG_INFO("serial-%d group-%d --> type: %d, saddr: %d, num: %d", (i+1), (j+1), sys_cfg->serial_list[i].group_list[j].dev_type,
            sys_cfg->serial_list[i].group_list[j].dev_saddr, sys_cfg->serial_list[i].group_list[j].dev_num);
        }
    }

}
static void On_Sigint(int s){
    (void)s;
    acl_db_disconnect();
    acl_db_global_cleanup();
    _exit(0);
}

int main(int argc, char* argv[])
{
    UNUSED_PARAM(argc);
    UNUSED_PARAM(argv);
    static int i = 0;
    static int j = 0;
    //zlog初始化zlog initialization

    u32_conv Datatemp1;
    static int item_dcdc;

    //zlog初始化 zlog initialization

    /* zlog日志初始化  zlog initialization */
    g_last_nvme_state = Is_MountPoint_Mounted(NVME_MOUNT_POINT);

    log_Init();
    LOG_INFO("本地控制器启动!!");

    // led_on();
    //读取配置文件init的内容 Read the contents of the configuration file init
    AppConf_Init();
    Output_Init_Log();
    sysPara* sys_cfg = SysConf_GetInfo();

    for (i = 0; i < MAX_DEVICE_NUM; i++) {
        pthread_index[i] = i;
    }

    if(sys_cfg->bmsNum==8)
    {
        SET_INPUT(STATUS_WORD3,0xFF);
        BMS_Unpack_To_Set_Hold(0xFF);//切入切除功能对应的点位，后续可能去掉
        SET_INPUT(NUMBER_OF_BMSs,sys_cfg->bmsNum);
    }

    else if(sys_cfg->bmsNum==4)
    {
        SET_INPUT(STATUS_WORD3,0xF);

        BMS_Unpack_To_Set_Hold(0xF);
        SET_INPUT(NUMBER_OF_BMSs,sys_cfg->bmsNum);

    }
    else
    {
        SET_INPUT(STATUS_WORD3,0x3);
        BMS_Unpack_To_Set_Hold(0x3);
        SET_INPUT(NUMBER_OF_BMSs,sys_cfg->bmsNum);
    }

    if(sys_cfg->pcsNum==4)
    {
        SET_INPUT(STATUS_WORD1,0xF);

        PCS_Unpack_To_Set_Hold(0xF);

        SET_INPUT(NUMBER_OF_PCSs,sys_cfg->pcsNum);
    }
    else if(sys_cfg->pcsNum==2)
    {
        SET_INPUT(STATUS_WORD1,0x3);
        PCS_Unpack_To_Set_Hold(0x3);
        SET_INPUT(NUMBER_OF_PCSs,sys_cfg->pcsNum);
    }
    else
    {
        SET_INPUT(STATUS_WORD1,0x1);
        PCS_Unpack_To_Set_Hold(0x1);
        SET_INPUT(NUMBER_OF_PCSs,sys_cfg->pcsNum);
    }
    /*版本号*/
    SET_INPUT(LC_PROTOCOL_VERSION, 14);
    SET_INPUT(SOFTWARE_MAJOR_VERSION,1);
    SET_INPUT(SOFTWARE_MINOR_VERSION_H,1);
    SET_INPUT(SOFTWARE_MINOR_VERSION_L,1);
    SET_INPUT(SOFTWARE_SPC_VERSION,105);
    SET_HOLD(REMOTE_LOCAL_CONTROL_ENABLE,1);
    SET_HOLD(REMOTE_LOCAL_CONTROL_MODE,1);
    LOG_INFO("协议版本号：V%d,软件版本号：V%dR%dC%dSPC%d",GET_INPUT(LC_PROTOCOL_VERSION),GET_INPUT(SOFTWARE_MAJOR_VERSION),GET_INPUT(SOFTWARE_MINOR_VERSION_H),GET_INPUT(SOFTWARE_MINOR_VERSION_L),GET_INPUT(SOFTWARE_SPC_VERSION));
    //初始化系统配置参数 Initialize system configuration parameters
    SystemTotalData[SYS_PCS_NUM_ADDR].D16 = sys_cfg->pcsNum;
    SystemTotalData[SYS_IN_PCS_NUM_ADDR].D16 = sys_cfg->pcsNum;
    SystemTotalData[SYS_BMS_NUM_ADDR].D16 = sys_cfg->bmsNum;
    SystemTotalData[SYS_IN_BMS_NUM_ADDR].D16 = sys_cfg->bmsNum;
    if((sys_cfg->pcsNum)==(sys_cfg->bmsNum))
    {
        BusType = S_BUS;
        g_ems_mask_bms_raw = S_BUS;

        BMS_PER_SYS=2;
        SUBS_PER_SYS=BMS_PER_SYS;
        MAX_BMS=(SYS_COUNT * BMS_PER_SYS);
        BMS_G0_MASK=0x03;
        BMS_G1_MASK=0x0C;
        BMS_G2_MASK=0x0F;
        SubNumGlobe = 2;

    }
    else
    {
        BusType = D_BUS;
        g_ems_mask_bms_raw = D_BUS;

        BMS_PER_SYS=4;
        SUBS_PER_SYS=BMS_PER_SYS;
        MAX_BMS=(SYS_COUNT * BMS_PER_SYS);
        BMS_G0_MASK=0x0F;
        BMS_G1_MASK=0xF0;
        BMS_G2_MASK=0xFF;
        SubNumGlobe = 4;
    }
    // 初始化
    if (acl_db_global_init() != 0) {
      LOG_INFO("mysql_library_init failed\n");
        return 1;
    }

    // 2) 连接数据库
    if (acl_db_connect("127.0.0.1", 3306, "root", "qwer1234", "log_db") != 0) {
        LOG_INFO("connect MySQL failed\n");

    }
    else
    {
        LOG_INFO("connect MySQL sucess\n");

    }

    //设置分离式线程属性 Air conditioner initialized to power on
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_t new_pthread_Timed_Check = 0;

    //复位所有定时器 Reset all timers
    Reset_All_Timer();

    //初始化定时器，Linux系统一个进程一个定时器

    Timer_Init();
    pthread_t tid = 0;
    pthread_create(&tid, &thread_attr, Nvme_Log_Monitor_Thread, NULL);
    pthread_detach(tid);
    //建立定时检查线程 Create a scheduled check thread
    pthread_create(&new_pthread_Timed_Check, &thread_attr, (void*)Task_Timed_Check, NULL);
    pthread_t new_pthread_Timed_Work = 0;
    // 建立定时工作线程 Create a scheduled worker thread
    pthread_create(&new_pthread_Timed_Work, &thread_attr, (void*)Task_Timed_Work, NULL);
    pthread_t new_pthread_Serial_Work = 0;
    // 初始化串口硬件
    pthread_create(&new_pthread_Serial_Work, &thread_attr, (void *)Serial_Device_Task, (void *)4);
    usleep(10*1000);

    pthread_t new_pthread_EMS_Server= 0;
    pthread_t new_pthread_P2P_Server= 0;
    pthread_t new_pthread_alarm_log= 0;
    // ***************************TCP/IP服务端建立********************************//

    if (sys_cfg->emsEnable > 0)
    {
        pthread_create(&new_pthread_EMS_Server,&thread_attr, (void*)Task_EMS_Server, NULL);
    }
    usleep(10*1000);

    if (sys_cfg->pcs_brand[0] == PCS_Taida)
    {
        pthread_create(&new_pthread_alarm_log,&thread_attr, (void*)Alarm_ParseAndLog_All, NULL);

        usleep(10*1000);
    }
    else if(sys_cfg->pcs_brand[0] == PCS_TRINA)
    {
        pthread_create(&new_pthread_alarm_log,&thread_attr, (void*)Trina_Alarm_ParseAndLog_All, NULL);
        usleep(10*1000);
    }
    //创建对点模式
    if (sys_cfg->P2P_EN > 0)
    {
        pthread_create(&new_pthread_P2P_Server,&thread_attr, (void*)Task_P2P_Server, NULL);
    }
    //创建web线程
    pthread_t new_pthread_web_Server= 0;
    pthread_create(&new_pthread_web_Server,&thread_attr, (void*)Task_WEB_Server, NULL);

    usleep(10*1000);
    pthread_t new_pthread_whitelist_Server= 0;
    pthread_create(&new_pthread_whitelist_Server,&thread_attr, (void*)Task_Whitelist_Server, NULL);

    usleep(10*1000);
    // 建立BMS线程，BMS序号传入线程
    pthread_t thread_eth_BMS[MAX_BMS_NUM] = {0};
    pthread_t thread_eth_write_BMS[MAX_BMS_NUM] = {0};
    for (i = 0; (i < sys_cfg->bmsNum) && (i < MAX_BMS_NUM); i++)
    {
        if (sys_cfg->bms_brand[i] == BMS_XIENENG)
        {

            pthread_create(&thread_eth_BMS[i], &thread_attr, (void*)BMS_XieNeng_Task, (void *)&pthread_index[i]);

            usleep(10 * 1000);
        }

        else if (sys_cfg->bms_brand[i] == BMS_XIENENG_G2pro)
        {

            pthread_create(&thread_eth_BMS[i], &thread_attr, (void*)BMS_G2pro_Task, (void *)&pthread_index[i]);
            usleep(10 * 1000);
        }
    }
    // 建立PCS线程
    pthread_t thread_eth_PCS[MAX_PCS_NUM] = {0};
    pthread_t thread_eth_PCS_write[MAX_PCS_NUM] = {0};
    for (i = 0; i < sys_cfg->pcsNum; i++)
    {
        if (sys_cfg->pcs_brand[i] == PCS_ShangNeng)
        {

            pthread_create(&thread_eth_PCS[i], &thread_attr, (void*)PCS_ShangNeng_Task, (void *)&pthread_index[i]);

            usleep(10 * 1000);
        }

        else if (sys_cfg->pcs_brand[i] == PCS_PE)
        {

            pthread_create(&thread_eth_PCS[i], &thread_attr, (void*)PCS_PE_Task, (void *)&pthread_index[i]);

        }
        else if (sys_cfg->pcs_brand[i] == PCS_TRINA)
        {

            pthread_create(&thread_eth_PCS[i], &thread_attr, (void*)PCS_Trina_Task, (void *)&pthread_index[i]);
            usleep(10 * 1000);
            pthread_create(&thread_eth_PCS_write[i], &thread_attr,  PCS_TRina_write_Task, (void *)&pthread_index[i]);

        }
        else if (sys_cfg->pcs_brand[i] == PCS_Taida)
        {

            pthread_create(&thread_eth_PCS[i], &thread_attr, (void*)PCS_Taida_Task, (void *)&pthread_index[i]);
            usleep(10 * 1000);
            pthread_create(&thread_eth_PCS_write[i], &thread_attr, (void*)PCS_TAida_write_Task, (void *)&pthread_index[i]);

        }
    }
    pthread_t thread_share_ram = 0;
    pthread_create(&thread_share_ram, &thread_attr, (void *)Task_share_ram, NULL);
    usleep(1000);
    pthread_t new_pthread_State= 0;
    pthread_t new_pthread_Trina_State= 0;
    if (sys_cfg->pcs_brand[0] == PCS_Taida)
    {
        pthread_create(&new_pthread_State, &thread_attr, (void*)State_Task, NULL);
        usleep(10*1000);
    }
    else if (sys_cfg->pcs_brand[0] == PCS_TRINA)
    {
        pthread_create(&new_pthread_Trina_State, &thread_attr, (void*)Trina_State_Task, NULL);
        usleep(10*1000);
    }

    pthread_t new_cem9000= 0;

    pthread_create(&new_cem9000, &thread_attr, (void*)Cem9000_Task, (void *)&pthread_index[0]);
    usleep(10*1000);

    while (1)
    {

        sleep(100); //延时 delay
    }
    exit(0);
}
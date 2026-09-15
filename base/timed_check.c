/* ------------------------------------------------------------
* Copyright (C) 2022
* File Name : timed.c
* Project :
* Description : 定时器、超时判定
* File Created :
* Author : wujinjin
* ------------------------------------------------------------
* Last Modified :
* Modified By :
* ------------------------------------------------------------
* HISTORY :
*/

#include "main.h"
#include "timed_check.h"
#include "timer.h"
//通讯超时故障标志位
static INT8U bms_comm_fault[MAX_BMS_NUM] = {0};             //bms通讯故障标志位，1-通讯故障，0-正常
static INT8U ems_comm_fault[1] = {0};                       //EMS通讯故障标志位，1-通讯故障，0-正常
static INT8U pcs_comm_fault[MAX_PCS_NUM] = {0};                       //pcs通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U di_comm_fault[MAX_DI_NUM] = {0};                  //DI通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U dido_comm_fault[MAX_DIDO_NUM] = {0};              //DI通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U temp_comm_fault[MAX_TEMP_NUM] = {0};              //温湿度传感器通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U measu_comm_fault[MAX_MEASU_NUM] = {0};            //测控装置通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U gas_comm_fault[MAX_GAS_NUM] = {0};                //气体传感器通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U ups_comm_fault[MAX_UPS_NUM] = {0};                //UPS通讯故障标志位，1-通讯故障，0-正常
static volatile INT8U ac_meter_comm_fault[MAX_AC_METER_NUM] = {0};      //AC电表通讯故障标志位，1-通讯故障，0-正常
INT8U Get_BMS_Comm(INT8U num)
{
if((num < 0) || (num >= MAX_BMS_NUM))
{
return TIMED_CHECK_OUT_BOUNDS;
}
return bms_comm_fault[num];
}

void Set_BMS_Comm(INT8U num, INT8U status,INT8U isreset)
{
if((num < 0) || (num >= MAX_BMS_NUM))
{
return;
}
bms_comm_fault[num] = status;
if((status == IsNoFault)&&(TRUE==isreset))
{
Reset_Timer(BMS_TIMER_BASE + num);    //没有故障，则自动把定时器复位
}
}



INT8U Get_EMS_Comm(INT8U num)
{

return ems_comm_fault[0];
}

void Set_EMS_Comm(INT8U num, INT8U status,INT8U isreset)
{

ems_comm_fault[0] = status;
if((status == IsNoFault)&&(TRUE==isreset))
{
Reset_Timer(EMS_TIMER_BASE + 0);    //没有超时，则自动把定时器复位
}
}



/**
 * @brief 初始化通讯故障位
 */
static void Init_Fault_Flag()
{
int i = 0;

//清除BMS通讯故障位
for(i = 0; i < MAX_BMS_NUM; i++)
{
Set_BMS_Comm(i, IsNoFault,TRUE);
}

}

INT8U Get_PCS_Comm(INT8U num)
{
if((num < 0) || (num >= MAX_PCS_NUM))
{
return TIMED_CHECK_OUT_BOUNDS;
}
return pcs_comm_fault[num];
}

void Set_PCS_Comm(INT8U num, INT8U status,INT8U isreset)
{
if((num < 0) || (num >= MAX_PCS_NUM))
{
return;
}
pcs_comm_fault[num] = status;
if((status == IsNoFault)&&(TRUE==isreset))
{
Reset_Timer(PCS_TIMER_BASE + num);    //没有故障，则自动把定时器复位
}
}

INT8U Get_DI_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_DI_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return di_comm_fault[num];
}

void Set_DI_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_DI_NUM)) {
return;
}
di_comm_fault[num] = status;
if (status == IsNoFault) {
Reset_Timer(DI_TIMER_BASE + num);
}
}

INT8U Get_DIDO_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_DIDO_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return dido_comm_fault[num];
}

void Set_DIDO_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_DIDO_NUM)) {
return;
}
dido_comm_fault[num] = status;
if (status == IsNoFault) {
Reset_Timer(DIDO_TIMER_BASE + num);
}
}

INT8U Get_TEMP_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_TEMP_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return temp_comm_fault[num];
}

void Set_TEMP_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_TEMP_NUM)) {
return;
}
temp_comm_fault[num] = status;
if (status == IsNoFault) {
Reset_Timer(TEMP_HUMI_TIMER_BASE + num);
}
}

INT8U Get_MEASU_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_MEASU_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return measu_comm_fault[num];
}

void Set_MEASU_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_MEASU_NUM)) {
return;
}
measu_comm_fault[num] = status;
if (status == IsNoFault) {
Reset_Timer(MEASU_TIMER_BASE + num);
}
}

INT8U Get_GAS_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_GAS_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return gas_comm_fault[num];
}

void Set_GAS_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_GAS_NUM)) {
return;
}
gas_comm_fault[num] = status;
if (status == IsNoFault) {

Reset_Timer(GAS_TIMER_BASE + num);
}
}

INT8U Get_UPS_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_UPS_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return ups_comm_fault[num];
}

void Set_UPS_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_UPS_NUM)) {
return;
}
ups_comm_fault[num] = status;
if (status == IsNoFault) {
Reset_Timer(UPS_TIMER_BASE + num);
}
}


INT8U Get_AC_METER_Comm(INT8U num)
{
if ((num < 0) || (num >= MAX_AC_METER_NUM)) {
return TIMED_CHECK_OUT_BOUNDS;
}
return ac_meter_comm_fault[num];
}

void Set_AC_METER_Comm(INT8U num, INT8U status)
{
if ((num < 0) || (num >= MAX_AC_METER_NUM)) {
return;
}
ac_meter_comm_fault[num] = status;
if (status == IsNoFault) {
Reset_Timer(AC_METER_TIMER_BASE + num);
}
}
/********************************************通讯超时检测************************************************/

/**
 * @brief 设备通讯超时检测
 * @param[in] dev_name 设备名称
 * @param[in] dev_num 设备数量
 * @param[in] timer 定时器序号
 * @param[in] timeout 定时器超时时间
 * @param[in] set_handler 通讯状态设置函数声明
 * @param[in] get_handler 通讯状态获取函数声明
 * @param[in] data_pos 设备状态点表位置
 */
static void Check_Dev_Timeout(char* dev_name, INT8U dev_num, INT16U timer, INT32U timeout,
            COMM_SET set_handler, COMM_GET get_handler, INT32U data_pos)
{
int i = 0;
INT32U curtime;
INT32U timewarn; //提前重连时间
for(i = 0; (i < dev_num) && ((timer + i) < TIMER_CNT); i++)
{
//超时设置设备通讯故障状态
curtime = Timer_GetTick() - TIMEOUT_TIMER[timer + i];
//LOG_INFO("nowtime is %d",curtime);
timewarn = timeout/4;//
if(curtime >= timeout)
{
set_handler(i, IsFault,TRUE);
//   LOG_INFO("%s-%d timeout fault ", dev_name, i);

}
else if((curtime/20>0)&&(((curtime/20)%(timewarn/20))==0))//提示重连,不做定时器复位
{
set_handler(i, IsWarn,FALSE);
}
//有故障
if(get_handler(i) == IsFault)
{
if(GetBit(SystemTotalData[data_pos].D16, i) != 1)
{
SetBit(SystemTotalData[data_pos].D16, i);        //设备通信故障
LOG_INFO("%s-%d timeout fault generated: [134:%x] ", dev_name, i, SystemTotalData[data_pos].D16);
}
}
//没故障
else
{
if(GetBit(SystemTotalData[data_pos].D16, i) != 0)
{
ClearBit(SystemTotalData[data_pos].D16, i);      //设备通信正常
LOG_INFO("%s-%d timeout fault removal: [134:%x] ", dev_name, i, SystemTotalData[data_pos].D16);
}
}
} //loop
}



/**
 * @brief  定时任务集合
 * @note
 * @retval
 */
void Task_Timed_Check(void)
{
//初始化
int i = 0;
sysPara* sys_cfg = SysConf_GetInfo();
LOG_INFO("Check Fault Task start! ");

//初始化通讯故障位
Init_Fault_Flag();

while(1)
{
/********************************************故障/告警检测************************************************/
//故障/告警检测
if((Timer_GetTick() - TIMEOUT_TIMER[FALUT_CHECK_TIMER]) >= WARN_FAULT_TIMEOUT_CNT)
{


//轮询周期50ms
TIMEOUT_TIMER[FALUT_CHECK_TIMER] = Timer_GetTick();
}

/********************************************通讯超时检测************************************************/


//BMS超时检测
Check_Dev_Timeout("BMS", sys_cfg->bmsNum, BMS_TIMER_BASE, BMS_TIMEOUT_CNT, Set_BMS_Comm, Get_BMS_Comm, 135);

//EMS超时检测
Check_Dev_Timeout("EMS", 1, EMS_TIMER_BASE, EMS_TIMEOUT_CNT, Set_EMS_Comm, Get_EMS_Comm, 154);

//pcs超时检测
Check_Dev_Timeout("PCS", sys_cfg->pcsNum, PCS_TIMER_BASE, PCS_TIMEOUT_CNT, Set_PCS_Comm, Get_PCS_Comm, 100);

//EMS超时检测
Check_Dev_Timeout("MEASU", 1, MEASU_TIMER_BASE, MEASU_TIMEOUT_CNT, Set_MEASU_Comm, Get_MEASU_Comm, 155);

//pcs超时检测2
Check_Dev_Timeout("UPS", 1, UPS_TIMER_BASE, UPS_TIMEOUT_CNT, Set_UPS_Comm, Get_UPS_Comm, 156);


usleep(10 * 1000); //10ms
}
}

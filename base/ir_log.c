#include "ir_log.h"

//**一条记录大小大概60byte**//

static zlog_category_t *ir_log;
static zlog_category_t *ir_csv;
static BMS_State_ENUM bms_sw_state[2][16]={0};      //记录BMS的状态
static INT16U bms_record_cnt[2][16]={0};          //设置绝缘电阻值输出条目数
#define RECORD_LENTH    (20)                        //需要打印记录的数量
#define RECORD_TIME_S(a)  (a*100)                   //打印间隔
void Read_Rack_SW_State(void)
{
    char bms_name[64]={0};
    for(INT16U j = 0; j < 2; j++)
    {
        for (INT16U i = 0; i < 16; i++)
        {
            switch (bms_sw_state[j][i])
            {
                case SW_INIT:
                    if(GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)     //主正吸合
                    && GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))    //且主负吸合
                    {
                        bms_sw_state[j][i] = SW_CLOSE;
                        bms_record_cnt[j][i]=0;
                        // sprintf(bms_name,"bms[%d][%d]",j,i);
                        // zlog_put_mdc("bms_num",bms_name);
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 闭合",j,i);
                    }
                    else if(!GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)   //主正断开
                         && !GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))  //且主负断开
                    {
                        bms_sw_state[j][i] = SW_OPEN;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 断开",j,i);
                    }
                    else
                    {
                        bms_sw_state[j][i] = SW_OPEN_2_CLOSE;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 闭合中",j,i);
                    }
                    break;
                case SW_OPEN:
                    if(GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)     //主正吸合
                    && GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))    //且主负吸合
                    {
                        bms_sw_state[j][i] = SW_CLOSE;
                        bms_record_cnt[j][i]=0;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 闭合",j,i);
                    }
                    else if(GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)   //主正吸合
                         || GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))  //或主负吸合
                    {
                        bms_sw_state[j][i] = SW_OPEN_2_CLOSE;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 闭合中",j,i);
                    }             
                    break;
                case SW_OPEN_2_CLOSE:
                    if(GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)     //主正吸合
                    && GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))    //且主负吸合
                    {
                        bms_sw_state[j][i] = SW_CLOSE;
                        bms_record_cnt[j][i]=0;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 闭合",j,i);
                    }
                    else if(!GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)   //主正断开
                         && !GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))  //且主负断开
                    {
                        bms_sw_state[j][i] = SW_OPEN;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 断开",j,i);
                    }
                    break;
                case SW_CLOSE:
                    if(!GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)     //主正吸合
                    || !GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))    //且主负吸合
                    {
                        bms_sw_state[j][i] = SW_CLOSE_2_OPEN;
                        bms_record_cnt[j][i] = 1;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 断开中",j,i);
                    }
                    break;
                case SW_CLOSE_2_OPEN:
                    if(GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)     //主正吸合
                    && GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))    //且主负吸合
                    {
                        bms_sw_state[j][i] = SW_CLOSE;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 闭合",j,i);
                        bms_record_cnt[j][i]=0;
                    }
                    else if(!GetBit(SystemTotalData[10201+j*1000+50*i].D16,1)   //主正断开
                         && !GetBit(SystemTotalData[10201+j*1000+50*i].D16,2))  //且主负断开
                    {
                        bms_sw_state[j][i] = SW_OPEN;
                        zlog_info(ir_log,"BMS[%d]电池簇[%d] 断开",j,i);
                    }                    
                    break;                    
                default:
                    break;
            }
        }
    }
}

void Printf_IR_LOG(void)
{
    char bms_name[64]={0};
    static INT16U last_ir_value[2][16]={0};
    sysPara* sys_cfg = SysConf_GetInfo();
    INT16U change_limit=0;
    INT16U abs_value=0;
    for(INT16U j = 0; j < 2; j++)
    {
        for (INT16U i = 0; i < 16; i++)
        {
            switch (sys_cfg->zlogFlag)
            {
            case 1:
                if(bms_record_cnt[j][i])                                
                {
                    abs_value=abs(last_ir_value[j][i] - SystemTotalData[10213+j*1000+50*i].D16);
                    change_limit=last_ir_value[j][i]*0.01;
                    if(abs_value>change_limit) 
                    {
                        sprintf(bms_name,"bms[%d][%d]",j,i);
                        zlog_put_mdc("bms_num",bms_name);
                        zlog_info(ir_csv,",%d",SystemTotalData[10213+j*1000+50*i].D16);
                        last_ir_value[j][i] = SystemTotalData[10213+j*1000+50*i].D16;
                    }
                }
                break;
            case 2:
                if(last_ir_value[j][i] != SystemTotalData[10213+j*1000+50*i].D16) 
                {
                    sprintf(bms_name,"bms[%d][%d]",j,i);
                    zlog_put_mdc("bms_num",bms_name);
                    zlog_info(ir_csv,",%d",SystemTotalData[10213+j*1000+50*i].D16);
                    last_ir_value[j][i] = SystemTotalData[10213+j*1000+50*i].D16;
                }
                break;
            default:
                break;
            }
        }
    }
    zlog_put_mdc("bms_num","");                     //文件存入常规日志中
}



void IR_LOG_Record_Task(void)
{
    INT16U cnt=0;
    INT8U bms_rack_num[2]={0};     //几个簇
    ir_log = zlog_get_category("my_dog");
    ir_csv = zlog_get_category("my_dog1");
    INT32U last_time=0,now_time=0;
    if (!ir_log || !ir_csv) 
    {
        LOG_INFO("电池簇绝缘电阻日志记录任务失败");
        return;
    }
    zlog_info(ir_log, "开始记录电池簇绝缘电阻值");
    last_time = Timer_GetTick();
    while (1)
    {
        Read_Rack_SW_State();
        now_time = Timer_GetTick();
        if(last_time > now_time)
        {
            last_time = now_time;
        }
        if((now_time-last_time)>=RECORD_TIME_S(2))         //2秒记录一次
        {
            last_time = now_time;
            Printf_IR_LOG();
        }
        usleep(100*1000);
    }
}
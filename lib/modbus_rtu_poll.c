#include  "modbus_rtu_poll.h"

#define BUFFER_SIZE     (64)
static INT8U temp_buffer[BUFFER_SIZE]={0};
static INT16U poll_register_value[20]={0};

#define POLL_WATI_TIME    (50)
#define POLL_REPEAT_ASK   (3-1)

/*  小模块地址说明
    10：温度读取（只读）  11：设定温度（读写）  12：显示精度（读写）
    13：温度补偿（读写）  31：华氏度切换（读写）16：输入类型（读写）    
*/
//**************************读寄存器地址*************************************//
poll_data_str poll_data_list[]=
{
//* 序号  register_value      poll_state    wait_time repeat_ask   slave_id    cmd_type  register_addr  register_num*//
    {0,     NULL,             SEND_READY,       0,       0,          10,       MODBUS_CMD_03,    10,          1},      //这里地址定义有弊端，如果读取的地址数量多，会有问题
//    {1,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch1_accuracy,       SEND_READY,       0,       0,          10,       MODBUS_CMD_03,    12,          1},
//    {2,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch1_correction,     SEND_READY,       0,       0,          10,       MODBUS_CMD_03,    13,          1},
//    {3,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch1_input_type,     SEND_READY,       0,       0,          10,       MODBUS_CMD_03,    16,          1},
//    {4,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch1_f_c,            SEND_READY,       0,       0,          10,       MODBUS_CMD_03,    31,          1},
//    {5,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch2_pv,             SEND_READY,       0,       0,          11,       MODBUS_CMD_03,    10,          1},
//    {6,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch2_accuracy,       SEND_READY,       0,       0,          11,       MODBUS_CMD_03,    12,          1},
//    {7,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch2_correction,     SEND_READY,       0,       0,          11,       MODBUS_CMD_03,    13,          1},   
//    {8,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch2_input_type,     SEND_READY,       0,       0,          11,       MODBUS_CMD_03,    16,          1},
//    {9,     (INT16U *)&g_sys_run_parameter.temperature_parameter.ch2_f_c,            SEND_READY,       0,       0,          11,       MODBUS_CMD_03,    31,          1},    
 
};




#define POLL_LIST_SIZE  (sizeof(poll_data_list)/sizeof(poll_data_str))              //计算巡检命令的条数
//#define POLL_06_LIST_SIZE  (sizeof(poll_06_data_list)/sizeof(poll_data_str))              //计算巡检命令的条数
//**********************************数据读取命令***************************************//
INT16U Modbus_RTU_Read_Slave(INT8U *send_buf,INT8U slave_id,INT8U read_cmd,INT16U read_addr,INT16U read_num)
{
    INT16U send_length=0,crc_value=0;
    send_buf[send_length++]=slave_id;
    send_buf[send_length++]=read_cmd;
    send_buf[send_length++]=(read_addr>>8)&0xFF;
    send_buf[send_length++]=(read_addr>>0)&0xFF;
    send_buf[send_length++]=(read_num>>8)&0xFF;
    send_buf[send_length++]=(read_num>>0)&0xFF;
    crc_value=crc16(send_buf,send_length);     
    send_buf[send_length++]=(crc_value>>0)&0xFF;
    send_buf[send_length++]=(crc_value>>8)&0xFF;     
    return send_length;
}

//**********************************写单个线圈***************************************//
INT16U Modbus_RTU_Write_CMD05(INT8U *send_buf,INT8U slave_id,INT16U write_addr,INT16U write_value)
{
    INT16U send_length=0,crc_value=0;
    send_buf[send_length++]=slave_id;
    send_buf[send_length++]=MODBUS_CMD_05;
    send_buf[send_length++]=(write_addr>>8)&0xFF;
    send_buf[send_length++]=(write_addr>>0)&0xFF;
    if(write_value)
    {
        send_buf[send_length++]=(0xFF00>>8)&0xFF;
        send_buf[send_length++]=(0xFF00>>0)&0xFF;
    }
    else
    {
        send_buf[send_length++]=(0x0000>>8)&0xFF;
        send_buf[send_length++]=(0x0000>>0)&0xFF;
    }
    crc_value=crc16(send_buf,send_length);     
    send_buf[send_length++]=(crc_value>>0)&0xFF;
    send_buf[send_length++]=(crc_value>>8)&0xFF;     
    return send_length;

}
//*************************写单个寄存器的值************************************//
INT16U Modbus_RTU_Write_CMD06(INT8U *send_buf,INT8U slave_id,INT16U write_addr,INT16U *write_value)
{
    INT16U send_length=0,crc_value=0;
    send_buf[send_length++]=slave_id;
    send_buf[send_length++]=MODBUS_CMD_06;
    send_buf[send_length++]=(write_addr>>8)&0xFF;
    send_buf[send_length++]=(write_addr>>0)&0xFF;  //
    send_buf[send_length++]=(*write_value>>8)&0xFF;
    send_buf[send_length++]=(*write_value>>0)&0xFF;    
    crc_value=crc16(send_buf,send_length);     
    send_buf[send_length++]=(crc_value>>0)&0xFF;
    send_buf[send_length++]=(crc_value>>8)&0xFF;      
    return send_length;
}
INT16U Modbus_RTU_Write_CMD15(INT8U *send_buf,INT8U slave_id,INT16U write_addr,INT16U write_num,INT8U *write_value)
{
    INT16U send_length=0,crc_value=0,coil_num=0;
    send_buf[send_length++]=slave_id;
    send_buf[send_length++]=MODBUS_CMD_15;
    send_buf[send_length++]=(write_addr>>8)&0xFF;
    send_buf[send_length++]=(write_addr>>0)&0xFF;  //
    send_buf[send_length++]=(write_num>>8)&0xFF;   //要写的线圈数量
    send_buf[send_length++]=(write_num>>0)&0xFF;    
    coil_num=(write_num%8)==0?write_num/8 : write_num/8+1;  //求余+1
    send_buf[send_length++]=coil_num;               //实际传输的字节数
    for(INT8U i=0;i<coil_num;i++)
    {
        send_buf[send_length++]=*(write_value+i);
    }
    crc_value=crc16(send_buf,send_length);
    send_buf[send_length++]=(crc_value>>0)&0xFF;
    send_buf[send_length++]=(crc_value>>8)&0xFF;      
    return send_length;
}
//*************************写多个寄存器的值************************************//
INT16U Modbus_RTU_Write_CMD16(INT8U *send_buf,INT8U slave_id,INT16U write_addr,INT16U write_num,INT16U *write_value)
{
    INT16U send_length=0,crc_value=0;
    send_buf[send_length++]=slave_id;
    send_buf[send_length++]=MODBUS_CMD_16;
    send_buf[send_length++]=(write_addr>>8)&0xFF;
    send_buf[send_length++]=(write_addr>>0)&0xFF;  //
    send_buf[send_length++]=(write_num>>8)&0xFF;
    send_buf[send_length++]=(write_num>>0)&0xFF;
    send_buf[send_length++]=write_num*2;
    for(INT8U i=0;i<write_num;i++)
    {
        send_buf[send_length++]=*(write_value+i)>>8;
        send_buf[send_length++]=*(write_value+i)>>0;
    }
    crc_value=crc16(send_buf,send_length);     
    send_buf[send_length++]=(crc_value>>0)&0xFF;
    send_buf[send_length++]=(crc_value>>8)&0xFF;      
    return send_length;
}

//*************************01命令解析************************************//
static INT8S Modbus_RTU_CMD01_Analysis(INT16U read_num,INT8U *rx_buf,INT8U *data_buf)
{
    INT16U return_num=0;
    return_num=read_num%8?read_num/8+1:read_num/8;
    if(rx_buf[1]==0x81)
    {
        // printf("01命令解析错误\n");
        return -1;                              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }
    else if(rx_buf[2]==return_num)
    {
        for(INT16U x=0;x<return_num;x++)
        {   
            *(data_buf+x)=rx_buf[x+3];               //返回正确数据
        }  
        return 1;
    }
    else
    {
        // printf("01命令解析失败\n");
        return 0;                               //数据解析有误
    }
}
//*************************02命令解析************************************//
// static INT8S Modbus_RTU_CMD02_Analysis(INT8U read_num,INT8U *rx_buf,INT8U *data_buf)
// {
//     if(rx_buf[1]==0x81)
//     {
//         return -1;                              //读取数据错误
//     }
//     else if(rx_buf[2]==read_num*2)
//     {
//         for(INT16U x=0;x<read_num;x++)
//         {   
//             data_buf=rx_buf[x+3];               //返回正确数据
//         }  
//         return 1;
//     }
//     else
//     {
//         return 0;                               //数据解析有误
//     }
// }


//*************************03命令解析************************************//
static INT8S Modbus_RTU_CMD03_Analysis(INT8U read_num,INT8U *rx_buf,INT16U *data_buf)
{
    if(rx_buf[1]==0x83)
    {
        return -1;                              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }    
    else if(rx_buf[2]==read_num*2)
    {
        for(INT16U x=0;x<read_num;x++)
        {
            data_buf[x]=rx_buf[x*2+3]<<8 | rx_buf[x*2+4]<<0;
        }  
        return 1;                               //返回正确数据
    }
    else
    {
        return 0;                               //数据解析有误
    }
}

//*************************04命令解析************************************//
static INT8S Modbus_RTU_CMD04_Analysis(INT8U read_num,INT8U *rx_buf,INT16U *data_buf)
{
    if(rx_buf[1]==0x84)
    {
        return -1;                              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }    
    else if(rx_buf[2]==read_num*2)
    {
        for(INT16U x=0;x<read_num;x++)
        {
            data_buf[x]=rx_buf[x*2+3]<<8 | rx_buf[x*2+4]<<0;
        }  
        return 1;                               //返回正确数据
    }
    else
    {
        return 0;                               //数据解析有误
    }
}
//*************************05命令解析************************************//
static INT8S Modbus_RTU_CMD05_Analysis(INT16U write_addr,INT8U *rx_buf,INT16U *data_buf)
{
    if(rx_buf[1]==0x85)
    {
        return -1;                              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }    
    else if(write_addr == (rx_buf[2]<<8 | rx_buf[3]<<0)
        && *data_buf == (rx_buf[4]<<8 | rx_buf[5]<<0))    //读取地址和返回的地址相同
    {
        // *data_buf=rx_buf[4]<<8 | rx_buf[5]<<0;
        return 1;                               //返回正确数据
    }
    else
    {
        return 0;                               //数据解析有误
    }
}

//*************************06命令解析************************************//
static INT8S Modbus_RTU_CMD06_Analysis(INT16U write_addr,INT8U *rx_buf,INT16U *write_value)
{ 
    if(rx_buf[1]==0x86)
    {
        return -1;              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }  
    else if((write_addr == (rx_buf[2]<<8 | rx_buf[3]<<0)) 
         && (*write_value == (rx_buf[4]<<8 | rx_buf[5]<<0)))
    {
        return 1;               //返回正确数据
    }
    else
    {
        return 0;               //数据解析有误
    }
}    

//*************************15命令解析************************************//
static INT8S Modbus_RTU_CMD15_Analysis(INT16U write_addr,INT8U *rx_buf,INT16U write_num)
{
    if(rx_buf[1]==0x8F)
    {
        return -1;              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }  
    else if((write_addr == (rx_buf[2]<<8 | rx_buf[3]<<0)) 
         && (write_num == (rx_buf[4]<<8 | rx_buf[5]<<0)))
    {
        return 1;               //返回正确数据
    }
    else
    {
        return 0;               //数据解析有误
    }
}
//*************************16命令解析************************************//
static INT8S Modbus_RTU_CMD16_Analysis(INT16U write_addr,INT8U *rx_buf,INT16U write_num)
{
    if(rx_buf[1]==0x90)
    {
        return -1;              //读取数据错误 rx_buf[2]:01 不支持的功能码 02读取地址有误  03读取数量有误  04输出数据有误
    }  
    else if((write_addr == (rx_buf[2]<<8 | rx_buf[3]<<0)) 
         && (write_num == (rx_buf[4]<<8 | rx_buf[5]<<0)))
    {
        return 1;               //返回正确数据
    }
    else
    {
        return 0;               //数据解析有误
    }
}

//*********************************modbus处理状态**************************************************//
void Modbus_RTU_POLL(INT8U *send_buf,poll_data_str *p1,INT8U *rx_buf,int fd)
{
    INT16U send_length,x,crc_value;
    INT8S resault=0;
    int receive_lenth=0;
    uint64_t now_time=Timer_GetTick();
    uint64_t dif_time=0;
    INT32U delay_time=0;
    switch(p1->poll_state)
    {
        case SEND_READY:                              //发送巡检命令
            switch(p1->cmd_type)
            {
                case MODBUS_CMD_01:      
                case MODBUS_CMD_02:            
                case MODBUS_CMD_03:
                case MODBUS_CMD_04: 
                    send_length=Modbus_RTU_Read_Slave(send_buf,p1->slave_id,p1->cmd_type,p1->register_addr,p1->register_num);
                    break;
                case MODBUS_CMD_05:
                    break;
                case MODBUS_CMD_06:
                    send_length=Modbus_RTU_Write_CMD06(send_buf,p1->slave_id,p1->register_addr,p1->register_value);
                    break;
                case MODBUS_CMD_16:
                    send_length=Modbus_RTU_Write_CMD16(send_buf,p1->slave_id,p1->register_addr,p1->register_num,p1->register_value);
                    break;
                default:
                    break;
            }
            write(fd,send_buf,send_length);
            // delay_time=(波特率/（字节数*12）)+10ms
          //  LOG_INFO("发送长度：%d",send_length);
            p1->poll_state=SEND_END;                //进入下个状态
            break;
        case SEND_END:     
            // USART_Send_Data(p1,send_length);             
            // if(p1->usart_state==0)                  //检测有没有发送完成
            {
                p1->wait_time=Timer_GetTick();
                p1->poll_state=WAIT_ACK;                   //进入下个状态
            }
            break;
        case WAIT_ACK:
            // receive_lenth=read(fd,rx_buf,256);        //查询数据是否接收到了
            receive_lenth=__Serial_ReadComPort(fd,rx_buf,256);
            // dif_time=TIMER_U16_DIFF(now_time,p2->wait_time);    //判断超时时间
            dif_time=(now_time-p1->wait_time)*10;    //判断超时时间
            if(dif_time>=POLL_WATI_TIME && receive_lenth==0)    //超时且数据没接收到
            {                                       
                if(p1->repeat_ask++>=POLL_REPEAT_ASK)           //判断超时次数是否超过了阈值
                {
                    p1->repeat_ask=0;
                    p1->poll_state=ACK_ERR;
                    // printf("数据回复超时\n");
                }
                else
                {
                    p1->poll_state=SEND_READY;                           //否，再发送一次    
                }
            }
            else if(receive_lenth>=4)                                   //接收到数据了
            {
                for(x=0;x<receive_lenth;x++)
                {
                    if(rx_buf[x]==p1->slave_id)                                         //判断设备ID
                    {
                        crc_value=crc16(rx_buf+x,receive_lenth-x-2);
                        if(crc_value==(rx_buf[receive_lenth-2]<<0)|(rx_buf[receive_lenth-1]<<8))    //判断CRC的值
                        {
                            switch((rx_buf[x+1]))
                            {   
                                case MODBUS_CMD_01:
                                    resault=Modbus_RTU_CMD01_Analysis(p1->register_num,rx_buf,(INT8U *)(p1->register_value));
                                    break;                                                 
                                case MODBUS_CMD_03:
                                    resault=Modbus_RTU_CMD03_Analysis(p1->register_num,rx_buf,p1->register_value);
                                    break;
                                case MODBUS_CMD_04:
                                    resault=Modbus_RTU_CMD04_Analysis(p1->register_num,rx_buf,p1->register_value);
                                    break;                                    
                                case MODBUS_CMD_05:
                                    resault=Modbus_RTU_CMD05_Analysis(p1->register_addr,rx_buf,p1->register_value);
                                    break;
                                case MODBUS_CMD_06:
                                    resault=Modbus_RTU_CMD06_Analysis(p1->register_addr,rx_buf,p1->register_value);
                                    break;
                                case MODBUS_CMD_16:
                                    resault=Modbus_RTU_CMD16_Analysis(p1->register_addr,rx_buf,p1->register_num);
                                    break;
                                default:
                                    p1->poll_state=ACK_ERR;    
                                    break;                         
                            }
                            break;    
                        }
                        // else
                        // {
                        //     p1->poll_state=ACK_ERR;                            
                        // }
                    }
                }
                if(resault != 1)
                {
                    if(p1->repeat_ask++>=POLL_REPEAT_ASK)           //判断超时次数是否超过了阈值
                    {
                        p1->repeat_ask=0;
                        p1->poll_state=ACK_ERR;
                    }
                    else
                    {
                        p1->poll_state=SEND_READY;                           //否，再发送一次    
                    }
                }
                else
                {
                    p1->repeat_ask=0;
                    p1->poll_state=ACK_OK;
                }
            }
            break;
        case ACK_OK:
            break;   
        case ACK_ERR:
            break;
        case WRITE_READY:
            break;   
    }
    usleep(10*1000);               //这个时间要根据波特率和发送字节数设置      
}



// void Modbus_Poll(usart_str *p1)
// {
//     static INT8U poll_num;
//     Modbus_Polling_Device(p1,&poll_data_list[poll_num],temp_buffer);  
//     if(poll_data_list[poll_num].poll_state==ACK_OK || poll_data_list[poll_num].poll_state==ACK_ERR)
//     {
//         if(poll_data_list[poll_num].poll_state==ACK_ERR)                                    //判断小模块是否通讯故障
//         {
//             if(poll_data_list[poll_num].slave_id==10)
//             {
// //                g_sys_run_parameter.sys_ram_data.bit_alarm_2 |= ERR_FLAG_TEMP_CH1_LOSS;
//             }
//             if(poll_data_list[poll_num].slave_id==11)
//             {
// //                g_sys_run_parameter.sys_ram_data.bit_alarm_2 |= ERR_FLAG_TEMP_CH2_LOSS;
//             }            
//         }
//         if(poll_data_list[poll_num].poll_state==ACK_OK)
//         {
//             if(poll_data_list[poll_num].slave_id==10)
//             {
// //                g_sys_run_parameter.sys_ram_data.bit_alarm_2 &= (~ERR_FLAG_TEMP_CH1_LOSS);
//             }
//             if(poll_data_list[poll_num].slave_id==11)
//             {
// //                g_sys_run_parameter.sys_ram_data.bit_alarm_2 &= (~ERR_FLAG_TEMP_CH2_LOSS);
//             }        
//         }
//         poll_data_list[poll_num].poll_state=SEND_READY;
//         poll_data_list[poll_num].cmd_type=MODBUS_CMD_03;
//         poll_num++;
//         if(poll_num>=POLL_LIST_SIZE)
//         {
//             poll_num=0;
//         }        
//     }
// }





/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : UPS_device_task.c
 * Project : 串口设备通信处理任务
 * Description : UPS设备通信处理任务
 * File Created : 2023/03/27
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/27
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/27 新规
 */

#include "main.h"
#include "trina_log.h"
#include "ups_device_task.h"


void MHC_UPS_Device_Task(const void* item)
{
    //初始化
    if (item == NULL) {
        return;
    }
    DEVICE_GROUP_T* dev_group = (DEVICE_GROUP_T*)item;
    int recv_bytes = 0;
    INT8U ups_send_buffer[SERIAL_SEND_LEN] = {0};
    INT8U ups_recv_buffer[SERIAL_RECV_LEN] = {0};
    int dev_fd = dev_group->dev_fd;
    int saddr = dev_group->dev_saddr; //总线上从地址,第一个地址
    INT8U ups_index = saddr - 1;
    dev_group->dev_start++;

    //开启任务处理
    while (1) {
        if ((saddr <= 0) || (saddr >= MAX_UPS_NUM)) {
            LOG_INFO("山特UPS invalid device slave address : %d ", saddr);
            usleep(1000);
            break;
        }

        //发送UPS状态请求
        memset(ups_send_buffer, 0, sizeof(ups_send_buffer));
        ups_send_buffer[0] = 0x51;
        ups_send_buffer[1] = 0x31;
        ups_send_buffer[2] = 0x0D;
        Serial_WriteComPort(dev_fd, (char*)ups_send_buffer, 3);
        usleep(CMD_DELAY_500);

        //接收数据
        memset(ups_recv_buffer, 0, sizeof(ups_recv_buffer));
        recv_bytes = Serial_ReadComPort(dev_fd, ups_recv_buffer, sizeof(ups_recv_buffer));
        //接收失败
        if (recv_bytes < 0) {
            LOG_INFO("山特UPS receive data failure");
        }
        //接收成功
        else if (recv_bytes > 0) {
            //数据解析
            MHC_UPS_DataProcess(ups_recv_buffer, ups_index);
        }

        //下一个从设备
        saddr++;
        if (saddr >= (dev_group->dev_saddr + dev_group->dev_num)) {
            saddr = dev_group->dev_saddr;
        }
        //轮询周期
        usleep(100 * 1000);

    } //loop R/W
}

void MHC_UPS_DataProcess(const INT8U* pbuf, const INT8U ups_index)
{
    //接收的数据
    if (pbuf == NULL) {
        return;
    }
#if 0
    printf("[%s:%d]  Recv : ", __FUNCTION__, __LINE__);
    for (int i = 0; i < 12; i++) {
        printf("0x%X ", pbuf[i]);
    }
    printf("\n");
#endif
    //初始化
    int int_data = 0;
    float float_arry[6] = {0};
    INT8U ups_state[10] = {0};
    int ret = 0;

    //收信内容整理
    ret = sscanf((char*)pbuf, "(%f %f %f %d %f %f %f %s",
                 &float_arry[0], &float_arry[1], &float_arry[2], &int_data, &float_arry[3], &float_arry[4], &float_arry[5], ups_state);
    if (ret != EOF) {
        //通讯正常
        Set_UPS_Comm(ups_index, IsNoFault);
        //UPS参数
        SystemTotalData[53602 + ups_index * 10].D16 = float_arry[0] * 10;  //输入电压: MMM.M
        SystemTotalData[53608 + ups_index * 10].D16 = float_arry[1] * 10;  //上次转电池放电时电压: NNN.N
        SystemTotalData[53604 + ups_index * 10].D16 = float_arry[2] * 10;  //输出电压: PPP.P
        SystemTotalData[53605 + ups_index * 10].D16 = int_data;            //输出负载百分比: QQQ
        SystemTotalData[53603 + ups_index * 10].D16 = float_arry[3] * 10;  //输入频率: RR.R
        SystemTotalData[53607 + ups_index * 10].D16 = float_arry[4] * 100; //电池单元电压: S.SS
        SystemTotalData[53606 + ups_index * 10].D16 = float_arry[5] * 10;  //温度: TT.T
        //UPS状态 : b7b6b5b4b3b2b1b0
        INT8U status = 0;
        WriteBit(status, 7, (ups_state[0] == '1'));  //bit7 ： 市电异常
        WriteBit(status, 6, (ups_state[1] == '1'));  //bit6 ： 电池电压低
        WriteBit(status, 5, (ups_state[2] == '1'));  //bit5 ： 旁路模式
        WriteBit(status, 4, (ups_state[3] == '1'));  //bit4 ： UPS故障
        WriteBit(status, 3, (ups_state[4] == '1'));  //bit3 ： UPS后备式(0：在线式)
        WriteBit(status, 2, (ups_state[5] == '1'));  //bit2 ： 测试进行中
        WriteBit(status, 1, (ups_state[6] == '1'));  //bit1 ： 关机
        WriteBit(status, 0, (ups_state[7] == '1'));  //bit0 ： 保留是0
        SystemTotalData[53601 + ups_index * 10].D16 = status;
    }
}

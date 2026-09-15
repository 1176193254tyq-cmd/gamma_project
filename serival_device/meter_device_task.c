/* ------------------------------------------------------------
 * Copyright (C) 2023
 * File Name : meter_device_task.c
 * Project : 串口设备处理项目
 * Description : 电表设备处理程序
 * File Created : 2023/03/29
 * Author : jinxue.zhao
 * ------------------------------------------------------------
 * Last Modified : 2023/03/29
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2023/03/29 文件创建
 */

#include "main.h"
#include "trina_log.h"
#include "meter_device_task.h"

void ACREL_PZ_Meter_Task(const void *item)
{
    // 初始化
    if (item == NULL)
    {
        return;
    }
    DEVICE_GROUP_T *dev_group = (DEVICE_GROUP_T *)item;
    INT8U recv_buff[SERIAL_RECV_LEN] = {0};
    int dev_fd = dev_group->dev_fd;
    int saddr = dev_group->dev_saddr;
    int Loop = 0;
    int recv_bytes = 0;
    sysPara *sys_cfg = SysConf_GetInfo();
    dev_group->dev_start++;

    // 开启任务处理
    while (1)
    {
        if ((saddr <= 0) || (saddr >= MAX_AC_METER_NUM))
        {
            LOG_INFO("安科瑞PZ系列交流电能表 invalid device slave address : %d ", saddr);
            usleep(1000);
            break;
        }

        switch (Loop++)
        {
        case 0:
            Modbus_RTU_Read03_HoldRegist(dev_fd, saddr, ACREL_PZ_block1_addr, ACREL_PZ_block1_size, CMD_DELAY_200);
            break;
        case 1:
            Modbus_RTU_Read03_HoldRegist(dev_fd, saddr, ACREL_PZ_block2_addr, ACREL_PZ_block2_size, CMD_DELAY_200);
            break;
        case 2:
            Modbus_RTU_Read03_HoldRegist(dev_fd, saddr, ACREL_PZ_block3_addr, ACREL_PZ_block3_size, CMD_DELAY_200);
            break;
        case 3:
            if (MS_AC_METER_CT != sys_cfg->ACmeterCT)
            {
                INT16U setval[1] = {sys_cfg->ACmeterCT};
                Modbus_RTU_Write_Multiple(dev_fd, saddr, ACREL_PZ_CT_addr, 1, setval, CMD_DELAY_200);
                LOG_INFO("set ACmeterCT %d : %d", saddr, sys_cfg->ACmeterCT);
                MS_AC_METER_CT = sys_cfg->ACmeterCT;
            }
            break;
        default:
            // 下一个从设备
            Loop = 0;
            saddr++;
            break;
        }

        // 接收返信
        memset((char *)recv_buff, 0, sizeof(recv_buff));
        recv_bytes = Serial_ReadComPort(dev_fd, recv_buff, sizeof(recv_buff));
        if (recv_bytes < 0)
        {
            LOG_INFO("RS485: receive data failed!");
        }
        // 需要增加粘包处理
        else if (recv_bytes > 0)
        {
            // 数据解析
            ACREL_PZ_DataProcess(recv_buff);
        }

        // 从地址都已经遍历完毕
        if (saddr >= (dev_group->dev_saddr + dev_group->dev_num))
        {
            saddr = dev_group->dev_saddr;
        }
        // 轮询周期
        usleep(1000);

    } // loop R/W
}

void ACREL_PZ_DataProcess(const INT8U *pbuf)
{
    // 初始化
    if (pbuf == NULL)
    {
        return;
    }
#if 0
    printf("[%s:%d]  Recv : ", __FUNCTION__, __LINE__);
    for (int i = 0; i < 32; i++) {
        printf("0x%X ", pbuf[i]);
    }
    printf("\n");
#endif
    u16_conv temp = {0};
    INT16U i = 0;
    INT16U saddr = pbuf[0];
    if (saddr >= MAX_AC_METER_NUM)
    {
        LOG_INFO("安科瑞PZ系列交流电能表 slave address transfinite : %d ", saddr);
        return;
    }

    // 安科瑞PZ系列交流电能表 系统设置
    if (pbuf[2] == 36)
    {
        // 通讯正常
        Set_AC_METER_Comm(saddr - 1, IsNoFault);

        for (i = 0; i < 18; i++)
        {
            if (i != 4)
            {
                continue;
            }
            temp.D8[1] = pbuf[3 + i * 2];
            temp.D8[0] = pbuf[4 + i * 2];
            MS_AC_METER_CT = temp.D8[0];
        }
    }
    // 安科瑞PZ系列交流电能表 功率相关
    else if (pbuf[2] == 58)
    {
        // 通讯正常
        Set_AC_METER_Comm(saddr - 1, IsNoFault);

        int DPT = 0x00;
        int DCT = 0x00;
        int DPQ = 0x00;
        int PQ = 0x00;
        int complement = 0;
        for (i = 0; i < 29; i++)
        {
            if (i == 0 || (i >= 6 && i <= 8) || (i >= 12 && i <= 14) ||
                (i >= 16 && i <= 18) || (i >= 20 && i <= 22) || (i >= 24 && i <= 26))
            {
                continue;
            }
            temp.D8[1] = pbuf[3 + i * 2];
            temp.D8[0] = pbuf[4 + i * 2];
            // 小数点 U
            if (i == 1)
            {
                DPT = temp.D8[1];
                DCT = temp.D8[0];
            }
            // 小数点 PQ
            if (i == 2)
            {
                DPQ = temp.D8[1];
                PQ = temp.D8[0];
            }
            // 三相交流电A相电压值
            if (i == 3)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DPT - 4)) * 10;
                SystemTotalData[50501 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 三相交流电B相电压值
            if (i == 4)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DPT - 4)) * 10;
                SystemTotalData[50502 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 三相交流电C相电压值
            if (i == 5)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DPT - 4)) * 10;
                SystemTotalData[50503 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 三相交流电A相电流值
            if (i == 9)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DCT - 4)) * 10;
                SystemTotalData[50504 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 三相交流电B相电流值
            if (i == 10)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DCT - 4)) * 10;
                SystemTotalData[50505 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 三相交流电C相电流值
            if (i == 11)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DCT - 4)) * 10;
                SystemTotalData[50506 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 总有功功率
            if (i == 15)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DPQ - 4)) / 100;
                if (GetBit(PQ, 3) == 1)
                {
                    complement = -temp.D16;
                }
                else
                {
                    complement = temp.D16;
                }
                SystemTotalData[50507 + 20 * (saddr - 1)].D16 = complement >> 16;
                SystemTotalData[50508 + 20 * (saddr - 1)].D16 = complement & 0xffff;
            }
            // 总无功功率
            if (i == 19)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DPQ - 4)) / 100;
                if (GetBit(PQ, 7) == 1)
                {
                    complement = -temp.D16;
                }
                else
                {
                    complement = temp.D16;
                }
                SystemTotalData[50509 + 20 * (saddr - 1)].D16 = complement >> 16;
                SystemTotalData[50510 + 20 * (saddr - 1)].D16 = complement & 0xffff;
            }
            // 总功率因数
            if (i == 23)
            {
                SystemTotalData[50518 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 总视在功率
            if (i == 27)
            {
                temp.D16 = temp.D16 * Magnification_Adjustment(10, (DPQ - 4)) / 100;
                SystemTotalData[50512 + 20 * (saddr - 1)].D16 = temp.D16;
            }
            // 频率
            if (i == 28)
            {
                SystemTotalData[50517 + 20 * (saddr - 1)].D16 = temp.D16;
            }
        }
    }
    // 安科瑞PZ系列交流电能表 电能
    else if (pbuf[2] == 32)
    {
        // 通讯正常
        Set_AC_METER_Comm(saddr - 1, IsNoFault);
        int remainder = 0;
        // 电表正向总有功电度
        temp.D8[1] = pbuf[3];
        temp.D8[0] = pbuf[4];
        remainder = temp.D16 % 1000 * 65536 / 1000;
        SystemTotalData[50513 + 5 * (saddr - 1)].D16 = temp.D16 / 1000;
        temp.D8[1] = pbuf[5];
        temp.D8[0] = pbuf[6];
        SystemTotalData[50514 + 5 * (saddr - 1)].D16 = temp.D16 / 1000 + remainder;
        // 电表反向总有功电度
        temp.D8[1] = pbuf[7];
        temp.D8[0] = pbuf[8];
        remainder = temp.D16 % 1000 * 65536 / 1000;
        SystemTotalData[50515 + 5 * (saddr - 1)].D16 = temp.D16 / 1000;
        temp.D8[1] = pbuf[9];
        temp.D8[0] = pbuf[10];
        SystemTotalData[50516 + 5 * (saddr - 1)].D16 = temp.D16 / 1000 + remainder;
    }
}

double Magnification_Adjustment(int a, int b)
{
    int i = 0;
    double result = 1;

    if (b > 0)
    {
        for (i = 0; i < b; i++)
        {
            result = result * a;
        }
    }
    else if (b < 0)
    {
        b = -b;
        for (i = 0; i < b; i++)
        {
            result = result / a;
        }
    }

    return result;
}

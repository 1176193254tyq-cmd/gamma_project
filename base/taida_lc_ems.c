/*LC通过封装台达PCS的控制点位和反馈点位来适配trina EMS*/
#include "main.h"
#include "taida_lc_ems.h"

extern INT8U BusType;

/*
 * 功能：
 * 根据输入地址进行映射转换
 *
 * 参数：
 *   n     -> 设备编号/索引
 *   addr  -> 原始地址
 *
 * 返回值：
 *   >=0   -> 转换后的地址
 *   -1    -> 地址不在映射范围内
 */
int LC_EMS_Convert_Address(int pcsnum, int addr)
{
    int base = BASE_ADDR + STEP * pcsnum;

    /* 200 ~ 203  -> 103 */
    if (addr >= (base + 200) && addr <= (base + 203))
    {
        return base + 103;//开关机
    }

    /* 208 ~ 211 -> 108 */
    if (addr >= (base + 208) && addr <= (base + 211))
    {
        return base + 53;//无功功率
    }

    /* 212 ~ 215 -> 111 */
    if (addr >= (base + 212) && addr <= (base + 215))
    {
        return base + 111;//孤岛电压
    }

    /* 216 ~ 219 -> 112 */
    if (addr >= (base + 216) && addr <= (base + 219))
    {
        return base + 112;//孤岛频率
    }

    return -1;
}
static void Set_Group_Power_Multi(int pcsnum, int32_t sum, int16_t result[3])
{
    int base;

    if (pcsnum < 0 || pcsnum > 1)
    {
        return;
    }

    if (sum == 0)
    {
        return;
    }

    base = BASE_ADDR + STEP * pcsnum;

    if (sum < 0)
    {
        SET_HOLD(base + 50, POWER_DEFAULT_RATIO);
        SET_HOLD(base + 46, POWER_DEFAULT_RATIO);
        SET_HOLD(base + 47, POWER_DEFAULT_RATIO);

        SET_HOLD(base + 51, result[0]);
        SET_HOLD(base + 48, result[1]);
        SET_HOLD(base + 49, result[2]);
    }
    else
    {
        SET_HOLD(base + 50, result[0]);
        SET_HOLD(base + 46, result[1]);
        SET_HOLD(base + 47, result[2]);

        SET_HOLD(base + 51, POWER_DEFAULT_RATIO);
        SET_HOLD(base + 48, POWER_DEFAULT_RATIO);
        SET_HOLD(base + 49, POWER_DEFAULT_RATIO);
    }

    SET_HOLD(base + 52, (int16_t)sum);

    if (pcsnum == 0)
    {
        Group1_power_multi_Flag = true;
    }
    else
    {
        Group2_power_multi_Flag = true;
    }
}
void LC_EMS_Calc_Power_Percent(int pcsnum)
{
    int16_t value[4];
    int32_t sum = 0;
    int16_t result[4];
    int base = BASE_ADDR + STEP * pcsnum ;
    if(BusType==D_BUS)
    {
    /* 读取4个功率值 */
    for (int i = 0; i < 4; i++)
    {
        value[i] = (int16_t)GET_HOLD(base + 204 + i);

        sum += value[i];
    }

    /* 防止除0 */
    if (sum == 0)
    {
        return;
    }

    /* 计算千分比 */
    for (int i = 0; i < 4; i++)
    {
        result[i] = (int16_t)((value[i] * 1000) / sum);
    }
      Set_Group_Power_Multi(pcsnum,sum,result);

    }
    else
    {
      /* 读取2个功率值 */
    for (int i = 0; i < 2; i++)
    {
        value[i] = (int16_t)GET_HOLD(base + 204 + i);

        sum += value[i];
    }

    /* 防止除0 */
    if (sum == 0)
    {
        return;
    }

    /* 计算千分比 */
    for (int i = 0; i < 2; i++)
    {
        result[i] = (int16_t)((value[i] * 1000) / sum);
    }

      if(sum>0)
      {
        SET_HOLD(base + 50, result[0]);
        SET_HOLD(base + 51,500);
        SET_HOLD(base + 52, sum);
      }
      else
      {
        SET_HOLD(base + 50, 500);
        SET_HOLD(base + 51, result[0]);
        SET_HOLD(base + 52, sum);
      }
    }



}
static void Update_Bit_Status_By_One_Word(int base,
                                          int src_offset,
                                          int dst_offset_8,
                                          int dst_offset_9,
                                          int dst_offset_12,
                                          int dst_offset_13)
{
    uint16_t value;

    value = GET_INPUT(base + src_offset);

    /*
     * bit8 -> dst_offset_8
     * bit10 == 1 时，dst_offset_8 写 2
     */
    if (value & (1U << 10))
    {
        SET_INPUT(base + dst_offset_8, 2);
    }
    else
    {
        SET_INPUT(base + dst_offset_8, (value >> 8) & 0x01);
    }

    /*
     * bit9 -> dst_offset_9
     * bit11 == 1 时，dst_offset_9 写 2
     */
    if (value & (1U << 11))
    {
        SET_INPUT(base + dst_offset_9, 2);
    }
    else
    {
        SET_INPUT(base + dst_offset_9, (value >> 9) & 0x01);
    }

    /* bit12 -> dst_offset_12 */
    SET_INPUT(base + dst_offset_12, (value >> 12) & 0x01);

    /* bit13 -> dst_offset_13 */
    SET_INPUT(base + dst_offset_13, (value >> 13) & 0x01);
}

void Update_Input_Bit_Status(int system_num)
{
    int base = INPUT_BASE_ADDR + INPUT_GROUP_STEP * system_num;

    /* 处理 17000 + 300*n + 61 */
    Update_Bit_Status_By_One_Word(base, 61, 208, 209, 200, 201);

    /* 处理 17000 + 300*n + 62 */
    Update_Bit_Status_By_One_Word(base, 62, 210, 211, 202, 203);

    for(int i=0;i<4;i++)
    {
        SET_INPUT(base + 212 + i, GET_INPUT(base + 43));
    }

}
void Update_Slave_sysstate2(uint16_t Slave_sys_state2_value,uint16_t addr)
{
    uint16_t reg_value;

    /* 先读取17062原值，避免影响其他bit */
    reg_value = GET_INPUT(addr);

    /* 清除 bit8~bit13 */
    reg_value &= ~(0x3F << 8);

    /* 提取 Slave1_sys_state2 的 bit8~bit13 */
    reg_value |= (Slave_sys_state2_value & (0x3F << 8));

    /* 写回 */
    SET_INPUT(addr, reg_value);
}
/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : assist_func.h
 * Project :
 * Description :
 * File Created  : 2022-8-21
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/05
 * Modified By : jinxue.zhao
 * ------------------------------------------------------------
 * HISTORY : 2022/12/05 添加cpp适配宏
 * ------------------------------------------------------------
 * HISTORY : 2022/10/08 修改宏定义
 */

#ifndef ASSIST_FUNC_H
#define ASSIST_FUNC_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CRC16校验码生成
 * @param[in] puchMsg 需要校验的数据
 * @param[in] usDataLen 数据长度
 * @return 校验码，低字节在前，高字节在后
 */
unsigned short crc16(unsigned char* puchMsg, unsigned char usDataLen);


/**
 * @brief 校验故障是否被屏蔽
 * @param[in] data_addr 故障点位
 * @return
 * 1-屏蔽，0-非屏蔽    
 */
unsigned char FAULT_Mask_Check(unsigned short data_addr);

/**
 * @brief 校验逻辑控制是否被屏蔽
 * @param[in] data_addr 逻辑控制点位
 * @return
 * 1-屏蔽，0-非屏蔽    
 */
unsigned char LOGIC_Mask_Check(unsigned short data_addr);






/***************************************自研BMS通信定义************************************/

//获取高8位
#define HI_UINT16(a) (((a) >> 8) & 0xFF)

//获取低8位
#define LO_UINT16(a) ((a) & 0xFF)

//指定的某一位数置1
#define SetBit(x, y)     (x |= (1 << y))

//指定的某一位数置0
#define ClearBit(x, y)   (x &= ~(1 << y))

//指定的某一位数取反
#define ReverseBit(x,y)  (x ^= (1 << y))

//获取的某一位的值
#define GetBit(x, y)     ((x >> y) & 1)

//指定的某一位数置为指定的0或1
#define WriteBit(data, position, flag)   (flag ? SetBit(data, position) : ClearBit(data, position))

//指定的某一位数或为指定的flag
#define OrBit(data, position, flag)  (flag ? (data |= (1 << position)) : (data |= (0 << position)))

//指定的某一位数与为指定的flag
#define AndBit(data, position, flag)  (flag ? (data &= ~(0 << position)) : (data &= ~(1 << position)))

//有效位的验证，校验的位不为0
#define VaildBit(data, offset)    ((data & (1 << offset)) != 0)

//无效位的验证，校验的位为0
#define InvaildBit(data, offset)  ((data & (1 << offset)) == 0)

//位的值是1
#define BIT_ONE   (1)

//位的值是0
#define BIT_ZERO  (0)

#ifdef __cplusplus
}
#endif

#endif  //ASSIST_FUNC_H

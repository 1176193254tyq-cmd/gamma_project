/* ------------------------------------------------------------
 * Copyright (C) 2022
 * File Name : di_e1210.h
 * Project :
 * Description : E1210 DI转eth设备
 * File Created :
 * Author : wujinjin
 * ------------------------------------------------------------
 * Last Modified : 2022/12/14
 * Modified By : jia.liu
 * ------------------------------------------------------------
 * HISTORY : 2022/12/14 修改函数声明
 * ------------------------------------------------------------
 * HISTORY : 2022/11/11 修改宏定义
 */

#ifndef DI_E1210_H
#define DI_E1210_H
#include "system.h"


//读取
#define DI_E1210block1_addr  (0x00)  //0x00 ~ 0x0F，16个DI离散量
#define DI_E1210block1_size  (16)

//读写定义
#define DI_E1210_BUFF_LEN    (64)    //接收的缓存长度

/**
 * @brief E1210设备处理线程
 * @param[in] _item 设备序号
 */
void DI_E1210_Task(INT8U di_num);




#endif //DI_E1210_H

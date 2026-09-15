#ifndef LOG_G2_MV_H
#define LOG_G2_MV_H

#include <stdarg.h>
#include <stdint.h>

#include "system.h"

#ifndef BIT
#define BIT(n) (1u << (n))
#endif

/* 1) 点表：仅描述 bit 含义（可被多个地址复用） */
typedef struct {
    const char *word_name;
    const char *bit_name[16];
} AlarmMap_t;

/* 2) 实例：绑定 addr + 点表 + 独立状态 last_value */
typedef struct {
    uint16_t addr;
    const AlarmMap_t *map;
    uint16_t last_value;
    uint16_t num;
} AlarmInstance_t;

void Alarm_ParseAndLog_All(void); // 解析并记录所有报警

void Trina_Alarm_ParseAndLog_All(void);





#endif
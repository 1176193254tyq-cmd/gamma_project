#ifndef BMS_WRITE_QUEUE_H
#define BMS_WRITE_QUEUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 单条写请求：
 * - 设备仅支持 0x06（写单寄存器）。
 * - 若 is_multi=true 且 len>1，消费端会按 0x06 逐个寄存器写，
 *   数据来源为 GET_HOLD(addr+i)（保持与现有逻辑一致）。
 * - 若 is_multi=false，则使用 value 作为写入值。
 */
typedef struct {
    INT16U addr;     
    INT16U value;    
    INT16U len;     
    bool     is_multi; 
} BmsWriteReq;

/* 可用通道数量 */

#define BMS_WRITEQ_MAX_CHANNELS 10


/* 每个通道的环形队列深度 */
#ifndef BMS_WRITEQ_CAPACITY
#define BMS_WRITEQ_CAPACITY 32
#endif


void bms_write_queue_init(void);


bool bms_write_enqueue(int ch, const BmsWriteReq* r);

/* 入队（按地址去重）：
 * - 若队列中已存在相同 addr 的未出队请求，则覆盖其内容（value/len/is_multi），不改变其相对顺序；
 * - 若不存在，则和 pcs_write_enqueue 一样推入队尾；
 * - 返回 true 表示最终队列中保证存在该地址的一条最新请求。
 */
bool bms_write_enqueue_dedup_by_addr(int ch, const BmsWriteReq* r);

/* 非阻塞出队：有数据则写入 *out 并返回 true；否则返回 false。*/
bool bms_write_try_dequeue(int ch, BmsWriteReq* out);

/* 查询当前队列长度（仅作监控/调试）。*/
size_t bms_write_queue_len(int ch);

#ifdef __cplusplus
}
#endif

#endif /* PCS_WRITE_QUEUE_H */

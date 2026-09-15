#include "bms_write_queue.h"
#include <string.h>
#include <pthread.h>


#ifndef PWQ_LOG
#define PWQ_LOG(fmt, ...)  ((void)0)
#endif

typedef struct {
    pthread_mutex_t mtx;
    BmsWriteReq     buf[BMS_WRITEQ_CAPACITY];
    size_t          head;   /* 出队位置 */
    size_t          tail;   /* 入队位置 */
    size_t          count;  /* 当前元素数 */
    bool            inited;
} BmsWriteQ;

static BmsWriteQ g_q[BMS_WRITEQ_MAX_CHANNELS];
static pthread_once_t g_once = PTHREAD_ONCE_INIT;

static void bms_q_do_init(void)
{
    for (int i=0; i<BMS_WRITEQ_MAX_CHANNELS; ++i) {
        pthread_mutex_init(&g_q[i].mtx, NULL);
        g_q[i].head = g_q[i].tail = g_q[i].count = 0;
        g_q[i].inited = true;
    }
}

void bms_write_queue_init(void)
{
    (void)pthread_once(&g_once, bms_q_do_init);
}

static inline bool ch_ok(int bmsnum)//简短的代码可以直接定义成inline形式，减少调用的开销（在编译的时候将这个函数展开而不是去跳到函数地址进行运行）
{
    return (bmsnum >= 0) && (bmsnum < BMS_WRITEQ_MAX_CHANNELS);
}

static inline void ensure_init(void)
{
    (void)pthread_once(&g_once, bms_q_do_init);//只初始化这个线程一次，即使其他的线程调用它
}

/* 丢弃一个最旧元素（若存在） */
static void drop_oldest_unlocked(BmsWriteQ* q)
{
    if (q->count == 0) return;
    q->head = (q->head + 1) % BMS_WRITEQ_CAPACITY;
    q->count--;
}

/* 追加到队尾（必要时丢弃最旧） */
static void push_back_unlocked(BmsWriteQ* q, const BmsWriteReq* r)
{
    if (q->count == BMS_WRITEQ_CAPACITY) {
        /* 队满：丢弃最旧以让位 */
        drop_oldest_unlocked(q);
    }
    q->buf[q->tail] = *r;
    q->tail = (q->tail + 1) % BMS_WRITEQ_CAPACITY;//形成环形队列
    q->count++;
}

/* 查找相同 addr 的位置（存在返回索引，不存在返回 SIZE_MAX） */
static size_t find_by_addr_unlocked(const BmsWriteQ* q, INT16U addr)
{
    if (q->count == 0) return (size_t)-1;

    size_t idx = q->head;
    for (size_t i=0; i<q->count; ++i) {
        if (q->buf[idx].addr == addr) return idx;
        idx = (idx + 1) % BMS_WRITEQ_CAPACITY;
    }
    return (size_t)-1;
}

bool bms_write_enqueue(int bmsnum, const BmsWriteReq* r)
{
    if (!r) return false;
    ensure_init();
    if (!ch_ok(bmsnum)) return false;

    BmsWriteQ* q = &g_q[bmsnum];
    pthread_mutex_lock(&q->mtx);
    push_back_unlocked(q, r);
    pthread_mutex_unlock(&q->mtx);
    return true;
}

bool bms_write_enqueue_dedup_by_addr(int bmsnum, const BmsWriteReq* r)
{
    if (!r) return false;
    ensure_init();
    if (!ch_ok(bmsnum)) return false;

    BmsWriteQ* q = &g_q[bmsnum];
    pthread_mutex_lock(&q->mtx);

    size_t pos = find_by_addr_unlocked(q, r->addr);
    if (pos != (size_t)-1) {
        /* 覆盖已有项：保持其在队列中的相对顺序 */
        q->buf[pos] = *r;
        pthread_mutex_unlock(&q->mtx);
        return true;
    }

    /* 无相同地址：压入队（必要时丢弃最旧） */
    push_back_unlocked(q, r);
    pthread_mutex_unlock(&q->mtx);
    return true;
}
//取队列
bool bms_write_try_dequeue(int bmsnum, BmsWriteReq* out)
{
    if (!out) return false;
    ensure_init();
    if (!ch_ok(bmsnum)) return false;

    BmsWriteQ* q = &g_q[bmsnum];
    pthread_mutex_lock(&q->mtx);
    if (q->count == 0) {
        pthread_mutex_unlock(&q->mtx);
        return false;
    }
    *out = q->buf[q->head];
    q->head = (q->head + 1) % BMS_WRITEQ_CAPACITY;
    q->count--;
    pthread_mutex_unlock(&q->mtx);
    return true;
}

size_t bms_write_queue_len(int bmsnum)
{
    ensure_init();
    if (!ch_ok(bmsnum)) return 0;

    BmsWriteQ* q = &g_q[bmsnum];
    pthread_mutex_lock(&q->mtx);
    size_t n = q->count;
    pthread_mutex_unlock(&q->mtx);
    return n;
}

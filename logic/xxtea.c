#include "main.h"
#include <string.h>

#define DELTA 0x9E3779B9u

/* 小端装载/回写 */
static inline INT32U rd32le(const INT8U *p){
    return ((INT32U)p[0])       |
           ((INT32U)p[1] << 8)  |
           ((INT32U)p[2] << 16) |
           ((INT32U)p[3] << 24);
}
static inline void wr32le(INT8U *p, INT32U v){
    p[0] = (INT8U)(v      );
    p[1] = (INT8U)(v >> 8 );
    p[2] = (INT8U)(v >> 16);
    p[3] = (INT8U)(v >> 24);
}


/**
 * XXTEA加密算法实现，用于16字节数据块的加密
 * @param block 要加密的16字节数据块
 * @param key 4个32位字的加密密钥
 */
void xxtea_encrypt_16(INT8U block[16], const INT32U key[4]){
    INT32U v[4], z, y, sum = 0, e;  // 定义变量：v[4]存储数据块，z,y,sum,e为中间变量
    const INT32U *k = key;           // 密钥指针
    int n = 4, q = 6 + 52/n; /* 19 */ // 定义循环次数q，n为数据块长度(4个32位字)

    /* bytes -> words (LE) */
    for (int i=0;i<4;i++) v[i] = rd32le(block + 4*i);

    z = v[n-1];
    while (q-- > 0) {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (int p=0; p<n-1; ++p) {
            y = v[p+1];
            v[p] += (( (z>>5) ^ (y<<2) ) + ( (y>>3) ^ (z<<4) )) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z));
            z = v[p];
        }
        y = v[0];
        v[n-1] += (( (z>>5) ^ (y<<2) ) + ( (y>>3) ^ (z<<4) )) ^ ((sum ^ y) + (k[((n-1)&3) ^ e] ^ z));
        z = v[n-1];
    }

    /* words -> bytes (LE) */
    for (int i=0;i<4;i++) wr32le(block + 4*i, v[i]);
}

/**
 * XXTEA算法解密函数，用于解密16字节的块
 * @param block 要解密的16字节数据块
 * @param key 4个32位字的密钥数组
 */
void xxtea_decrypt_16(INT8U block[16], const INT32U key[4]){
    INT32U v[4], z, y, sum, e; // 定义解密所需的变量
    const INT32U *k = key;     // 密钥指针
    int n = 4, q = 6 + 52/n; /* 19 */ // 设置循环参数n和q

    // 将输入的16字节块转换为4个32位字
    for (int i=0;i<4;i++) v[i] = rd32le(block + 4*i);

    y = v[0];                  // 初始化y为第一个字
    sum = (INT32U)(q * DELTA); // 初始化sum值
    while (sum) {              // 主解密循环
        e = (sum >> 2) & 3;    // 计算e值
        // 处理除第一个字外的所有字
        for (int p=n-1; p>0; --p) {
            z = v[p-1];        // 保存前一个字的值
            // XXTEA核心解密操作
            v[p] -= (( (z>>5) ^ (y<<2) ) + ( (y>>3) ^ (z<<4) )) ^ ((sum ^ y) + (k[(p & 3) ^ e] ^ z));
            y = v[p];          // 更新y为当前处理的字
        }
        z = v[n-1];            // 保存最后一个字的值
        // 处理第一个字
        v[0] -= (( (z>>5) ^ (y<<2) ) + ( (y>>3) ^ (z<<4) )) ^ ((sum ^ y) + (k[(0 & 3) ^ e] ^ z));
        y = v[0];              // 更新y为第一个字
        sum -= DELTA;          // 减少sum值
    }

    // 将解密后的4个32位字转换回16字节块
    for (int i=0;i<4;i++) wr32le(block + 4*i, v[i]);
}


bool xxtea_decrypt_bytes(INT8U *dst, const INT8U *src, size_t len, const INT32U key[4]){
    if (!dst || !src || !key) return false;
    if (len < 8 || (len & 3)) return false;

    const int n = (int)(len / 4);
    INT32U vstack[64]; 
    INT32U *v = vstack;
    INT32U *heap = NULL;
    if (n > (int)(sizeof(vstack)/sizeof(vstack[0]))){
        heap = (INT32U*)malloc(len);
        if (!heap) return false;
        v = heap;
    }

    for (int i=0;i<n;i++) v[i] = rd32le(src + 4*i);

    const INT32U *k = key;
    const int q = 6 + 52/n;
    INT32U sum = (INT32U)(q * DELTA);
    INT32U y = v[0], z, e;

    while (sum){
        e = (sum >> 2) & 3u;
        for (int p=n-1; p>0; --p){
            z = v[p-1];
            v[p] -= ((((z>>5) ^ (y<<2)) + ((y>>3) ^ (z<<4))) ^ ((sum ^ y) + (k[(p&3)^e] ^ z)));
            y = v[p];
        }
        z = v[n-1];
        v[0] -= ((((z>>5) ^ (y<<2)) + ((y>>3) ^ (z<<4))) ^ ((sum ^ y) + (k[((0)&3)^e] ^ z)));
        y = v[0];
        sum -= DELTA;
    }

    for (int i=0;i<n;i++) wr32le(dst + 4*i, v[i]);

    if (heap) free(heap);
    return true;
}

size_t pkcs7_unpad4(INT8U *buf, size_t len){
    if (!buf || len < 8) return len;
    INT8U pad = buf[len-1];
    if (pad == 0 || pad > 4) return len;
    if (pad > len) return len;
    for (size_t i=0;i<pad;i++){
        if (buf[len-1-i] != pad) return len;
    }
    return len - pad;
}
size_t pkcs7_pad4(INT8U *dst, const INT8U *src, size_t len){
    memcpy(dst, src, len);

    // 先补到 4 的倍数（PKCS#7：正好对齐也补 4）
    size_t pad4 = (len & 3) ? (4 - (len & 3)) : 4;
    size_t target = len + pad4;

    // 再保证至少 8 字节；但必须“合并”为一次填充，不要分两段
    if (target < 8) {
        pad4 += (8 - target);   // 把“补 8”的差额并入本次 pad
        target = len + pad4;
    }

    for (size_t i = 0; i < pad4; ++i) dst[len + i] = (INT8U)pad4;
    return target; // 返回补完后的总长度
}
 bool xxtea_encrypt_bytes(INT8U *dst, const INT8U *src, size_t len, const INT32U key[4]){
    if (!dst || !src || !key) return false;
    if (len < 8 || (len & 3)) return false;
    const int n = (int)(len/4);

    INT32U vstack[64];
    INT32U *v = vstack, *heap = NULL;
    if (n > (int)(sizeof(vstack)/sizeof(vstack[0]))){
        heap = (INT32U*)malloc(len);
        if (!heap) return false;
        v = heap;
    }
    for (int i=0;i<n;i++) v[i]=rd32le(src+4*i);

    const INT32U *k = key;
    const int q = 6 + 52/n;
    INT32U sum = 0, e, z = v[n-1], y;

    for (int i=0;i<q;i++){
        sum += DELTA; e = (sum>>2) & 3u;
        for (int p=0; p<n-1; ++p){
            y = v[p+1];
            v[p] += ((((z>>5) ^ (y<<2)) + ((y>>3) ^ (z<<4))) ^ ((sum ^ y) + (k[(p&3)^e] ^ z)));
            z = v[p];
        }
        y = v[0];
        v[n-1] += ((((z>>5) ^ (y<<2)) + ((y>>3) ^ (z<<4))) ^ ((sum ^ y) + (k[((n-1)&3)^e] ^ z)));
        z = v[n-1];
    }

    for (int i=0;i<n;i++) wr32le(dst+4*i, v[i]);
    if (heap) free(heap);
    return true;
}
#include "crypto_util.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

bool get_random_bytes(INT8U *out, size_t n){
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0){
        ssize_t r = read(fd, out, n);
        close(fd);
        return r == (ssize_t)n;
    }

    for (size_t i=0;i<n;i++) out[i]=(INT8U)(0xA5 ^ (i*29));//如果打不开的情况下，就自己生成一个
    return false;
}

void make_xxtea_key_from_ascii(const char *ascii, INT32U key[4]){
    INT8U k[16]={0};
    size_t L = ascii? strlen(ascii):0;
    if (L > 16) L = 16;
    memcpy(k, ascii, L);
    // 按小端装入 4×u32
    for (int i=0;i<4;i++){
        key[i] = (INT32U)k[i*4] |
                 ((INT32U)k[i*4+1]<<8) |
                 ((INT32U)k[i*4+2]<<16) |
                 ((INT32U)k[i*4+3]<<24);
    }
}

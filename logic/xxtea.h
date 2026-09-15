#include <stdint.h>
#include "system.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 使用 128-bit key（4×uint32）对 16 字节块进行加/解密 */
void xxtea_encrypt_16(INT8U block[16], const INT32U key[4]);
void xxtea_decrypt_16(INT8U block[16], const INT32U key[4]);

bool xxtea_decrypt_bytes(INT8U *dst , const INT8U *src, size_t len, const INT32U key[4]);


size_t pkcs7_unpad4(INT8U *buf, size_t len);
 size_t pkcs7_pad4(INT8U *dst, const INT8U *src, size_t len);
  bool xxtea_encrypt_bytes(INT8U *dst, const INT8U *src, size_t len, const INT32U key[4]);
#ifdef __cplusplus
}
#endif
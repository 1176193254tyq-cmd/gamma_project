#ifndef USER_DEBUG_H
#define USER_DEBUG_H

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define XINLE_DEBUG    0

int HexToAscii(unsigned char *pHexStr,unsigned char *pAscStr, int Len);


#ifdef __cplusplus
}
#endif

#endif //

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "system.h"

bool get_random_bytes(INT8U *out, size_t n);  
void make_xxtea_key_from_ascii(const char *ascii, INT32U key[4]); 
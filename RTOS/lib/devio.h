#ifndef LIB_DEVIO_H_
#define LIB_DEVIO_H_

#include "stdarg.h"
#include "stdint.h"

#include "BspUart.h"

typedef enum utoa_t {
    utoa_dec = 10,
    utoa_hex = 16
} utoa_t;

uint32_t putstr(const char* s);
uint32_t debug_printf(const char* format, ...);
uint32_t vsprintf(char* buf, const char* format, va_list arg);
//uint32_t utoa(char* buf, uint32_t val, utoa_t base);

#endif // LIB_DEVIO_H_

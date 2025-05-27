#ifndef LIB_DEVIO_H_
#define LIB_DEVIO_H_

#include "stdio.h"
//#include "stdarg.h"
//#include "stdint.h"

#include "BspUart.h"

uint32_t _write(uint32_t file, char *data, uint32_t len);

//typedef enum utoa {
//    utoa_dec = 10,
//    utoa_hex = 16
//} utoa_t;
//
//uint32_t putstr(const char* s);
//uint32_t debug_printf(const char* format, ...);
//uint32_t vsprintf(char* buf, const char* format, va_list arg);
//uint32_t dev_utoa(char* buf, uint32_t val, utoa_t base);

#endif // LIB_DEVIO_H_

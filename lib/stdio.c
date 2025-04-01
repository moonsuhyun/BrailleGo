#include "stdint.h"
#include "HalUart.h"
#include "stdio.h"

#define PRINTF_BUF_LEN 1024

static char printf_buf[PRINTF_BUF_LEN];

uint32_t putstr(const char* s) {
    // putstr 구현
    uint32_t count = 0;
    while(*s) {     // NULL을 만나기 전까지
        Hal_uart_put_char(*s++);
        count++;
    }
    return count;
}

uint32_t debug_printf(const char* format, ...) {
    // debug_printf 구현
    va_list args;
    va_start(args, format);
    vsprintf(printf_buf, format, args);
    va_end(args);
    return putstr(printf_buf);
}

uint32_t vsprintf(char* buf, const char* format, va_list arg) {
    // vsprintf 구현
    uint32_t count = 0;

    char ch;
    char* str;
    uint32_t uint;
    uint32_t hex;

    for (uint32_t i = 0; format[i]; i++) {
        if (format[i] == '%') {
            switch (format[++i]) {
            case 'c':
                ch = (char)va_arg(arg, int32_t);
                buf[count++] = ch;
                break;
            case 's':
                str = (char*)va_arg(arg, char*);
                if (str == (void*)0) str = "(null)";
                while (*str) buf[count++] = *str++;
                break;
            case 'u':
                uint = (uint32_t)va_arg(arg, uint32_t);
                count += utoa(&buf[count], uint, utoa_dec);
                break;
            case 'x':
                hex = (uint32_t)va_arg(arg, uint32_t);
                count += utoa(&buf[count], hex, utoa_hex);
                break;
            }
        } else buf[count++] = format[i];
    }

    if (count >= PRINTF_BUF_LEN) {
        buf[0] = '\0';
        return 0;
    }
    
    buf[count] = '\0';
    return count;
}

uint32_t utoa(char* buf, uint32_t val, utoa_t base) {
    uint32_t count = 0;
    int32_t index = 0;
    char temp[11];   // 0xFFFFFFFF = 4294967295\0
    do {
        uint32_t digit = val % (uint32_t)base;
        if (digit >= 10) digit = 'A' + digit - 10;   // 16진수 처리
        else digit = '0' + digit;
        temp[index++] = digit;
        val /= base;
    } while (val);
    index--;
    while (index >= 0) buf[count++] = temp[index--]; // 순서 반대로 buffer에 추가
    return count;
}

#ifndef LIB_DEVIO_H_
#define LIB_DEVIO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t _write(uint32_t file, char *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // LIB_DEVIO_H_

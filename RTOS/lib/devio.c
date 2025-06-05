#include "devio.h"

#include <stdio.h>

#include "BspUart.h"

uint32_t _write(uint32_t file, char *data, uint32_t len) {
	BSP_UART_Transmit((uint8_t*)data, len);
	return len;
}

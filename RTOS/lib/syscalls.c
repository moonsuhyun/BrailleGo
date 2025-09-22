#include "BspUart.h"
#include "PortCore.h"

uint32_t _write(uint32_t file, char *data, uint32_t len) {
	Port_Core_Disable_PendSV();
	BSP_UART_Transmit((uint8_t*)data, len);
	Port_Core_Enable_PendSV();
	return len;
}

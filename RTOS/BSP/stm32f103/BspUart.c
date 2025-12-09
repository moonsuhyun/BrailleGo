/*
 * BspUart.c
 *
 *  Created on: May 26, 2025
 *      Author: moon
 */

#include "BspUart.h"

#include "Kernel.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart2;

void BSP_UART_Transmit(uint8_t* data, uint32_t len)
{
	HAL_UART_Transmit(&huart2, data, len, HAL_MAX_DELAY);
}

#define UART_RX_BUF_SIZE 1024

typedef struct {
    uint8_t  buf[UART_RX_BUF_SIZE];
    volatile uint16_t head;    // write index (ISR)
    volatile uint16_t tail;    // read index (Task)
} UartRxRing;

static UartRxRing g_uart_rx;

static uint8_t g_rx_byte;

static void UartRx_PutFromISR(uint8_t b)
{
    uint16_t next = (g_uart_rx.head + 1U) % UART_RX_BUF_SIZE;

    if (next == g_uart_rx.tail) {

        return;
    }

    g_uart_rx.buf[g_uart_rx.head] = b;
    g_uart_rx.head = next;

    // Event_Signal(&uart_rx_event);
}

void BSP_UART_RxISRHandler(uint8_t byte)
{
    UartRx_PutFromISR(byte);

    HAL_UART_Receive_IT(&huart2, &g_rx_byte, 1);
}

int32_t BSP_UART_GetChar(uint8_t* out)
{
    if (g_uart_rx.head == g_uart_rx.tail) {
        return 0;
    }

    *out = g_uart_rx.buf[g_uart_rx.tail];
    g_uart_rx.tail = (g_uart_rx.tail + 1U) % UART_RX_BUF_SIZE;
    return 1;
}

int32_t BSP_UART_GetCharBlocking(uint8_t* out)
{
    for (;;) {
        if (BSP_UART_GetChar(out)) {
            return 1;
        }
        Task_Delay(1);
    }
}

// HAL Callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        BSP_UART_RxISRHandler(g_rx_byte);
    }
}

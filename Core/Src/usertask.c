/*
 * usertask.c
 *
 *  Created on: May 21, 2025
 *      Author: moon
 */

#include "usertask.h"

#include "mnist_model.h"
#include "Kernel.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    IR_WAIT_M,
    IR_WAIT_N,
    IR_WAIT_W,
    IR_WAIT_H,
    IR_RECV_DATA,
    IR_RECV_CHK,
} ImgRxState_t;

static uint8_t g_img_buf[2][MNIST_IMG_SIZE];
static SemHandle_t g_img_sem = -1;
static SemHandle_t g_buf_sem = -1;

static int g_write_idx = 0;
static int g_read_idx  = 0;

static int g_img = 0;

static void busy_loop(uint32_t ms)
{
    uint32_t end_time = SysTick_GetTick() + ms;
    while (SysTick_GetTick() < end_time) { __asm volatile("nop"); }
}

static void shell_begin(void)
{
    Mutex_Lock(MUTEX_UART);
    printf("[SH] > ");
    Mutex_Unlock(MUTEX_UART);
}

static void shell_end(void)
{
    // nop
}

static int32_t sImgReceiver(void)
{
    uint16_t idx = 0;
    uint8_t checksum = 0;
    uint8_t byte;
    ImgRxState_t rx_state = IR_WAIT_M;

    for (;;) {

        UART_Read(&byte, 1);

        switch (rx_state) {

        case IR_WAIT_M:
            if (byte == 'M') rx_state = IR_WAIT_N;
            break;

        case IR_WAIT_N:
            if (byte == 'N') rx_state = IR_WAIT_W;
            else rx_state = IR_WAIT_M;
            break;

        case IR_WAIT_W:
            if (byte == MNIST_IMG_W) rx_state = IR_WAIT_H;
            else
            {
                Mutex_Lock(MUTEX_UART);
                printf("[SH] image width error\r\n");
                Mutex_Unlock(MUTEX_UART);
                rx_state = IR_WAIT_M;
            }
            break;

        case IR_WAIT_H:
            if (byte == MNIST_IMG_H)
            {
                rx_state = IR_RECV_DATA;
                idx = 0;
                checksum = 0;
            } else
            {
                Mutex_Lock(MUTEX_UART);
                printf("[SH] image height error\r\n");
                Mutex_Unlock(MUTEX_UART);
                rx_state = IR_WAIT_M;
            }
            break;

        case IR_RECV_DATA:
            Mutex_Lock(MUTEX_UART);
            printf("[SH] %d bytes received\r\n", idx);
            Mutex_Unlock(MUTEX_UART);
            g_img_buf[g_write_idx][idx++] = byte;
            checksum ^= byte;
            if (idx >= MNIST_IMG_SIZE) {
                rx_state = IR_RECV_CHK;
            }
            break;

        case IR_RECV_CHK:
            if (checksum == byte) {
                Semaphore_Signal(g_img_sem);
                g_img++;
                Mutex_Lock(MUTEX_UART);
                printf("[SH] image received OK buf=%d, tick=%u\r\n", g_write_idx, SysTick_GetTick());
                Mutex_Unlock(MUTEX_UART);
                g_write_idx ^= 1;
                rx_state = IR_WAIT_M;
                return 0;
            }
            Mutex_Lock(MUTEX_UART);
            printf("[SH] checksum error\r\n");
            Mutex_Unlock(MUTEX_UART);
            return -2;
            break;
        }
    }
}

void Task_Shell(void* arg)
{
    g_img_sem = Semaphore_Create(0);
    g_buf_sem = Semaphore_Create(2);

    char line[64];

    for (;;) {

        int32_t n = UART_ReadLineEx(line, sizeof(line), shell_begin, shell_end);
        if (n <= 0) continue;

        if (strcmp(line, "help") == 0) {
            Mutex_Lock(MUTEX_UART);
            printf("[SH] commands: help, recv, stat\r\n");
            Mutex_Unlock(MUTEX_UART);
        } else if (strcmp(line, "recv") == 0) {
            Semaphore_Wait(g_buf_sem);

            Mutex_Lock(MUTEX_UART);
            printf("[SH] Ready to receive MNIST image (784 bytes)\r\n");
            Mutex_Unlock(MUTEX_UART);

            int32_t result = sImgReceiver();

            if (result < 0)
            {
                Semaphore_Signal(g_buf_sem);
            }

        } else if (strcmp(line, "stat") == 0) {
            Mutex_Lock(MUTEX_UART);
            printf("[SH] tick=%u, img_ready=%d\r\n", SysTick_GetTick(), Semaphore_GetCount(g_img_sem));
            Mutex_Unlock(MUTEX_UART);
        } else {
            Mutex_Lock(MUTEX_UART);
            printf("[SH] unknown cmd: %s\r\n", line);
            Mutex_Unlock(MUTEX_UART);
        }
    }
}

void Task_Inference(void* arg)
{
    while (g_img_sem < 0)
    {
        Task_Yield();
    }
    for (;;) {
        Semaphore_Wait(g_img_sem);
        g_img--;

        uint32_t t0 = SysTick_GetTick();

        int buf_idx = g_read_idx;
        g_read_idx ^= 1;

        Mutex_Lock(MUTEX_UART);
        printf("[IF] start inference buf=%d, tick=%u\r\n", buf_idx, t0);
        Mutex_Unlock(MUTEX_UART);

        busy_loop(2000);
        int predicted = Mnist_RunInference(g_img_buf[buf_idx]);
        busy_loop(2000);

        uint32_t t1 = SysTick_GetTick();

        Mutex_Lock(MUTEX_UART);
        printf("[IF] done. result=%d, elapsed=%u ms\r\n", predicted, t1 - t0);
        Mutex_Unlock(MUTEX_UART);

        Semaphore_Signal(g_buf_sem);
    }
}

void Task_Heartbeat(void* arg)
{
    for (;;) {
        Task_Delay(1000);

        Mutex_Lock(MUTEX_UART);
        printf("[HB] tick=%u, img_ready=%d\r\n", SysTick_GetTick(), g_img);
        Mutex_Unlock(MUTEX_UART);
    }
}

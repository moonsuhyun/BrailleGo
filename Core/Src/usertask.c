/*
 * usertask.c
 *
 *  Created on: May 21, 2025
 *      Author: moon
 */

#include "usertask.h"

#include "Kernel.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define IMG_W 28
#define IMG_H 28
#define IMG_SIZE (IMG_W * IMG_H)

typedef enum
{
    IR_WAIT_M,
    IR_WAIT_N,
    IR_WAIT_W,
    IR_WAIT_H,
    IR_RECV_DATA,
    IR_RECV_CHK,
} ImgRxState_t;

static uint8_t g_img_buf[IMG_SIZE];
static SemHandle_t g_img_sem = -1;

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
            if (byte == IMG_W) rx_state = IR_WAIT_H;
            else
            {
                Mutex_Lock(MUTEX_UART);
                printf("[SH] image width error\r\n");
                Mutex_Unlock(MUTEX_UART);
                return -1;
            }
            break;

        case IR_WAIT_H:
            if (byte == IMG_H)
            {
                rx_state = IR_RECV_DATA;
                idx = 0;
                checksum = 0;
            } else
            {
                Mutex_Lock(MUTEX_UART);
                printf("[SH] image height error\r\n");
                Mutex_Unlock(MUTEX_UART);
                return -1;
            }
            break;

        case IR_RECV_DATA:
            g_img_buf[idx++] = byte;
            checksum ^= byte;
            if (idx >= IMG_SIZE) {
                rx_state = IR_RECV_CHK;
            }
            break;

        case IR_RECV_CHK:
            if (checksum == byte) {
                Semaphore_Signal(g_img_sem);
                Mutex_Lock(MUTEX_UART);
                printf("[SH] image received OK at tick=%u\r\n", SysTick_GetTick());
                Mutex_Unlock(MUTEX_UART);
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

    char line[64];

    for (;;) {
        UART_ReadLine(line, sizeof(line));

        if (strcmp(line, "help") == 0) {
            Mutex_Lock(MUTEX_UART);
            printf("[SH] commands: help, recv, stat\r\n");
            Mutex_Unlock(MUTEX_UART);
        } else if (strcmp(line, "recv") == 0) {
            Mutex_Lock(MUTEX_UART);
            printf("[SH] Ready to receive MNIST image (784 bytes)\r\n");
            Mutex_Unlock(MUTEX_UART);

            sImgReceiver();

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
    for (;;) {
        Semaphore_Wait(g_img_sem);
        uint32_t t0 = SysTick_GetTick();

        Mutex_Lock(MUTEX_UART);
        printf("[IF] start inference at tick=%u\r\n", t0);
        Mutex_Unlock(MUTEX_UART);

        // 1) g_img_buf -> 입력 텐서로 변환 (정규화, reshape 등)
        // 2) 작은 MLP/CNN 실행 (또는 더미 연산)
        int predicted = Mnist_RunInference(g_img_buf);

        uint32_t t1 = SysTick_GetTick();

        Mutex_Lock(MUTEX_UART);
        printf("[IF] done. result=%d, elapsed=%u ms\r\n", predicted, t1 - t0);
        Mutex_Unlock(MUTEX_UART);
    }
}

void Task_Heartbeat(void* arg)
{
    for (;;) {
        Task_Delay(1000);

        Mutex_Lock(MUTEX_UART);
        printf("[HB] tick=%u, img_ready=%d\r\n", SysTick_GetTick(), Semaphore_GetCount(g_img_sem));
        Mutex_Unlock(MUTEX_UART);
    }
}

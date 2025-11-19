/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <Kernel.h>

#include <stdio.h>

#include "usertask.h"

static void sKernel_Init();
static void sTask_Init();

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    Kernel_Start(sTask_Init);

    while (1)
    {
    }
}

static void sTask_Init(void)
{
    int task_id;
    task_id = Task_Create(Task_L, NULL, 2);
    task_id = Task_Create(Task_M, NULL, 1);
    task_id = Task_Create(Task_H, NULL, 0);

    // Task_Create(TaskA, NULL, 0);
    // Task_Create(TaskB, NULL, 0);

    while (1)
    {
        Task_Yield();
    }
}

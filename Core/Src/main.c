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
    sKernel_Init();

    while (1)
    {
    }
}

static void sKernel_Init(void)
{
    Kernel_Init();

    printf("Kernel has initiated...\r\n");

    sTask_Init();

    Kernel_Start();
}

static void sTask_Init(void)
{
    Kernel_Create(TaskA);
    Kernel_Create(TaskB);
    //	Kernel_Create(TaskC);
    //	Kernel_Create(TaskD);
    //	Kernel_Create(TaskE);
    //	Kernel_Create(TaskF);
    //	Kernel_Create(TaskG);
    //	Kernel_Create(TaskH);
}

/**
 ******************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 *          of the TIM instances.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* TIM6 init function */
void MX_TIM6_Init(void)
{
    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);

    /* TIM6 interrupt Init */
    NVIC_SetPriority(TIM6_DAC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(TIM6_DAC_IRQn);

    TIM_InitStruct.Prescaler = 8400 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 1000 - 1;
    LL_TIM_Init(TIM6, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM6);
    LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM6);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

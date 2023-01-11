/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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
#include "gpio.h"

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PC9   ------> RCC_MCO_2
     PA8   ------> RCC_MCO_1
*/
void MX_GPIO_Init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);

    /**/
    LL_GPIO_SetOutputPin(GPIOE, RTD_NCS0_Pin | RTD_NCS4_Pin | RTD_NCS5_Pin);

    /**/
    LL_GPIO_SetOutputPin(GPIOA, LAN_NRST_Pin | VIBR_NSYNC_Pin);

    /**/
    LL_GPIO_SetOutputPin(GPIOD, VIBR_NPWD0_Pin | VIBR_NPWD1_Pin | VIBR_NPWD2_Pin | VIBR_NPWD3_Pin
                                    | VIBR_NPWD4_Pin | VIBR_NPWD5_Pin | VIBR_NPWD6_Pin | RTD_NCS3_Pin
                                    | RTD_NCS2_Pin | RTD_NCS7_Pin | RTD_NCS6_Pin);

    /**/
    LL_GPIO_SetOutputPin(GPIOC, VIBR_NPWD7_Pin | VIBR_NCS_Pin);

    /**/
    LL_GPIO_SetOutputPin(GPIOB, EXT_SPI_NCS_Pin | RTD_NCS1_Pin);

    /**/
    LL_GPIO_ResetOutputPin(GPIOC, PC14_Pin | PC15_Pin);

    /**/
    GPIO_InitStruct.Pin = RTD_NCS0_Pin | RTD_NCS4_Pin | RTD_NCS5_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = RTD_NRDY0_Pin | RTD_NRDY4_Pin | RTD_NRDY5_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = PC14_Pin | PC15_Pin | VIBR_NPWD7_Pin | VIBR_NCS_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = LAN_NRST_Pin | VIBR_NSYNC_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = VIBR_NPWD0_Pin | VIBR_NPWD1_Pin | VIBR_NPWD2_Pin | VIBR_NPWD3_Pin
                        | VIBR_NPWD4_Pin | VIBR_NPWD5_Pin | VIBR_NPWD6_Pin | RTD_NCS3_Pin
                        | RTD_NCS2_Pin | RTD_NCS7_Pin | RTD_NCS6_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = RCC_LAN_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(RCC_LAN_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = RCC_ADC_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(RCC_ADC_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = RTD_NDRY3_Pin | RTD_NDRY2_Pin | RTD_NDRY7_Pin | RTD_NDRY6_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = EXT_SPI_NCS_Pin | RTD_NCS1_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = RTD_NRDY1_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(RTD_NRDY1_GPIO_Port, &GPIO_InitStruct);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_ll_tim.h"

#include "gpio_ex.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
    while (1) {
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/
void DMA1_Stream0_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC0(DMA1)) {
        LL_DMA_ClearFlag_TC0(DMA1);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
        DMA1_SPI3_ReceiveComplete_Callback();
    }
    if (LL_DMA_IsActiveFlag_TE0(DMA1)) {
        LL_DMA_ClearFlag_TE0(DMA1);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_0);
    }
}

void DMA1_Stream5_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC5(DMA1)) {
        LL_DMA_ClearFlag_TC5(DMA1);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
    }
    if (LL_DMA_IsActiveFlag_TE5(DMA1)) {
        LL_DMA_ClearFlag_TE5(DMA1);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
    }
}

void DMA1_Stream3_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC3(DMA1)) {
        LL_DMA_ClearFlag_TC3(DMA1);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
    }
    if (LL_DMA_IsActiveFlag_HT3(DMA1)) {
        LL_DMA_ClearFlag_HT3(DMA1);
        DMA1_SPI2_HalfReceiveComplete_Callback();
    }
    if (LL_DMA_IsActiveFlag_TE3(DMA1)) {
        LL_DMA_ClearFlag_TE3(DMA1);
        LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_3);
    }
    test_pin15_toggle();
}

void DMA1_Stream4_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC4(DMA1)) {
        LL_DMA_ClearFlag_TC4(DMA1);
    }
    if (LL_DMA_IsActiveFlag_TE4(DMA1)) {
        LL_DMA_ClearFlag_TE4(DMA1);
    }
    LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_4);
    test_pin15_toggle();
}

void EXTI9_5_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7) != RESET) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
        LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_3);
        LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_4);

        test_pin14_toggle();
    }
}

void TIM6_DAC_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM6)) {
        LL_TIM_ClearFlag_UPDATE(TIM6);
        TIM6_Update_Callback();
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

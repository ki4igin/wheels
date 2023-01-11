
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define RTD_NCS0_Pin           LL_GPIO_PIN_2
#define RTD_NCS0_GPIO_Port     GPIOE
#define RTD_NRDY0_Pin          LL_GPIO_PIN_3
#define RTD_NRDY0_GPIO_Port    GPIOE
#define RTD_NCS4_Pin           LL_GPIO_PIN_4
#define RTD_NCS4_GPIO_Port     GPIOE
#define RTD_NRDY4_Pin          LL_GPIO_PIN_5
#define RTD_NRDY4_GPIO_Port    GPIOE
#define PC14_Pin               LL_GPIO_PIN_14
#define PC14_GPIO_Port         GPIOC
#define PC15_Pin               LL_GPIO_PIN_15
#define PC15_GPIO_Port         GPIOC
#define LAN_NRST_Pin           LL_GPIO_PIN_5
#define LAN_NRST_GPIO_Port     GPIOA
#define VIBR_SCK_Pin           LL_GPIO_PIN_10
#define VIBR_SCK_GPIO_Port     GPIOB
#define VIBR_MISO_Pin          LL_GPIO_PIN_14
#define VIBR_MISO_GPIO_Port    GPIOB
#define VIBR_MOSI_Pin          LL_GPIO_PIN_15
#define VIBR_MOSI_GPIO_Port    GPIOB
#define VIBR_NPWD0_Pin         LL_GPIO_PIN_9
#define VIBR_NPWD0_GPIO_Port   GPIOD
#define VIBR_NPWD1_Pin         LL_GPIO_PIN_10
#define VIBR_NPWD1_GPIO_Port   GPIOD
#define VIBR_NPWD2_Pin         LL_GPIO_PIN_11
#define VIBR_NPWD2_GPIO_Port   GPIOD
#define VIBR_NPWD3_Pin         LL_GPIO_PIN_12
#define VIBR_NPWD3_GPIO_Port   GPIOD
#define VIBR_NPWD4_Pin         LL_GPIO_PIN_13
#define VIBR_NPWD4_GPIO_Port   GPIOD
#define VIBR_NPWD5_Pin         LL_GPIO_PIN_14
#define VIBR_NPWD5_GPIO_Port   GPIOD
#define VIBR_NPWD6_Pin         LL_GPIO_PIN_15
#define VIBR_NPWD6_GPIO_Port   GPIOD
#define VIBR_NPWD7_Pin         LL_GPIO_PIN_6
#define VIBR_NPWD7_GPIO_Port   GPIOC
#define VIBR_NCS_Pin           LL_GPIO_PIN_7
#define VIBR_NCS_GPIO_Port     GPIOC
#define RCC_LAN_Pin            LL_GPIO_PIN_9
#define RCC_LAN_GPIO_Port      GPIOC
#define RCC_ADC_Pin            LL_GPIO_PIN_8
#define RCC_ADC_GPIO_Port      GPIOA
#define VIBR_NSYNC_Pin         LL_GPIO_PIN_15
#define VIBR_NSYNC_GPIO_Port   GPIOA
#define RTD_SCK_Pin            LL_GPIO_PIN_10
#define RTD_SCK_GPIO_Port      GPIOC
#define RTD_MISO_Pin           LL_GPIO_PIN_11
#define RTD_MISO_GPIO_Port     GPIOC
#define RTD_MOSI_Pin           LL_GPIO_PIN_12
#define RTD_MOSI_GPIO_Port     GPIOC
#define RTD_NCS3_Pin           LL_GPIO_PIN_0
#define RTD_NCS3_GPIO_Port     GPIOD
#define RTD_NDRY3_Pin          LL_GPIO_PIN_1
#define RTD_NDRY3_GPIO_Port    GPIOD
#define RTD_NCS2_Pin           LL_GPIO_PIN_2
#define RTD_NCS2_GPIO_Port     GPIOD
#define RTD_NDRY2_Pin          LL_GPIO_PIN_3
#define RTD_NDRY2_GPIO_Port    GPIOD
#define RTD_NCS7_Pin           LL_GPIO_PIN_4
#define RTD_NCS7_GPIO_Port     GPIOD
#define RTD_NDRY7_Pin          LL_GPIO_PIN_5
#define RTD_NDRY7_GPIO_Port    GPIOD
#define RTD_NCS6_Pin           LL_GPIO_PIN_6
#define RTD_NCS6_GPIO_Port     GPIOD
#define RTD_NDRY6_Pin          LL_GPIO_PIN_7
#define RTD_NDRY6_GPIO_Port    GPIOD
#define EXT_SPI_SCK_Pin        LL_GPIO_PIN_3
#define EXT_SPI_SCK_GPIO_Port  GPIOB
#define EXT_SPI_MISO_Pin       LL_GPIO_PIN_4
#define EXT_SPI_MISO_GPIO_Port GPIOB
#define EXT_SPI_MOSI_Pin       LL_GPIO_PIN_5
#define EXT_SPI_MOSI_GPIO_Port GPIOB
#define EXT_SPI_NCS_Pin        LL_GPIO_PIN_6
#define EXT_SPI_NCS_GPIO_Port  GPIOB
#define RTD_NCS1_Pin           LL_GPIO_PIN_8
#define RTD_NCS1_GPIO_Port     GPIOB
#define RTD_NRDY1_Pin          LL_GPIO_PIN_9
#define RTD_NRDY1_GPIO_Port    GPIOB
#define RTD_NCS5_Pin           LL_GPIO_PIN_0
#define RTD_NCS5_GPIO_Port     GPIOE
#define RTD_NRDY5_Pin          LL_GPIO_PIN_1
#define RTD_NRDY5_GPIO_Port    GPIOE

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

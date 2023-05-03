#include "main.h"
#include "lwip.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "debug.h"
#include "udp_server.h"
#include "ads1278.h"
#include "ads1220.h"
#include "gpio_ex.h"
#include "settings.h"

void SystemClock_Config(void);


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    settings_init();

    /* Initialize all configured peripherals */    
    MX_GPIO_Init();
    MX_LWIP_Init();
    MX_USART1_UART_Init();
    MX_SPI1_Init();

    debug_printf("\nDEV: init start\n");
    

    delay_ms(10);
    ads1278_init();
    ads1220_init();
    udp_server_init();
    debug_printf("DEV: init complete\n");
    test_pin14_enable();
    test_pin15_disable();

    while (1) {
        MX_LWIP_Process();
        if (ads1278_pac_iscomplete) {
            ads1278_pac_iscomplete = 0;            
            udp_server_send_vibr(ads1278_pac, ads1278_pac_size);
        }
        if (ads1220_pac_iscomplete) {
            ads1220_pac_iscomplete = 0;
            udp_server_send_rtd(ads1220_pac, 40);
        }
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
    while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5) {
    }
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    LL_RCC_HSE_EnableBypass();
    LL_RCC_HSE_Enable();

    /* Wait till HSE is ready */
    while (LL_RCC_HSE_IsReady() != 1) {
    }
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, 336, LL_RCC_PLLP_DIV_2);
    LL_RCC_PLL_Enable();

    /* Wait till PLL is ready */
    while (LL_RCC_PLL_IsReady() != 1) {
    }
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    /* Wait till System clock is ready */
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {
    }
    LL_SetSystemCoreClock(168000000);

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
        Error_Handler();
    }
    LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_1);
    LL_RCC_ConfigMCO(LL_RCC_MCO2SOURCE_HSE, LL_RCC_MCO2_DIV_1);    
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
}

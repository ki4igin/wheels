#include "ads1220.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

uint8_t rx_buf[BUF_SIZE] = {0};
uint8_t tx_buf[BUF_SIZE] = {0};

struct ads1220_pac *ads1220_pac;
uint32_t ads1220_pac_iscomplete = 1;

struct ads1220_pac ads1220_pacs[2] = {
    [0] = {.id = 1, .cnt = 0, .data = {0}},
    [1] = {.id = 1, .cnt = 1, .data = {0}}
};

static void nrdy_int_init()
{
    LL_EXTI_InitTypeDef EXTI_InitStruct = {0};

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE7);

    /**/
    EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_7;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;
    LL_EXTI_Init(&EXTI_InitStruct);

    /**/
    LL_GPIO_SetPinPull(VIBR_NDRDY_GPIO_Port, VIBR_NDRDY_Pin, LL_GPIO_PULL_NO);

    /**/
    LL_GPIO_SetPinMode(VIBR_NDRDY_GPIO_Port, VIBR_NDRDY_Pin, LL_GPIO_MODE_INPUT);

    /* EXTI interrupt init*/
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

static void nrdy_int_enable()
{
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

static void nrdy_int_disable()
{
    NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void ads1220_init(void)
{
    MX_SPI3_Init();
    LL_SPI_Enable(SPI3);
       

}

void ads1220_start()
{
}

void ads1220_stop()
{

}


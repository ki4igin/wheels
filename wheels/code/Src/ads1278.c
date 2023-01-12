#include "ads1278.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

uint8_t rx_buf[BUF_SIZE] = {0};
uint8_t tx_buf[BUF_SIZE] = {0};

struct ads1278_pac *ads1278_pac;
uint32_t ads1278_pac_iscomplete = 1;

struct ads1278_pac ads1278_pacs[2] = {
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

void ads1278_init(void)
{
    MX_SPI2_Init();
    MX_DMA_SPI2_Init(tx_buf, rx_buf, BUF_SIZE);

    LL_SPI_Enable(SPI2);

    nrdy_int_init();
}

void ads1278_start()
{
    nrdy_int_enable();
}

void ads1278_stop()
{
    nrdy_int_disable();
}

void DMA1_HalfReceiveComplete_Callback()
{
    static uint32_t cnt = 0;
    static uint32_t pac_num = 0;
    uint32_t cnt_bytes = cnt * BUF_SIZE / 2;

    if (cnt_bytes == (PAC_DATA_SIZE * BUF_SIZE)) {
        cnt = 0;
        cnt_bytes = 0;
        ads1278_pac = &ads1278_pacs[pac_num];
        ads1278_pac->cnt += 2;
        ads1278_pac_iscomplete = 1;
        pac_num = (pac_num + 1) & 0x01;
    }

    uint32_t *src;
    if ((cnt & 0x1) == 0) {
        src = (uint32_t *)&rx_buf[0];
    } else {
        src = (uint32_t *)&rx_buf[BUF_SIZE / 2];
    }

    uint32_t *dst = (uint32_t *)&ads1278_pacs[pac_num].data[cnt_bytes];
    for (uint32_t i = 0; i < BUF_SIZE / 2 / 4; i++) {
        *dst++ = *src++;
    }

    cnt++;
}

#include "ads1278.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "gpio_ex.h"

#define ADS1278_BUF_SIZE      (3 * 8)
#define ADS1278_SAMPLES_COUNT 24

static uint8_t rx_buf[ADS1278_BUF_SIZE] = {0};
static uint8_t tx_buf[ADS1278_BUF_SIZE] = {0};

struct ads1278_pac *ads1278_pac;
uint32_t ads1278_pac_iscomplete = 1;

uint8_t *pac_data;
uint32_t sample_num = {0};
uint32_t pac_num = {0};

struct ads1278_pac ads1278_pacs[2] = {
    [0] = {.id = 1, .cnt = 0, .data = {0}},
    [1] = {.id = 1, .cnt = 1, .data = {0}}
};

static void nrdy_int_init(void)
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
    MX_DMA_SPI2_Init(tx_buf, rx_buf, ADS1278_BUF_SIZE);

    LL_SPI_Enable(SPI2);

    nrdy_int_init();
}

void ads1278_start()
{
    pac_num = 0;
    ads1278_pacs[pac_num & 0x01].cnt = pac_num;
    pac_data = ads1278_pacs[pac_num & 0x01].data;
    sample_num = 0;
    MX_DMA_SPI2_SetRxAddr(pac_data);

    nrdy_int_enable();
}

void ads1278_stop()
{
    nrdy_int_disable();
}

void DMA1_SPI2_ReceiveComplete_Callback(void)
{
    if (++sample_num == ADS1278_SAMPLES_COUNT) {
        ads1278_pac = ads1278_pacs[pac_num & 0x01].data;
        ads1278_pac_iscomplete = 1;
        pac_num++;

        ads1278_pacs[pac_num & 0x01].cnt = pac_num;
        pac_data = ads1278_pacs[pac_num & 0x01].data;
        sample_num = 0;
        MX_DMA_SPI2_SetRxAddr(pac_data);

        return;
    }

    pac_data += ADS1278_BUF_SIZE;
    MX_DMA_SPI2_SetRxAddr(pac_data);
}

void DMA1_SPI2_HalfReceiveComplete_Callback()
{
    // static uint32_t cnt = 0;
    // static uint32_t pac_num = 0;
    // uint32_t cnt_bytes = cnt * ADS1278_BUF_SIZE / 2;

    // if (cnt_bytes == (ADS1278_PAC_DATA_SIZE * ADS1278_BUF_SIZE)) {
    //     cnt = 0;
    //     cnt_bytes = 0;
    //     ads1278_pac = &ads1278_pacs[pac_num];
    //     ads1278_pac->cnt += 2;
    //     ads1278_pac_iscomplete = 1;
    //     pac_num = (pac_num + 1) & 0x01;
    // }

    // uint32_t *src;
    // if ((cnt & 0x1) == 0) {
    //     src = (uint32_t *)&rx_buf[0];
    // } else {
    //     src = (uint32_t *)&rx_buf[ADS1278_BUF_SIZE / 2];
    // }

    // uint32_t *dst = (uint32_t *)&ads1278_pacs[pac_num].data[cnt_bytes];
    // for (uint32_t i = 0; i < ADS1278_BUF_SIZE / 2 / 4; i++) {
    //     *dst++ = *src++;
    // }

    // // test_pin15_toggle();

    // cnt++;
}

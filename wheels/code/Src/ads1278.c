#include "ads1278.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "gpio_ex.h"
#include "stm32f4xx_ll_gpio.h"
#include "debug.h"

#define ADS1278_CH_DATA_SIZE  3
#define ADS1278_BUF_MAX_SIZE  (ADS1278_CH_DATA_SIZE * 8)
#define ADS1278_SAMPLES_COUNT 24

const static struct gpio ads_npwd_pins[] = {
    {VIBR_NPWD0_GPIO_Port, VIBR_NPWD0_Pin},
    {VIBR_NPWD1_GPIO_Port, VIBR_NPWD1_Pin},
    {VIBR_NPWD2_GPIO_Port, VIBR_NPWD2_Pin},
    {VIBR_NPWD3_GPIO_Port, VIBR_NPWD3_Pin},
    {VIBR_NPWD4_GPIO_Port, VIBR_NPWD4_Pin},
    {VIBR_NPWD5_GPIO_Port, VIBR_NPWD5_Pin},
    {VIBR_NPWD6_GPIO_Port, VIBR_NPWD6_Pin},
    {VIBR_NPWD7_GPIO_Port, VIBR_NPWD7_Pin}
};

static uint8_t rx_buf[ADS1278_BUF_MAX_SIZE] = {0};
static uint8_t tx_buf[ADS1278_BUF_MAX_SIZE] = {0};

struct ads1278_pac *ads1278_pac;
uint32_t ads1278_pac_iscomplete = 0;
uint32_t ads1278_pac_size = ADS1278_SAMPLES_COUNT * ADS1278_BUF_MAX_SIZE;

uint8_t *pac_data;
volatile uint32_t sample_num = {0};
volatile uint32_t pac_num = {0};
static uint32_t buf_size = ADS1278_BUF_MAX_SIZE;

static struct ads1278_pac ads1278_pacs[4] = {
    [0] = {.id = 1, .cnt = 0, .data = {0}},
    [1] = {.id = 1, .cnt = 1, .data = {0}},
    [2] = {.id = 1, .cnt = 1, .data = {0}},
    [3] = {.id = 1, .cnt = 1, .data = {0}},
};

inline static void set_buf_size(uint32_t ch_count)
{
    if (ch_count == 0 || ch_count > 8) {
        return;
    }
    buf_size = ADS1278_CH_DATA_SIZE * ch_count;
    uint32_t pac_data_size = buf_size * ADS1278_SAMPLES_COUNT;
    ads1278_pac_size =
        sizeof(struct ads1278_pac) - ADS1278_PAC_MAX_SIZE + pac_data_size;
    MX_DMA_SPI2_SetSize(buf_size);
}

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
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 4));
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
    MX_DMA_SPI2_Init(tx_buf, rx_buf, ADS1278_BUF_MAX_SIZE);

    LL_GPIO_ResetOutputPin(VIBR_NPWD6_GPIO_Port, VIBR_NPWD6_Pin);
    LL_GPIO_ResetOutputPin(VIBR_NPWD7_GPIO_Port, VIBR_NPWD7_Pin);
    set_buf_size(6);

    LL_SPI_Enable(SPI2);

    nrdy_int_init();

    debug_printf("VIBR ADC Init Complete\n");
}

void ads1278_start()
{
    ads1278_pac_iscomplete = 0;
    pac_num = 0;
    ads1278_pacs[pac_num & 0x01].cnt = pac_num;
    pac_data = ads1278_pacs[pac_num & 0x01].data;
    sample_num = 0;
    // volatile uint8_t temp = SPI2->DR;
    // (void)temp;

    MX_DMA_SPI2_SetRxAddr(pac_data);

    nrdy_int_enable();
}

void ads1278_stop()
{
    nrdy_int_disable();
}

void DMA1_SPI2_ReceiveComplete_Callback(void)
{
    pac_data += buf_size;

    if (++sample_num == ADS1278_SAMPLES_COUNT) {
        sample_num = 0;

        ads1278_pac = &ads1278_pacs[pac_num & 0x03];
        ads1278_pac_iscomplete = 1;
        pac_num++;
        if (ads1278_pac->data[0] != 0xFF) {
            test_pin14_toggle();
            // debug_printf("= %d", ads1278_pac->data[0]);
            // delay_ms(100);
            // NVIC_SystemReset();
        }

        ads1278_pacs[pac_num & 0x03].cnt = pac_num;
        pac_data = ads1278_pacs[pac_num & 0x03].data;
    }

    MX_DMA_SPI2_SetRxAddr(pac_data);
}

void ads1278_setch(uint32_t mask)
{
    if (mask == 0 || mask > 0xFF) {
        return;
    }

    // Если включены все каналы, то минимальный предделитель для ADC CLK равен 2
    if ((mask == 0xFF) && ((RCC->CFGR & RCC_CFGR_MCO1PRE_Msk) == 0)) {
        ads1278_setdiv(2);
    }

    uint32_t ch_cnt = 0;
    for (uint32_t i = 0; i < 8; i++) {
        struct gpio gpio = ads_npwd_pins[i];
        if (mask & (1 << i)) {
            ch_cnt++;
            LL_GPIO_SetOutputPin(gpio.port, gpio.pin);
        } else {
            LL_GPIO_ResetOutputPin(gpio.port, gpio.pin);
        }
    }
    set_buf_size(ch_cnt);
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

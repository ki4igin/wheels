#include "ads1220.h"
#include "tools.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "tim.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "ads1220_regs.h"
#include "debug.h"

#define ADS_CNT   8

#define CMD_RESET 0x06
#define CMD_START 0x08
#define CMD_PDOWN 0x02
#define CMD_RDATA 0x10
#define CMD_RREG  0x20
#define CMD_WREG  0x40

struct reg_map reg_map = {
    .r0 = {.mux = 6, .gain = 3, .pga_pypass = 0},
    .r1 = {.bcs = 0, .ts = 0, .cm = 1, .mode = 0, .dr = 0},
    .r2 = {.idac = 5, .psw = 0, .f50_60 = 1, .vref = 1},
    .r3 = {.reserved = 0, .drdym = 0, .i1mux = 3, .i2mux = 4}
};

static uint8_t rx_buf[ADS1220_BUF_SIZE] = {0};
static uint8_t tx_buf[ADS1220_BUF_SIZE] = {0};

volatile uint32_t ads1220_pac_iscomplete = 1;

struct ads1220_pac *ads1220_pac;

struct ads1220_pac ads1220_pacs = {
    .id = 2,
    .cnt = 0,
    .data = {0}};

volatile uint32_t data_cnt;

struct gpio {
    GPIO_TypeDef *port;
    uint32_t pin;
};

const static struct gpio ads_cs_pins[ADS_CNT] = {
    {RTD_NCS3_GPIO_Port, RTD_NCS3_Pin},
    {RTD_NCS2_GPIO_Port, RTD_NCS2_Pin},
    {RTD_NCS7_GPIO_Port, RTD_NCS7_Pin},
    {RTD_NCS6_GPIO_Port, RTD_NCS6_Pin},
    {RTD_NCS1_GPIO_Port, RTD_NCS1_Pin},
    {RTD_NCS5_GPIO_Port, RTD_NCS5_Pin},
    {RTD_NCS0_GPIO_Port, RTD_NCS0_Pin},
    {RTD_NCS4_GPIO_Port, RTD_NCS4_Pin},
};

static struct gpio ads_cs;
static uint32_t ads_num;

static void continue_send_process(void)
{
    ads_cs = ads_cs_pins[ads_num];
    LL_GPIO_ResetOutputPin(ads_cs.port, ads_cs.pin);
    MX_DMA_SPI3_Start();
}

static void start_send_process(void)
{
    ads_num = 0;
    continue_send_process();
}

static void ads1220_write_regmap(void)
{
    MX_DMA_SPI3_SetSize(5);

    tx_buf[0] = CMD_WREG | 3,
    *(uint32_t *)&tx_buf[1] = *(uint32_t *)&reg_map;
    // memcpy_u32(&reg_map, &tx_buf[1], 1);

    start_send_process();
}

void ads1220_rreg()
{
    data_cnt = 0;
    ads1220_pac_iscomplete = 0;
    // чтение одного 3 регистра
    tx_buf[0] = CMD_RREG | (3 << 2) | 0;
    MX_DMA_SPI3_SetSize(2);
    start_send_process();
}

void ads1220_init(void)
{
    ads1220_pac = &ads1220_pacs;
    MX_SPI3_Init();
    MX_DMA_SPI3_Init(tx_buf, rx_buf, ADS1220_BUF_SIZE);
    LL_SPI_Enable(SPI3);
    debug_printf("RTD ADC Init Start\n");

    debug_printf(" reg map:\n");
    debug_printf(
        " r0: 0x%02x; r1: 0x%02x; r2: 0x%02x; r3: 0x%02x\n",
        reg_map.r0.val, reg_map.r1.val, reg_map.r2.val, reg_map.r3.val);

    ads1220_write_regmap();
    delay_ms(3);
    for (uint32_t i = 0; i < ADS1220_BUF_SIZE; i++) {
        tx_buf[i] = 0;
    }
    ads1220_rreg();
    while (ads1220_pac_iscomplete == 0) {
        ;
    }
    for (uint32_t i = 0; i < ADS_CNT; i++) {
        if (ads1220_pacs.data[i] == reg_map.r3.val) {
            debug_printf(" ADC%d: ok\n", i + 1);
        } else {
            debug_printf(" ADC%d: error\n", i + 1);
        }
    }
    debug_printf("RTD ADC Init Complete\n");
    ads1220_start_conv();

    MX_TIM6_Init();
    LL_TIM_ClearFlag_UPDATE(TIM6);
    LL_TIM_EnableIT_UPDATE(TIM6);
}

void ads1220_rdata()
{
    data_cnt = 0;
    ads1220_pac_iscomplete = 0;
    tx_buf[0] = CMD_RDATA;
    MX_DMA_SPI3_SetSize(4);
    start_send_process();
}

void ads1220_start_conv()
{
    tx_buf[0] = CMD_START;
    MX_DMA_SPI3_SetSize(1);
    start_send_process();
}

void ads1220_start(void)
{
    LL_TIM_EnableCounter(TIM6);
}

void ads1220_stop()
{
    LL_TIM_DisableCounter(TIM6);
}

void DMA1_SPI3_ReceiveComplete_Callback(void)
{
    LL_GPIO_SetOutputPin(ads_cs.port, ads_cs.pin);

    uint32_t cmd = tx_buf[0];
    switch (cmd) {
    case CMD_RDATA: {
        memcpy_u8(&rx_buf[1], &ads1220_pacs.data[data_cnt], 3);
        // debug_printf("0x%02x 0x%02x 0x%02x\n", rx_buf[1], rx_buf[2], rx_buf[3]);
        data_cnt += 3;
    } break;
    case (CMD_RREG | (3 << 2) | 0): {
        ads1220_pacs.data[data_cnt] = rx_buf[1];
        data_cnt++;
    } break;
    default:
        break;
    }

    if (++ads_num == ADS_CNT) {
        switch (cmd) {
        case CMD_RDATA:
            ads1220_pacs.cnt++;
            ads1220_pac_iscomplete = 1;
            break;
        case (CMD_RREG | (3 << 2) | 0):
            ads1220_pac_iscomplete = 1;
            break;
        default:
            break;
        }

        return;
    }
    // debug_printf("%d\n", ads_num);

    continue_send_process();
}

void TIM6_Update_Callback(void)
{
    ads1220_rdata();
}

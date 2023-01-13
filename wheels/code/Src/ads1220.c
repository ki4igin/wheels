#include "ads1220.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "ads1220_regs.h"

#define CMD_RESET 0x06
#define CMD_START 0x08
#define CMD_PDOWN 0x02
#define CMD_RDATA 0x10
#define CMD_RREG  0x20
#define CMD_WREG  0x40

struct reg_map reg_map = {
    .r0 = {.mux = 0, .gain = 0, .pga_pypass = 0},
    .r1 = {.bcs = 0, .ts = 0, .cm = 1, .mode = 0, .dr = 0},
    .r2 = {.idac = 5, .psw = 0, .f50_60 = 1, .vref = 1  },
    .r3 = {.reserved = 0, .drdym = 0, .i1mux = 4, .i2mux = 8}
};

static uint8_t rx_buf[ADS1220_BUF_SIZE] = {0};
static uint8_t tx_buf[ADS1220_BUF_SIZE] = {0};

uint32_t ads1220_pac_iscomplete = 1;

struct ads1220_pac *ads1220_pac;

struct ads1220_pac ads1220_pacs = {
    .id = 2,
    .cnt = 0,
    .data = {0}};

uint8_t *ads1220_pac_cur_data;

struct cs_gpio {
    GPIO_TypeDef *port;
    uint32_t pin;
};

#define ADS_CNT 8

const static struct cs_gpio ads1220_cs[ADS_CNT] = {
    {RTD_NCS0_GPIO_Port, RTD_NCS0_Pin},
    {RTD_NCS1_GPIO_Port, RTD_NCS1_Pin},
    {RTD_NCS2_GPIO_Port, RTD_NCS2_Pin},
    {RTD_NCS3_GPIO_Port, RTD_NCS3_Pin},
    {RTD_NCS4_GPIO_Port, RTD_NCS4_Pin},
    {RTD_NCS5_GPIO_Port, RTD_NCS5_Pin},
    {RTD_NCS6_GPIO_Port, RTD_NCS6_Pin},
    {RTD_NCS7_GPIO_Port, RTD_NCS7_Pin}
};

static struct cs_gpio cs_current;
static uint32_t cs_current_num;

void ads1220_init(void)
{
    MX_SPI3_Init();
    MX_DMA_SPI3_Init(tx_buf, rx_buf, ADS1220_BUF_SIZE);
    LL_SPI_Enable(SPI3);
}

void ads1220_start()
{
    cs_current_num = 0;
    cs_current = ads1220_cs[cs_current_num];
    ads1220_pac_cur_data = ads1220_pacs.data;

    LL_GPIO_ResetOutputPin(cs_current.port, cs_current.pin);
    MX_DMA_SPI3_SetSize(4);

    tx_buf[0] = CMD_RDATA;
    MX_DMA_SPI3_Start();
}

void ads1220_start_conv()
{
    cs_current_num = 0;
    cs_current = ads1220_cs[cs_current_num];

    LL_GPIO_ResetOutputPin(cs_current.port, cs_current.pin);
    MX_DMA_SPI3_SetSize(1);

    tx_buf[0] = CMD_START;
    MX_DMA_SPI3_Start();
}

void ads1220_stop()
{
}

void DMA1_SPI3_ReceiveComplete_Callback(void)
{
    LL_GPIO_SetOutputPin(cs_current.port, cs_current.pin);

    uint32_t cmd = tx_buf[0];
    switch (cmd) {
    case CMD_RDATA: {
        uint8_t *src = &rx_buf[1];
        for (uint32_t i = 0; i < 3; i++) {
            *ads1220_pac_cur_data++ = *src++;
        }
    } break;
    default:
        break;
    }

    if (++cs_current_num == ADS_CNT) {
        switch (cmd) {
        case CMD_RDATA:
            ads1220_pacs.cnt++;
            ads1220_pac_iscomplete = 1;
            break;
        default:
            break;
        }

        return;
    }

    cs_current = ads1220_cs[cs_current_num];
    LL_GPIO_ResetOutputPin(cs_current.port, cs_current.pin);
    MX_DMA_SPI3_Start();
}

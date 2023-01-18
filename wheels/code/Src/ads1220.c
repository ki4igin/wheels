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
#include "gpio_ex.h"

#define ADS_CNT   1

#define CMD_RESET 0x06
#define CMD_START 0x08
#define CMD_PDOWN 0x02
#define CMD_RDATA 0x10
#define CMD_RREG  0x20
#define CMD_WREG  0x40

const struct reg_map reg_map_work = {
    .r0 = {.mux = 6, .gain = 4, .pga_pypass = 0},
    .r1 = {.bcs = 0, .ts = 0, .cm = 1, .mode = 0, .dr = 0},
    .r2 = {.idac = 5, .psw = 0, .f50_60 = 0, .vref = 1},
    .r3 = {.reserved = 0, .drdym = 0, .i1mux = 3, .i2mux = 4}
};
// const struct reg_map reg_map_work = {
//     .r0 = {.mux = 14, .gain = 4, .pga_pypass = 0},
//     .r1 = {.bcs = 0, .ts = 0, .cm = 1, .mode = 0, .dr = 0},
//     .r2 = {.idac = 0, .psw = 0, .f50_60 = 1, .vref = 0},
//     .r3 = {.reserved = 0, .drdym = 0, .i1mux = 0, .i2mux = 0}
// };
const struct reg_map reg_map_calibr = {
    .r0 = {.mux = 14, .gain = 4, .pga_pypass = 0},
    .r1 = {.bcs = 0, .ts = 0, .cm = 1, .mode = 0, .dr = 0},
    .r2 = {.idac = 0, .psw = 0, .f50_60 = 0, .vref = 0},
    .r3 = {.reserved = 0, .drdym = 0, .i1mux = 0, .i2mux = 0}
};

static uint8_t rx_buf[ADS1220_BUF_SIZE] = {0};
static uint8_t tx_buf[ADS1220_BUF_SIZE] = {0};

volatile uint32_t ads1220_pac_iscomplete = 1;

struct ads1220_pac *ads1220_pac;

static uint32_t is_calibr = 0;
static int32_t calibr_arr[ADS_CNT];

struct ads1220_pac ads1220_pacs = {
    .id = 2,
    .cnt = 0,
    .data = {0}};

static uint8_t *pac_data;

struct gpio {
    GPIO_TypeDef *port;
    uint32_t pin;
};

const static struct gpio ads_cs_pins[ADS_CNT] = {
    // {RTD_NCS3_GPIO_Port, RTD_NCS3_Pin},
    // {RTD_NCS2_GPIO_Port, RTD_NCS2_Pin},
    // {RTD_NCS7_GPIO_Port, RTD_NCS7_Pin},
    // {RTD_NCS6_GPIO_Port, RTD_NCS6_Pin},
    // {RTD_NCS1_GPIO_Port, RTD_NCS1_Pin},
    // {RTD_NCS5_GPIO_Port, RTD_NCS5_Pin},
    {RTD_NCS0_GPIO_Port, RTD_NCS0_Pin},
    // {RTD_NCS4_GPIO_Port, RTD_NCS4_Pin},
};

static struct gpio ads_cs;
static uint32_t ads_num;

static void continue_send_process(void)
{
    ads_cs = ads_cs_pins[ads_num];
    LL_GPIO_ResetOutputPin(ads_cs.port, ads_cs.pin);
    MX_DMA_SPI3_SetRxAddr(pac_data);
    MX_DMA_SPI3_Start();
}

static void start_send_process(void)
{
    ads_num = 0;
    pac_data = ads1220_pacs.data;
    continue_send_process();
}

static void ads1220_reset()
{
    tx_buf[0] = CMD_RESET;
    MX_DMA_SPI3_SetSize(1);
    start_send_process();
}

static void ads1220_start_conv()
{
    tx_buf[0] = CMD_START;
    MX_DMA_SPI3_SetSize(1);
    start_send_process();
}

static void ads1220_write_regmap(const struct reg_map *preg_map)
{

    tx_buf[0] = CMD_WREG | 3,
    *(uint32_t *)&tx_buf[1] = *(uint32_t *)preg_map;
    // memcpy_u32(&reg_map, &tx_buf[1], 1);

    MX_DMA_SPI3_SetSize(5);
    start_send_process();
}

static void ads1220_rreg()
{
    ads1220_pac_iscomplete = 0;
    // чтение одного 3-го регистра
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

    ads1220_reset();
    debug_printf("RTD ADC Init Start\n");
    debug_printf(" reg map:\n");
    debug_printf(
        " r0: 0x%02x; r1: 0x%02x; r2: 0x%02x; r3: 0x%02x\n",
        reg_map_work.r0.val, reg_map_work.r1.val,
        reg_map_work.r2.val, reg_map_work.r3.val);

    ads1220_write_regmap(&reg_map_work);
    delay_ms(5);
    arr_clear_u8(tx_buf, ADS1220_BUF_SIZE);

    ads1220_rreg();
    while (ads1220_pac_iscomplete == 0) {
        ;
    }
    for (uint32_t i = 0; i < ADS_CNT; i++) {
        if (ads1220_pacs.data[4 * i + 1] == reg_map_work.r3.val) {
            debug_printf(" ADC%d: ok\n", i + 1);
        } else {
            debug_printf(" ADC%d: error\n", i + 1);
        }
    }
    arr_clear_u8(ads1220_pacs.data, ADS1220_PAC_DATA_SIZE);

    debug_printf("RTD ADC Init Complete\n");

    MX_TIM6_Init();
    LL_TIM_ClearFlag_UPDATE(TIM6);
    LL_TIM_EnableIT_UPDATE(TIM6);
}

static void ads1220_rdata()
{
    ads1220_pac_iscomplete = 0;
    tx_buf[0] = 0;
    MX_DMA_SPI3_SetSize(3);
    start_send_process();
}

void ads1220_start_calibr(void)
{
    arr_clear_u32(calibr_arr, ADS_CNT);
    ads1220_reset();
    debug_printf("RTD ADC Calibration Start\n");
    debug_printf(" reg map:\n");
    debug_printf(
        " r0: 0x%02x; r1: 0x%02x; r2: 0x%02x; r3: 0x%02x\n",
        reg_map_calibr.r0.val, reg_map_calibr.r1.val,
        reg_map_calibr.r2.val, reg_map_calibr.r3.val);

    ads1220_write_regmap(&reg_map_calibr);
    delay_ms(5);
    arr_clear_u8(tx_buf, ADS1220_BUF_SIZE);
    arr_clear_u8(ads1220_pacs.data, ADS1220_PAC_DATA_SIZE);

    is_calibr = 1;
    ads1220_start();
}

static void ads1220_stop_calibr(void)
{
    ads1220_reset();

    debug_printf("RTD ADC Calibration Complete\n");
    for (uint32_t i = 0; i < ADS_CNT; i++) {
        calibr_arr[i] /= (int32_t)ads1220_pacs.cnt;
        debug_printf(" ADC%d val: %d\n", i + 1, calibr_arr[i]);
    }
    ads1220_write_regmap(&reg_map_work);
    delay_ms(5);
    arr_clear_u8(tx_buf, ADS1220_BUF_SIZE);
    arr_clear_u8(ads1220_pacs.data, ADS1220_PAC_DATA_SIZE);
}

void ads1220_start(void)
{
    ads1220_start_conv();
    ads1220_pacs.cnt = 0;
    LL_TIM_SetCounter(TIM6, 0);
    LL_TIM_EnableCounter(TIM6);
}

void ads1220_stop()
{
    LL_TIM_DisableCounter(TIM6);
    if (is_calibr) {
        is_calibr = 0;
        ads1220_stop_calibr();
    }
}

void ads1220_offset_corr(void *data)
{
    if (is_calibr)
    {
        return;
    }
    
    int32_t *p = data;
    for (uint32_t i = 0; i < ADS_CNT; i++) {
        p[i] = __REV(__REV(p[i]) - calibr_arr[i]);
    }
}

void DMA1_SPI3_ReceiveComplete_Callback(void)
{
    LL_GPIO_SetOutputPin(ads_cs.port, ads_cs.pin);

    uint32_t cmd = tx_buf[0];
    pac_data += 4;

    if (++ads_num == ADS_CNT) {
        switch (cmd) {
        case 0:
            if (is_calibr) {
                for (uint32_t i = 0; i < ADS_CNT; i++) {
                    calibr_arr[i] += __REV(ads1220_pacs.adc_val[i]);
                }
            }
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

    continue_send_process();
}

void TIM6_Update_Callback(void)
{
    test_pin14_toggle();
    ads1220_rdata();
}

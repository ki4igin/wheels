#ifndef __ADS1278_H
#define __ADS1278_H

#define ADS1278_PAC_MAX_SIZE (3 * 8 * 24)

#include <stdint.h>
#include "stm32f4xx_ll_rcc.h"
#include "debug.h"

struct ads1278_pac {
    const uint32_t id;
    uint32_t cnt;
    uint8_t data[ADS1278_PAC_MAX_SIZE];    
};

extern struct ads1278_pac *ads1278_pac;
extern uint32_t ads1278_pac_iscomplete;
extern uint32_t ads1278_pac_size;

void ads1278_init(void);
void ads1278_start(void);
void ads1278_stop(void);
void ads1278_setch(uint32_t mask);

static inline void ads1278_setdiv(uint32_t div)
{
    switch (div)
    {
    case 1:
        LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_1);
        break;
    case 2:
        LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_2);
        break;
    case 3:
        LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_3);
        break;
    case 4:
        LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_4);
        break;
    case 5:
        LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_HSE, LL_RCC_MCO1_DIV_5);
        break;    
    default:
        debug_printf("VIBR ADC: div %d not correct\n", div);
        return;
    }
    debug_printf("VIBR ADC: div changed to %d\n", div);
}

#endif

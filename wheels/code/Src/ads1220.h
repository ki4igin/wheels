#ifndef __ADS1220_H
#define __ADS1220_H

#define ADS1220_BUF_SIZE      5
#define ADS1220_PAC_DATA_SIZE 32

#include <stdint.h>

struct ads1220_pac {
    const uint32_t id;
    uint32_t cnt;
    union {
        uint8_t data[ADS1220_PAC_DATA_SIZE];
        int32_t adc_val[ADS1220_PAC_DATA_SIZE / 4];
    };
};

extern struct ads1220_pac *ads1220_pac;
extern volatile uint32_t ads1220_pac_iscomplete;

void ads1220_init(void);
void ads1220_start(void);
void ads1220_start_calibr(void);
void ads1220_stop(void);
void ads1220_offset_corr(void *data);

#endif

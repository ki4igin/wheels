#ifndef __ADS1220_H
#define __ADS1220_H

#define ADS1220_BUF_SIZE      5
#define ADS1220_PAC_DATA_SIZE 24

#include <stdint.h>

struct ads1220_pac {
    const uint32_t id;
    uint32_t cnt;
    uint8_t data[ADS1220_PAC_DATA_SIZE];
};

extern struct ads1220_pac *ads1220_pac;
extern uint32_t ads1220_pac_iscomplete;

void ads1220_init(void);
void ads1220_start(void);
void ads1220_start_conv(void);
void ads1220_stop(void);

#endif

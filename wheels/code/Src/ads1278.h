#ifndef __ADS1278_H
#define __ADS1278_H

#define BUF_SIZE      (3 * 8)
#define PAC_DATA_SIZE 24

#include <stdint.h>

struct ads1278_pac {
    const uint32_t id;
    uint32_t cnt;
    uint8_t data[PAC_DATA_SIZE * BUF_SIZE];
};

extern struct ads1278_pac *ads1278_pac;
extern uint32_t ads1278_pac_iscomplete;

void ads1278_init(void);
void ads1278_start(void);
void ads1278_stop(void);

#endif

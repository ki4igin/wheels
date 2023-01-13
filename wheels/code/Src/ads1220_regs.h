#ifndef __ADS1220_REGS_H
#define __ADS1220_REGS_H

#include <stdint.h>
#include <assert.h>

union r0 {
    struct
    {
        uint8_t pga_pypass :1;
        uint8_t gain       :3;
        uint8_t mux        :4;
    };

    uint8_t val;
};

union r1 {
    struct
    {
        uint8_t bcs  :1;
        uint8_t ts   :1;
        uint8_t cm   :1;
        uint8_t mode :2;
        uint8_t dr   :3;
    };

    uint8_t val;
};

union r2 {
    struct
    {
        uint8_t idac   :3;
        uint8_t psw    :1;
        uint8_t f50_60 :2;
        uint8_t vref   :2;
    };

    uint8_t val;
};

union r3 {
    struct
    {
        uint8_t reserved :1;
        uint8_t drdym    :1;
        uint8_t i2mux    :3;
        uint8_t i1mux    :3;
    };

    uint8_t val;
};

static_assert(sizeof(union r0) == 1, "size error r0");
static_assert(sizeof(union r1) == 1, "size error r1");
static_assert(sizeof(union r2) == 1, "size error r2");
static_assert(sizeof(union r3) == 1, "size error r3");

struct reg_map {
    union r0 r0;
    union r1 r1;
    union r2 r2;
    union r3 r3;
};

#endif

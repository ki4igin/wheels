#ifndef __TOOLS_H
#define __TOOLS_H

#include <stdint.h>

#define arrlen(_arr) (sizeof(_arr) / sizeof((_arr)[0]))

inline static void memcpy_u8(void *src, void *dst, uint32_t size)
{
    uint8_t *s = src;
    uint8_t *d = dst;
    do {
        *d++ = *s++;
    } while (--size);
}

inline static void memcpy_u16(void *src, void *dst, uint32_t size)
{
    size >>= 1; 
    uint16_t *s = src;
    uint16_t *d = dst;
    do {
        *d++ = *s++;
    } while (--size);
}

inline static void memcpy_u32(void *src, void *dst, uint32_t size)
{
    size >>= 2; 
    uint32_t *s = src;
    uint32_t *d = dst;
    do {
        *d++ = *s++;
    } while (--size);
}

#endif

#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>

void flash_memcpy_u32(uint32_t *src, uint32_t *dst, uint32_t size);

#endif

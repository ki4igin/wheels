#include "stm32f4xx.h"

#define FLASH_KEY1 0x45670123U
#define FLASH_KEY2 0xCDEF89ABU

static void flash_unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

static void flash_program_u32(uint32_t adr, uint32_t data)
{
    while (FLASH->SR & FLASH_SR_BSY) {
        ;
    }

    FLASH->CR |= FLASH_CR_PG;
    *(__IO uint32_t *)adr = data;

    while (FLASH->SR & FLASH_SR_BSY) {
        ;
    }
}

void flash_erase_sector(uint32_t sector)
{
    while (FLASH->SR & FLASH_SR_BSY) {
        ;
    }

    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_PSIZE_WORD;

    FLASH->CR &= ~FLASH_CR_SNB;
    FLASH->CR |= FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY) {
        ;
    }
}

void flash_memcpy_u32(uint32_t *src, uint32_t *dst, uint32_t size)
{
    flash_unlock();

    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_PSIZE_WORD;

    size >>= 2;
    do {
        flash_program_u32((uint32_t)dst++, *src++);
    } while (size--);

    flash_lock();
}

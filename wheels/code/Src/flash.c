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

static void flash_clear_err(void)
{
    FLASH->SR |= FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR
               | FLASH_SR_WRPERR;
}

static void flash_program_u32(uint32_t adr, uint32_t data)
{
    while (FLASH->SR & FLASH_SR_BSY) {
        ;
    }

    *(__IO uint32_t *)adr = data;

    while ((FLASH->SR & FLASH_SR_EOP) != FLASH_SR_EOP) {
        ;
    }
    FLASH->SR |= FLASH_SR_EOP;
}

void flash_erase_sector(uint32_t sector)
{
    flash_unlock();
    flash_clear_err();

    while (FLASH->SR & FLASH_SR_BSY) {
        ;
    }

    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_PSIZE_WORD;

    FLASH->CR &= ~FLASH_CR_SNB;
    FLASH->CR |= FLASH_CR_EOPIE | FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos);
    FLASH->CR |= FLASH_CR_STRT;

    while ((FLASH->SR & FLASH_SR_EOP) != FLASH_SR_EOP) {
        ;
    }
    FLASH->SR |= FLASH_SR_EOP;

    FLASH->CR &= ~(FLASH_CR_EOPIE | FLASH_CR_SER);
    flash_lock();
}

void flash_memcpy_u32(void *src, void *dst, uint32_t size)
{
    flash_unlock();
    flash_clear_err();

    FLASH->CR &= ~FLASH_CR_PSIZE;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_EOPIE | FLASH_CR_PG;

    uint32_t *s = src;
    uint32_t *d = dst;

    size >>= 2;
    do {
        flash_program_u32((uint32_t)d++, *s++);
    } while (--size);

    FLASH->CR &= ~(FLASH_CR_EOPIE | FLASH_CR_PG);
    flash_lock();
}

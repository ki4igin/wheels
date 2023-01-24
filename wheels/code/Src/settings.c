#include <stdint.h>

#include "settings.h"
#include "flash.h"

#define SETTINGS_SECTOR_NUM  7
#define SETTINGS_SECTOR_ADDR FLASH_SECTOR7_ADDR

union settings *settings = (union settings *)SETTINGS_SECTOR_ADDR;

void settings_init(void)
{
    union settings *s = (union settings *)SETTINGS_SECTOR_ADDR;

    if (s->magic_key != SETTINGS_MAGIC_KEY) {
        union settings s_default = {
            .magic_key = SETTINGS_MAGIC_KEY,
            .ip_addr = {192, 168, 0, 10},
            {0}
        };
        settings_write(&s_default);
    }

    while (s->magic_key == SETTINGS_MAGIC_KEY) {
        s++;
    }

    s--;

    settings = s;
}

void settings_write(union settings *s_new)
{
    settings++;
    flash_memcpy_u32((uint32_t *)s_new, (uint32_t *)&settings, sizeof(union settings));
}

void settings_change_ipaddr(uint8_t ipaddr_new[4])
{
    union settings s_new = *settings;
    *(uint32_t*)s_new.ip_addr = *(uint32_t*)ipaddr_new;
    settings_write(&s_new);
}

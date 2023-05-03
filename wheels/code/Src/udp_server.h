#ifndef __UDP_SERVER_H
#define __UDP_SERVER_H

#include "stdint.h"

void udp_server_init(void);
void udp_server_send_rtd(void *data, uint32_t size);
void udp_server_send_vibr(void *data, uint32_t size);

#endif

#include "udp_server.h"

#include "udp.h"
#include "debug.h"
#include "ads1278.h"

#define UDP_SERVER_PORT                      5

#define cmd2uint(char1, char2, char3, char4) ((char1 << 24) + (char2 << 16) + (char3 << 8) + char4)

const uint8_t IP_DEFAULT[4] = {192, 168, 0, 10};

const enum cmd {
    CMD_CONNECT = cmd2uint('c', 'm', 'd', '0'),
    CMD_DISCONNECT = cmd2uint('c', 'm', 'd', '1'),
    CMD_START = cmd2uint('c', 'm', 'd', '2'),
    CMD_STOP = cmd2uint('c', 'm', 'd', '3'),
    CMD_ECHO = cmd2uint('c', 'm', 'd', '9')
} cmd;

enum udp_server_status {
    UDP_SERVER_DISCONNECTED,
    UDP_SERVER_CONNECTED
} udp_server_status = UDP_SERVER_DISCONNECTED;

enum status {
    STATUS_STOPPED,
    STATUS_STARTED
} status = STATUS_STOPPED;

struct udp_pcb *pcb;

struct ip4_addr ip;

static void try_connect(enum cmd cmd, const ip_addr_t *addr, uint16_t port)
{
    if (cmd == CMD_CONNECT) {
        udp_connect(pcb, addr, port);
        udp_server_status = UDP_SERVER_CONNECTED;
        char *ip_client = ipaddr_ntoa(addr);
        debug_printf("client connected, ip: %s\n", ip_client);
    }
}

static void cmd_work(enum cmd cmd, struct pbuf *p)
{
    switch (cmd) {
    case CMD_DISCONNECT:
        status = STATUS_STOPPED;
        delay_ms(10);
        udp_disconnect(pcb);
        udp_server_status = UDP_SERVER_DISCONNECTED;
        debug_printf("client disconnected\n");
        break;
    case CMD_START:
        status = STATUS_STARTED;
        debug_printf("start work\n");
        ads1278_start();
        break;
    case CMD_STOP:
        status = STATUS_STOPPED;
        debug_printf("stop work\n");
         ads1278_stop();
        break;
    case CMD_ECHO:
        // udp_send(pcb, p);
        break;
    default:
        return;
    }
    udp_send(pcb, p);
}

static void recv_callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port)
{
    enum cmd cmd = (enum cmd)__REV(*(uint32_t *)(p->payload));

    if (udp_server_status == UDP_SERVER_DISCONNECTED) {
        try_connect(cmd, addr, port);
    }

    if (udp_server_status == UDP_SERVER_CONNECTED) {
        cmd_work(cmd, p);
    }
}

void udp_server_init(void)
{
    err_t err = ERR_OK;

    pcb = udp_new();

    ip.addr = (*(uint32_t *)IP_DEFAULT);

    udp_bind(pcb, &ip, UDP_SERVER_PORT);
    udp_recv(pcb, recv_callback, NULL);
}

void udp_server_send(void *data, uint32_t size)
{
    if (status != STATUS_STARTED) {
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
    pbuf_take(p, data, size);
    udp_send(pcb, p);
    pbuf_free(p);
}

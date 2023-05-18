#include "udp_server.h"

#include "udp.h"
#include "debug.h"
#include "ads1278.h"
#include "ads1220.h"
#include "settings.h"

#define UDP_SERVER_PORT_CMD  2020

#define UDP_CLIENT_PORT_CMD  2020
#define UDP_CLIENT_PORT_RTD  2021
#define UDP_CLIENT_PORT_VIBR 2022

#define cmd2uint(char1, char2, char3, char4) \
    ((char4 << 24) + (char3 << 16) + (char2 << 8) + char1)

const enum cmd {
    CMD_CONNECT = cmd2uint('c', 'm', 'd', '0'),
    CMD_DISCONNECT = cmd2uint('c', 'm', 'd', '1'),
    CMD_START_VIBR = cmd2uint('c', 'm', 'd', '2'),
    CMD_STOP = cmd2uint('c', 'm', 'd', '3'),
    CMD_START_RTD = cmd2uint('c', 'm', 'd', '4'),
    CMD_SETDIV_VIBR = cmd2uint('c', 'm', 'd', '5'),
    CMD_SETCH_VIBR = cmd2uint('c', 'm', 'd', '6'),
    CMD_CHANGE_IP = cmd2uint('c', 'm', 'd', '7'),
    // CMD_START_RTD_CALIBR = cmd2uint('c', 'm', 'd', '5'),
    // CMD_START_VIBR_CALIBR = cmd2uint('c', 'm', 'd', '6'),
    CMD_ECHO = cmd2uint('c', 'm', 'd', '9'),
    CMD_RESET = cmd2uint('c', 'm', 'd', 'r')
} cmd;

enum udp_server_status {
    UDP_SERVER_DISCONNECTED,
    UDP_SERVER_CONNECTED
} udp_server_status = UDP_SERVER_DISCONNECTED;

enum status {
    STATUS_STOPPED,
    STATUS_STARTED
} status = STATUS_STOPPED;

struct udp_pcb *pcb_cmd_s;
struct udp_pcb *pcb_cmd_c;
struct udp_pcb *pcb_rtd_c;
struct udp_pcb *pcb_vibr_c;
struct ip4_addr ip;

static void connect(const ip_addr_t *addr)
{
    udp_connect(pcb_cmd_c, addr, UDP_CLIENT_PORT_CMD);
    udp_connect(pcb_rtd_c, addr, UDP_CLIENT_PORT_RTD);
    udp_connect(pcb_vibr_c, addr, UDP_CLIENT_PORT_VIBR);
}

static void disconnect(void)
{
    udp_disconnect(pcb_cmd_c);
    udp_disconnect(pcb_rtd_c);
    udp_disconnect(pcb_vibr_c);
}

static uint32_t try_connect(enum cmd cmd, const ip_addr_t *addr)
{
    if (cmd == CMD_CONNECT) {
        connect(addr);
        udp_server_status = UDP_SERVER_CONNECTED;
        char *ip_client = ipaddr_ntoa(addr);
        debug_printf("DEV: client %s connected\n", ip_client);
        return 1;
    }
    return 0;
}

static void cmd_work(enum cmd cmd, struct pbuf *p)
{
    uint32_t arg = 0;
    switch (cmd) {
    case CMD_DISCONNECT:
        status = STATUS_STOPPED;
        delay_ms(10);
        disconnect();
        udp_server_status = UDP_SERVER_DISCONNECTED;
        debug_printf("DEV: client disconnected\n");
        break;
    case CMD_START_VIBR:
        status = STATUS_STARTED;
        ads1278_start();
        break;
    case CMD_START_RTD:
        status = STATUS_STARTED;
        ads1220_start();
        break;
    case CMD_STOP:
        status = STATUS_STOPPED;
        ads1278_stop();
        ads1220_stop();
        break;
    case CMD_RESET:
        status = STATUS_STOPPED;
        ads1278_stop();
        ads1220_stop();
        debug_printf("DEV: reset...\n");
        delay_ms(100);
        udp_send(pcb_cmd_c, p);
        NVIC_SystemReset();
        break;
    case CMD_CHANGE_IP:
        status = STATUS_STOPPED;
        ads1278_stop();
        ads1220_stop();
        delay_ms(10);
        disconnect();
        udp_server_status = UDP_SERVER_DISCONNECTED;
        debug_printf("DEV: client disconnected\n");

        ip.addr = *(uint32_t *)(p->payload + 4);
        char *ip_str = ipaddr_ntoa(&ip);
        debug_printf("DEV: ip change to %s\n", ip_str);

        settings_change_ipaddr(ip.addr);

        debug_printf("DEV: reset...\n");
        delay_ms(100);
        NVIC_SystemReset();

        break;
    case CMD_SETDIV_VIBR:
        status = STATUS_STOPPED;
        ads1278_stop();
        arg = *(uint32_t *)(p->payload + 4);
        ads1278_setdiv(arg);
        break;
    case CMD_SETCH_VIBR:
        status = STATUS_STOPPED;
        ads1278_stop();
        arg = *(uint32_t *)(p->payload + 4);
        ads1278_setch(arg);
        break;
    case CMD_ECHO:
        break;
    default:
        return;
    }
    udp_send(pcb_cmd_c, p);
}

static void recv_callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port)
{
    enum cmd cmd = (enum cmd)(*(uint32_t *)(p->payload));

    if (udp_server_status == UDP_SERVER_DISCONNECTED) {
        if (try_connect(cmd, addr)) {
            udp_send(pcb_cmd_c, p);
            ;
        }
    }

    if (udp_server_status == UDP_SERVER_CONNECTED) {
        cmd_work(cmd, p);
    }
}

void udp_server_init(void)
{
    err_t err = ERR_OK;

    pcb_cmd_s = udp_new();
    pcb_cmd_c = udp_new();
    pcb_vibr_c = udp_new();
    pcb_rtd_c = udp_new();

    ip.addr = (*(uint32_t *)settings->ip_addr);

    udp_bind(pcb_cmd_s, &ip, UDP_SERVER_PORT_CMD);
    udp_recv(pcb_cmd_s, recv_callback, NULL);

    char *ip_str = ipaddr_ntoa(&ip);
    debug_printf("DEV: server %s:%d started\n", ip_str, UDP_SERVER_PORT_CMD);
    debug_printf("DEV:  cmd port %d\n", UDP_CLIENT_PORT_CMD);
    debug_printf("DEV:  rtd port %d\n", UDP_CLIENT_PORT_RTD);
    debug_printf("DEV: vibr port %d\n", UDP_CLIENT_PORT_VIBR);
}

void udp_server_send_vibr(void *data, uint32_t size)
{
    if (status != STATUS_STARTED) {
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
    pbuf_take(p, data, size);
    udp_send(pcb_vibr_c, p);
    pbuf_free(p);
}

void udp_server_send_rtd(void *data, uint32_t size)
{
    if (status != STATUS_STARTED) {
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
    pbuf_take(p, data, size);
    udp_send(pcb_rtd_c, p);
    pbuf_free(p);
}

#include "udp_server.h"

#include "udp.h"
#include "debug.h"
#include "ads1278.h"
#include "ads1220.h"
#include "settings.h"
#include "err.h"

#define SERVER_PORT_CMD  2020

#define CLIENT_PORT_CMD  2020
#define CLIENT_PORT_RTD  2021
#define CLIENT_PORT_VIBR 2022

#define cmd2uint(char1, char2, char3, char4) \
    ((char4 << 24) + (char3 << 16) + (char2 << 8) + char1)

enum {
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
};

struct cmd {
    uint32_t id;
    uint32_t arg[];
};

struct ans {
    uint32_t id;
    uint32_t err;
};

static struct udp_pcb *pcb_cmd_s;
static struct udp_pcb *pcb_c;
static ip_addr_t ip_s;
static ip_addr_t ip_c = {.addr = IPADDR_ANY};

static void udp_server_send(void *data, uint32_t size, uint16_t port)
{
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
    pbuf_take(p, data, size);
    udp_sendto(pcb_c, p, &ip_c, port);
    pbuf_free(p);
}

static void udp_server_send_ans(struct ans *ans)
{
    udp_server_send(ans, sizeof(*ans), CLIENT_PORT_CMD);
}

static void cmd_work(struct cmd *cmd)
{
    uint32_t cmd_id = cmd->id;
    struct ans ans = {.id = cmd_id};
    switch (cmd_id) {
    case CMD_CONNECT:
        char *ip_client = ipaddr_ntoa(&ip_c);
        debug_printf("DEV: client %s connected\n", ip_client);
        break;
    case CMD_DISCONNECT:
        delay_ms(10);
        debug_printf("DEV: client disconnected\n");
        break;
    case CMD_START_VIBR:
        ads1278_start();
        break;
    case CMD_START_RTD:
        ads1220_start();
        break;
    case CMD_STOP:
        ads1278_stop();
        ads1220_stop();
        break;
    case CMD_RESET:
        ads1278_stop();
        ads1220_stop();
        debug_printf("DEV: reset...\n");
        delay_ms(10);
        udp_server_send_ans(&ans);
        delay_ms(10);
        NVIC_SystemReset();
        break;
    case CMD_CHANGE_IP:
        ads1278_stop();
        ads1220_stop();
        delay_ms(10);
        udp_server_send_ans(&ans);
        delay_ms(10);
        debug_printf("DEV: client disconnected\n");

        ip_s.addr = cmd->arg[0];
        char *ip_str = ipaddr_ntoa(&ip_s);
        debug_printf("DEV: ip change to %s\n", ip_str);

        settings_change_ipaddr(ip_s.addr);

        debug_printf("DEV: reset...\n");
        delay_ms(10);
        NVIC_SystemReset();

        break;
    case CMD_SETDIV_VIBR:
        ads1278_stop();
        delay_ms(10);
        ads1278_setdiv(cmd->arg[0]);
        delay_ms(10);
        break;
    case CMD_SETCH_VIBR:
        ads1278_stop();
        delay_ms(10);
        ads1278_setch(cmd->arg[0]);
        delay_ms(10);
        break;
    case CMD_ECHO:
        break;
    default:
        ans.err = ERR_CMD_UNKNOW;
        return;
    }
    udp_server_send_ans(&ans);
}

static void recv_callback(
    void *arg,
    struct udp_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *addr,
    u16_t port)
{
    (void)arg;
    (void)pcb;
    (void)port;

    ip_c.addr = addr->addr;
    if (p->len > 8) {
        udp_server_send_ans(&(struct ans){
            .id = *(uint32_t *)p->payload,
            .err = ERR_CMD_SO_BIG,
        });
    } else {
        cmd_work(p->payload);
    }

    pbuf_free(p);
}

void udp_server_init(void)
{
    pcb_cmd_s = udp_new();
    pcb_c = udp_new();

    ip_s.addr = (*(uint32_t *)settings->ip_addr);

    udp_bind(pcb_cmd_s, &ip_s, SERVER_PORT_CMD);
    udp_recv(pcb_cmd_s, recv_callback, NULL);

    char *ip_str = ipaddr_ntoa(&ip_s);
    debug_printf("DEV: server %s:%d started\n", ip_str, SERVER_PORT_CMD);
    debug_printf("DEV:  cmd port %d\n", CLIENT_PORT_CMD);
    debug_printf("DEV:  rtd port %d\n", CLIENT_PORT_RTD);
    debug_printf("DEV: vibr port %d\n", CLIENT_PORT_VIBR);
}

void udp_server_send_vibr(void *data, uint32_t size)
{
    udp_server_send(data, size, CLIENT_PORT_VIBR);
}

void udp_server_send_rtd(void *data, uint32_t size)
{
    udp_server_send(data, size, CLIENT_PORT_RTD);
}

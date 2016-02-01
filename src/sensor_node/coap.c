/**
 * written by smlng
 */
 // standard
  #include <inttypes.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
 // network
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
 // riot
 #include "board.h"
#include "periph/gpio.h"
 #include "thread.h"
 #include "coap.h"

 // compatibility
 #ifndef LED_ON
 #define LED_ON      LED_RED_ON
 #define LED_OFF     LED_RED_OFF
 #define LED_TOGGLE  LED_RED_TOGGLE
 #endif

 // parameters
 #define COAP_BUF_SIZE         (63)
 #define COAP_PORT             (5683)
 #define COAP_MSG_QUEUE_SIZE   (8U)
 #define COAP_REPSONSE_LENGTH   (1500)

static char coap_server_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t coap_server_msg_queue[COAP_MSG_QUEUE_SIZE];
static char led = '0';
static char endpoints_response[COAP_REPSONSE_LENGTH] = "";

void _build_endpoints_response(void);

void coap_setup_endpoints(void)
{
    _build_endpoints_response();
}

static const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};
static int handle_get_well_known_core(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)endpoints_response, strlen(endpoints_response), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

static const coap_endpoint_path_t path_humidity = {1, {"humidity"}};
static int handle_get_humidity(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)&led, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

static const coap_endpoint_path_t path_temperature = {1, {"temperature"}};
static int handle_get_temperature(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)&led, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

static const coap_endpoint_path_t path_led = {1, {"led"}};
static int handle_put_led(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    if (inpkt->payload.len == 0)
        return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
    if (inpkt->payload.p[0] == '1')
    {
        led = '1';
        LED_ON;
        puts("LED ON");
        return coap_make_response(scratch, outpkt, (const uint8_t *)&led, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    else
    {
        led = '0';
        LED_OFF;
        puts("LED OFF");
        return coap_make_response(scratch, outpkt, (const uint8_t *)&led, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
}

const coap_endpoint_t endpoints[] =
{
    {COAP_METHOD_GET, handle_get_well_known_core, &path_well_known_core, "ct=40"},
    {COAP_METHOD_GET, handle_get_humidity, &path_humidity, "ct=0"},
    {COAP_METHOD_GET, handle_get_temperature, &path_temperature, "ct=0"},
    {COAP_METHOD_PUT, handle_put_led, &path_led, NULL},
//    {COAP_RSPCODE_CONTENT, handle_response, &path_led, NULL},
//    {COAP_RSPCODE_CONTENT, handle_response, &path_well_known_core, NULL},
    {(coap_method_t)0, NULL, NULL, NULL}
};

void _build_endpoints_response(void)
{
    uint16_t len = COAP_REPSONSE_LENGTH;
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; // Null-terminated string

    while(NULL != ep->handler)
    {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(endpoints_response)) {
            strncat(endpoints_response, ",", len);
            len--;
        }

        strncat(endpoints_response, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(endpoints_response, "/", len);
            len--;

            strncat(endpoints_response, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(endpoints_response, ">;", len);
        len -= 2;

        strncat(endpoints_response, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }
}

/**
 * @brief udp receiver thread function
 *
 * @param[in] arg   unused
 */
void *coap_server(void *arg)
{
    (void) arg;
    // start coap listener
    struct sockaddr_in6 server_addr;
    char src_addr_str[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;
    static int sock = -1;
    static uint8_t buf[COAP_BUF_SIZE];
    uint8_t scratch_raw[COAP_BUF_SIZE];
    coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)};

    msg_init_queue(coap_server_msg_queue, COAP_MSG_QUEUE_SIZE);
    sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    /* parse port */
    port = (uint16_t)COAP_PORT;
    if (port == 0) {
        puts("Error: invalid port specified");
        return NULL;
    }
    server_addr.sin6_family = AF_INET6;
    memset(&server_addr.sin6_addr, 0, sizeof(server_addr.sin6_addr));
    server_addr.sin6_port = htons(port);
    if (sock < 0) {
        puts("error initializing socket");
        sock = 0;
        return NULL;
    }
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        sock = -1;
        puts("error binding socket");
        return NULL;
    }
    while (1) {
        int res,rc;
        struct sockaddr_in6 src;
        socklen_t src_len = sizeof(struct sockaddr_in6);
        coap_packet_t pkt;
        // blocking receive, waiting for data
        if ((res = recvfrom(sock, buf, sizeof(buf), 0,
                            (struct sockaddr *)&src, &src_len)) < 0) {
            puts("Error on receive");
        }
        else if (res == 0) {
            puts("Peer did shut down");
        }
        else { // check for PING or PONG
            if (0 != (rc = coap_parse(&pkt, buf, res)))
                printf("Bad packet rc=%d\n", rc);
            else
            {
                inet_ntop(AF_INET6, &(src.sin6_addr),
                          src_addr_str, sizeof(src_addr_str));
                printf(". received COAP message from [%s].\n", src_addr_str);
                size_t rsplen = sizeof(buf);
                coap_packet_t rsppkt;
                coap_handle_req(&scratch_buf, &pkt, &rsppkt);

                if (0 != (rc = coap_build(buf, &rsplen, &rsppkt))) {
                    printf("coap_build failed rc=%d\n", rc);
                }
                else {
                    sendto(sock, buf, rsplen, 0, (struct sockaddr *)&src, src_len);
                }
            }
        }
    }

    return NULL;
}

/**
 * @brief start udp receiver thread
 */
void coap_start_server(void)
{
    thread_create(coap_server_stack, sizeof(coap_server_stack),
                    THREAD_PRIORITY_MAIN, THREAD_CREATE_STACKTEST,
                    coap_server, NULL, "coap_server");
    puts(". started COAP server...");
}

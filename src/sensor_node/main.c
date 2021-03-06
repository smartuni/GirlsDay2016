/**
 * @ingroup     smartuni
 * @{
 *
 * @file
 * @brief       Main control loop
 *
 * @author      smlng <s@mlng.net>
 *
 * @}
 */

// standard
 #include <inttypes.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
// riot
#include "board.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#include "periph/gpio.h"
#include "shell.h"
// own
//#include "poem.h"
#include "sensor.h"

#define COMM_PAN           (0x2804) // lowpan ID
#define COMM_CHAN          (16U)  // channel

// compatibility
#ifndef LED_ON
#define LED_ON      LED0_ON
#define LED_OFF     LED0_OFF
#define LED_TOGGLE  LED0_TOGGLE
#endif

int sensor_pid = -1;
int coap_pid = -1;
int shell_poem_cnt = 0;

extern const char* poem[];
extern const size_t poem_len;

//extern int coap_cmd(int argc, char **argv);
extern int sensor_start_thread(void);
extern int coap_start_thread(void);

static int cmd_get(int argc, char **argv);
static int cmd_put(int argc, char **argv);

// array with available shell commands
static const shell_command_t shell_commands[] = {
    { "get", "get sensor", cmd_get },
    { "put", "set actor",  cmd_put },
    { NULL, NULL, NULL }
};

static int comm_init(void)
{
    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    uint16_t pan = COMM_PAN;
    uint16_t chan = COMM_CHAN;

    /* get the PID of the first radio */
    if (gnrc_netif_get(ifs) <= 0) {
        puts("ERROR: comm init, not radio found!\n");
        return (-1);
    }

    /* initialize the radio */
    gnrc_netapi_set(ifs[0], NETOPT_NID, 0, &pan, 2);
    gnrc_netapi_set(ifs[0], NETOPT_CHANNEL, 0, &chan, 2);
    return 0;
}

/**
 * @brief the main programm loop
 *
 * @return non zero on error
 */
int main(void)
{
    // some initial infos
    puts("Girls Day 2016 @ HAW Hamburg!");
    puts("================");
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);
    puts("================");

    // init 6lowpan interface
    puts(". init network");
    if (comm_init()!=0) {
        return 1;
    }
    puts(".");
    // start sensor loop
    puts(".. init sensors");
    sensor_pid = sensor_start_thread();
    if (sensor_pid < 0) {
        return 1;
    }
    LED0_OFF;
    LED1_OFF;
    LED2_OFF;
    puts(":");
    // start coap receiver
    puts("... init coap");
    coap_pid = coap_start_thread();
    puts(".");
    puts(":");
    // start shell
    puts(".... init shell");
    puts(":");
    puts(":");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    // should be never reached
    return 0;
}

int cmd_get(int argc, char **argv)
{
    if ((argc == 2) && (strncmp(argv[1],"temperature",4) == 0)) {
        int t100 = sensor_get_temperature();
        int rest = t100 % 100;
        printf("Temperature: %d.%02d C\n", t100/100, rest);
    }
    else if ((argc == 2) && (strncmp(argv[1],"humitity",3) == 0)) {
        int h100 = sensor_get_humidity();
        int rest = h100 % 100;
        printf("Humidity: %d.%02d %%\n", h100/100, rest);
    }
    else if ((argc == 2) && (strncmp(argv[1],"airquality",3) == 0)) {
        int a100 = sensor_get_airquality();
        int rest = a100 % 100;
        printf("AirQuality: %d.%02d %%\n", a100/100, rest);
    }
    else if ((argc >= 2) &&(strncmp(argv[1],"poem",4) == 0)) {
        int l = 0;
        if (argc == 2) {
            l = shell_poem_cnt % poem_len;
            ++shell_poem_cnt;
        }
        else if (argc == 3) {
            l = atoi(argv[2]) % poem_len;
        }
        else {
            puts("ERROR: unknown poem request!");
            return (1);
        }
        printf("%02d  %s\n", l, poem[l]);
    }
    else {
        puts ("[WARN] unknown sensor value requested.");
        return (1);
    }
    return 0;
}

int cmd_put(int argc, char **argv)
{
    if ((argc == 3) && (strcmp(argv[1],"led") == 0)) {
        if (strcmp(argv[2], "1") == 0) {
            LED0_ON;
            LED1_ON;
            LED2_ON;
        }
        else if (strcmp(argv[2], "r") == 0) {
            LED0_TOGGLE;
        }
        else if (strcmp(argv[2], "g") == 0) {
            LED1_TOGGLE;
        }
        else if (strcmp(argv[2], "b") == 0) {
            LED2_TOGGLE;
        }
        else {
            LED0_OFF;
            LED1_OFF;
            LED2_OFF;
        }
    }
    else {
        puts ("[WARN] unknown actor setting requested.");
        return (1);
    }
    return 0;
}

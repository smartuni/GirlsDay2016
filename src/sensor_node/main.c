/**
 * written by smlng
 */

// standard
 #include <inttypes.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
// riot
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"
// own
#include "sensor.h"

int sensor_pid = -1;
int coap_pid = -1;

//extern int coap_cmd(int argc, char **argv);
extern int sensor_start_thread(void);
extern int coap_start_thread(void);

static int cmd_get(int argc, char **argv);

// array with available shell commands
static const shell_command_t shell_commands[] = {
    { "get", "get sensor readings", cmd_get },
    { NULL, NULL, NULL }
};

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

    // start sensor loop
    puts(". init sensors");
    sensor_pid = sensor_start_thread();
    puts(".");
    // start coap receiver
    puts(".. init coap");
    coap_pid = coap_start_thread();
    puts(":");
    // start shell
    puts("... init shell");
    puts(".");
    puts(":");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    // should be never reached
    return 0;
}

int cmd_get(int argc, char **argv)
{
    if ((argc == 2) && (strcmp(argv[0],"get") == 0)) {
        if (strcmp(argv[1],"temperature") == 0) {
            int t100 = sensor_get_temperature();
            int rest = t100 % 100;
            printf("Temperature: %d.%02d C\n", t100/100, rest);
        }
        else if (strcmp(argv[1],"humidity") == 0) {
            int h100 = sensor_get_humidity();
            int rest = h100 % 100;
            printf("Humidity: %d.%02d %%\n", h100/100, rest);
        }
        else {
            puts ("[WARN] unknown sensor value requested.");
        }
    }
    return 0;
}

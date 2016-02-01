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

extern int coap_cmd(int argc, char **argv);
extern void coap_start_server(void);
extern void coap_setup_endpoints(void);

// array with available shell commands
static const shell_command_t shell_commands[] = {
//    { "coap", "send COAP request", coap_cmd },
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

    // init coap endpoints
    coap_setup_endpoints();
    // start coap receiver
    coap_start_server();

    // start shell
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    // should be never reached
    return 0;
}

#include <stdio.h>
#include "portpeek/cli.h"

#define PORTPEEK_VERSION "0.1.0"

void execute_command(Command command) {

    switch (command.type) {
        case COMMAND_HELP:
            printf("Portpeek - Process and Network Intelligence\n");
            break;

        case COMMAND_VERSION:
            printf("Portpeek v%s\n", PORTPEEK_VERSION);
            break;

        case COMMAND_PORTS:
            printf("Listing ports...\n");
            break;

        case COMMAND_INSPECT:
            printf("Inspecting PID: %s\n", command.argument);
            break;

        case COMMAND_PORT_LOOKUP:
            printf("Investigating port: %s\n", command.argument);
            break;

        case COMMAND_INVALID:
            fprintf(stderr, "Invalid command. Use --help.\n");
            break;
    }
}
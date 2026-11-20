#include <stdio.h>

#include <portpeek/cli.h>

#define PORTPEEK_VERSION "0.1.0"

static void print_help(void)
{
    puts("Portpeek - Process and Network Intelligence");
    puts("Usage: portpeek [--json] [--no-color] [--verbose] COMMAND");
    puts("       portpeek [--json] [--no-color] [--verbose] PORT");
    puts("Commands: ports, inspect PID [--connections], connections, tree PID");
}

int execute_command(const Command *command)
{
    switch (command->type) {
        case COMMAND_HELP:
            print_help();
            break;
        case COMMAND_VERSION:
            puts("Portpeek v" PORTPEEK_VERSION);
            break;
        case COMMAND_PORTS:
            puts("Listing ports...");
            break;
        case COMMAND_INSPECT:
            printf("Inspecting PID: %ld\n", command->numeric_argument);
            if (command->include_connections) {
                puts("Including process connections...");
            }
            break;
        case COMMAND_PORT_LOOKUP:
            printf("Investigating port: %ld\n", command->numeric_argument);
            break;
        case COMMAND_CONNECTIONS:
            puts("Listing connections...");
            break;
        case COMMAND_TREE:
            printf("Displaying process tree for PID: %ld\n", command->numeric_argument);
            break;
    }
    return PORTPEEK_EXIT_SUCCESS;
}

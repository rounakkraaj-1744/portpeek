#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include <portpeek/cli.h>

static int is_valid_number (const char *str, long min, long max){
    char *endptr;
    errno = 0;

    long value = strtol(str, &endptr, 10);

    if (errno == ERANGE || str == endptr || *endptr != '\0' || value < min || value > max)
        return 0;

    return 1;
}

Command parse_command(int argc, char *argv[]) {
    Command command = {
        .type = COMMAND_INVALID,
        .argument = NULL
    };

    if (argc == 1){
        command.type = COMMAND_HELP;
        return command;
    }

    if (argc == 2 && strcmp(argv[1], "--help") == 0){
        command.type = COMMAND_HELP;
        return command;
    }

    if (argc == 2 && strcmp(argv[1], "--version") == 0){
        command.type = COMMAND_VERSION;
        return command;
    }

    if (argc == 2 && strcmp(argv[1], "--inspect") == 0){
        command.type = COMMAND_INSPECT;
        return command;
    }

    if (argc == 2 && strcmp(argv[1], "--ports") == 0){
        command.type = COMMAND_PORTS;
        return command;
    }

    if (argc == 3 && strcmp(argv[1], "inspect") == 0){
        if (!is_valid_number(argv[2], 1, INT_MAX)) {
            return command;
        }

        command.type = COMMAND_INSPECT;
        command.argument = argv[2];

        return command;
    }

    if (argc == 2 && is_valid_number(argv[1], 1, 65535)) {
        command.type = COMMAND_PORT_LOOKUP;
        command.argument = argv[1];

        return command;
    }

    return command;
}
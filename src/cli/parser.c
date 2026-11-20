#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include <portpeek/cli.h>

static ParseResult parse_error(const char *message)
{
    ParseResult result = {0};
    result.error_message = message;
    return result;
}

static int parse_number(const char *text, long minimum, long maximum, long *value)
{
    char *end = NULL;
    long parsed;

    if (text == NULL || text[0] < '0' || text[0] > '9') {
        return 0;
    }

    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' ||
        parsed < minimum || parsed > maximum) {
        return 0;
    }

    *value = parsed;
    return 1;
}

ParseResult parse_command(int argc, char *argv[])
{
    Command command = {0};
    const char *positional[3];
    int positional_count = 0;
    int help_requested = 0;
    int version_requested = 0;

    for (int i = 1; i < argc; ++i) {
        const char *argument = argv[i];

        if (strcmp(argument, "--json") == 0) {
            command.json = 1;
        } else if (strcmp(argument, "--no-color") == 0) {
            command.no_color = 1;
        } else if (strcmp(argument, "--verbose") == 0) {
            command.verbose = 1;
        } else if (strcmp(argument, "--help") == 0) {
            help_requested = 1;
        } else if (strcmp(argument, "--version") == 0) {
            version_requested = 1;
        } else {
            if (positional_count >= 3) {
                return parse_error("too many arguments");
            }
            positional[positional_count++] = argument;
        }
    }

    if (help_requested || version_requested) {
        if (help_requested && version_requested) {
            return parse_error("--help and --version cannot be used together");
        }
        if (positional_count != 0) {
            return parse_error("--help and --version do not accept command arguments");
        }
        command.type = help_requested ? COMMAND_HELP : COMMAND_VERSION;
    } else if (positional_count == 0) {
        command.type = COMMAND_HELP;
    } else if (positional_count == 1 &&
               (strcmp(positional[0], "ports") == 0 ||
                strcmp(positional[0], "--ports") == 0)) {
        command.type = COMMAND_PORTS;
    } else if (positional_count == 1 && strcmp(positional[0], "connections") == 0) {
        command.type = COMMAND_CONNECTIONS;
    } else if (positional_count == 2 && strcmp(positional[0], "inspect") == 0) {
        if (!parse_number(positional[1], 1, INT_MAX, &command.numeric_argument)) {
            return parse_error("PID must be a positive decimal integer no greater than INT_MAX");
        }
        command.type = COMMAND_INSPECT;
        command.has_numeric_argument = 1;
    } else if (positional_count == 3 && strcmp(positional[0], "inspect") == 0 &&
               strcmp(positional[2], "--connections") == 0) {
        if (!parse_number(positional[1], 1, INT_MAX, &command.numeric_argument)) {
            return parse_error("PID must be a positive decimal integer no greater than INT_MAX");
        }
        command.type = COMMAND_INSPECT;
        command.has_numeric_argument = 1;
        command.include_connections = 1;
    } else if (positional_count == 2 && strcmp(positional[0], "tree") == 0) {
        if (!parse_number(positional[1], 1, INT_MAX, &command.numeric_argument)) {
            return parse_error("PID must be a positive decimal integer no greater than INT_MAX");
        }
        command.type = COMMAND_TREE;
        command.has_numeric_argument = 1;
    } else if (positional_count == 1) {
        if (positional[0][0] == '-') {
            return parse_error("unsupported option");
        }
        if (!parse_number(positional[0], 1, 65535, &command.numeric_argument)) {
            return parse_error("port must be a decimal integer between 1 and 65535");
        }
        command.type = COMMAND_PORT_LOOKUP;
        command.has_numeric_argument = 1;
    } else {
        return parse_error("invalid command or incorrect number of arguments");
    }

    ParseResult result = {.valid = 1, .command = command, .error_message = NULL};
    return result;
}

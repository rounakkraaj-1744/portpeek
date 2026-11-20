#ifndef PORTPEEK_CLI_H
#define PORTPEEK_CLI_H

typedef enum {
    PORTPEEK_EXIT_SUCCESS = 0,
    PORTPEEK_EXIT_ERROR = 1,
    PORTPEEK_EXIT_USAGE = 2,
    PORTPEEK_EXIT_NO_RESULT = 3,
    PORTPEEK_EXIT_PARTIAL = 4
} ExitCode;

typedef enum {
    COMMAND_HELP,
    COMMAND_VERSION,
    COMMAND_PORTS,
    COMMAND_INSPECT,
    COMMAND_PORT_LOOKUP,
    COMMAND_CONNECTIONS,
    COMMAND_TREE
} CommandType;

typedef struct {
    CommandType type;
    long numeric_argument;
    int has_numeric_argument;
    int include_connections;
    int json;
    int no_color;
    int verbose;
} Command;

typedef struct {
    int valid;
    Command command;
    const char *error_message;
} ParseResult;

ParseResult parse_command(int argc, char *argv[]);
int execute_command(const Command *command);

#endif

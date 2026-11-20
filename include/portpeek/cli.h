#ifndef PORTPEEK_CLI_H
#define PORTPEEK_CLI_H

typedef enum {
    COMMAND_HELP, //0
    COMMAND_VERSION, //1 
    COMMAND_PORTS, //2
    COMMAND_INSPECT, //3
    COMMAND_PORT_LOOKUP, //4
    COMMAND_INVALID //5
} CommandType;

typedef struct Command{
    CommandType type;
    const char *argument;
} Command;

Command parse_command (int argc, char *argv[]);

void execute_command(Command command);

#endif
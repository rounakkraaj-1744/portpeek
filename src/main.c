#include <stdio.h>

#include <portpeek/cli.h>

int main(int argc, char *argv[])
{
    ParseResult parsed = parse_command(argc, argv);

    if (!parsed.valid) {
        fprintf(stderr, "portpeek: %s\nTry 'portpeek --help' for usage.\n",
                parsed.error_message);
        return PORTPEEK_EXIT_USAGE;
    }

    return execute_command(&parsed.command);
}

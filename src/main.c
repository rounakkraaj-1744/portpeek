#include <portpeek/cli.h>

int main (int argc, char *argv[]){
    Command command = parse_command(argc, argv);
    execute_command (command);

    return 0;
}
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include <portpeek/cli.h>

static int failures;

static void print_arguments(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i) {
        fprintf(stderr, " %s", argv[i]);
    }
}

static void expect_valid(int argc, char *argv[], CommandType expected_type,
                         long expected_value)
{
    ParseResult result = parse_command(argc, argv);
    if (!result.valid || result.command.type != expected_type ||
        (expected_value != 0 && result.command.numeric_argument != expected_value)) {
        fprintf(stderr, "FAIL: unexpected parse result for '%s'", argv[0]);
        print_arguments(argc, argv);
        fputc('\n', stderr);
        ++failures;
    }
}

static void expect_invalid(int argc, char *argv[])
{
    ParseResult result = parse_command(argc, argv);
    if (result.valid || result.error_message == NULL) {
        fprintf(stderr, "FAIL: expected parse error for '%s'", argv[0]);
        print_arguments(argc, argv);
        fputc('\n', stderr);
        ++failures;
    }
}

static void test_valid_commands(void)
{
    char *help[] = {"portpeek"};
    char *version[] = {"portpeek", "--version"};
    char *ports[] = {"portpeek", "ports"};
    char *legacy_ports[] = {"portpeek", "--ports"};
    char *inspect[] = {"portpeek", "inspect", "4821"};
    char *inspect_connections[] = {"portpeek", "inspect", "4821", "--connections"};
    char *port[] = {"portpeek", "65535"};
    char *connections[] = {"portpeek", "connections"};
    char *tree[] = {"portpeek", "tree", "1"};

    expect_valid(1, help, COMMAND_HELP, 0);
    expect_valid(2, version, COMMAND_VERSION, 0);
    expect_valid(2, ports, COMMAND_PORTS, 0);
    expect_valid(2, legacy_ports, COMMAND_PORTS, 0);
    expect_valid(3, inspect, COMMAND_INSPECT, 4821);
    expect_valid(4, inspect_connections, COMMAND_INSPECT, 4821);
    expect_valid(2, port, COMMAND_PORT_LOOKUP, 65535);
    expect_valid(2, connections, COMMAND_CONNECTIONS, 0);
    expect_valid(3, tree, COMMAND_TREE, 1);
}

static void test_invalid_commands_and_arity(void)
{
    char *unknown[] = {"portpeek", "unknown"};
    char *missing_pid[] = {"portpeek", "inspect"};
    char *extra_ports_arg[] = {"portpeek", "ports", "extra"};
    char *bad_suboption[] = {"portpeek", "inspect", "12", "--other"};
    char *unsupported_option[] = {"portpeek", "--color"};
    char *help_extra[] = {"portpeek", "--help", "ports"};
    char *help_version[] = {"portpeek", "--help", "--version"};

    expect_invalid(2, unknown);
    expect_invalid(2, missing_pid);
    expect_invalid(3, extra_ports_arg);
    expect_invalid(4, bad_suboption);
    expect_invalid(2, unsupported_option);
    expect_invalid(3, help_extra);
    expect_invalid(3, help_version);
}

static void test_numeric_boundaries(void)
{
    char *port_zero[] = {"portpeek", "0"};
    char *port_over[] = {"portpeek", "65536"};
    char *port_junk[] = {"portpeek", "80x"};
    char *port_sign[] = {"portpeek", "+80"};
    char *port_overflow[] = {"portpeek", "999999999999999999999999999999999"};
    char *pid_zero[] = {"portpeek", "inspect", "0"};
    char *pid_negative[] = {"portpeek", "inspect", "-2"};
    char *pid_over[] = {"portpeek", "inspect", "2147483648"};
    char pid_max_text[32];
    char *pid_max[] = {"portpeek", "inspect", pid_max_text};
    char *pid_junk[] = {"portpeek", "inspect", "4abc"};
    char *pid_overflow[] = {"portpeek", "inspect", "999999999999999999999999999999999"};

    snprintf(pid_max_text, sizeof(pid_max_text), "%d", INT_MAX);
    expect_invalid(2, port_zero);
    expect_invalid(2, port_over);
    expect_invalid(2, port_junk);
    expect_invalid(2, port_sign);
    expect_invalid(2, port_overflow);
    expect_invalid(3, pid_zero);
    expect_invalid(3, pid_negative);
    expect_invalid(3, pid_over);
    expect_valid(3, pid_max, COMMAND_INSPECT, INT_MAX);
    expect_invalid(3, pid_junk);
    expect_invalid(3, pid_overflow);
}

static void test_global_options(void)
{
    char *before[] = {"portpeek", "--json", "ports", "--no-color", "--verbose"};
    char *after[] = {"portpeek", "3000", "--json", "--no-color", "--verbose"};
    ParseResult first = parse_command(5, before);
    ParseResult second = parse_command(5, after);

    if (!first.valid || !first.command.json || !first.command.no_color ||
        !first.command.verbose || first.command.type != COMMAND_PORTS) {
        fprintf(stderr, "FAIL: global options before/after command were not captured\n");
        ++failures;
    }
    if (!second.valid || !second.command.json || !second.command.no_color ||
        !second.command.verbose || second.command.type != COMMAND_PORT_LOOKUP ||
        second.command.numeric_argument != 3000) {
        fprintf(stderr, "FAIL: global options on port lookup were not captured\n");
        ++failures;
    }
}

int main(void)
{
    test_valid_commands();
    test_invalid_commands_and_arity();
    test_numeric_boundaries();
    test_global_options();

    if (failures != 0) {
        fprintf(stderr, "%d CLI test(s) failed\n", failures);
        return 1;
    }

    puts("All CLI parser tests passed");
    return 0;
}

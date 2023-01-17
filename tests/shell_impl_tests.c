#include <unistd.h>
#include "tests.h"
#include "util.h"
#include "shell_impl.h"
#include "state.h"
#include "shell.h"
#include <stdbool.h>

static void test_init_state(const char *expected_prompt, FILE *in, FILE *out, FILE *err);
static void test_destroy_state(const bool *expected_fatal);
static void test_reset_state(const char *expected_prompt, bool expected_fatal);
static void test_read_commands(const char *command, const char *expected_command, int expected_return);
Describe(shell_impl);


static struct dc_env *environ;
static struct dc_error *error;

BeforeEach(shell_impl)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(shell_impl)
{
    dc_error_reset(error);
}

Ensure(shell_impl, init_state)
{
    if(dc_unsetenv(environ, error, "PS1") != 0)
    {
        dc_error_set_reporting(error, "unsetenv failed");
        exit(EXIT_FAILURE);
    }
    test_init_state("$ ", stdin, stdout, stderr);
    if(dc_setenv(environ, error, "PS1", "test", true) != 0)
    {
        dc_error_set_reporting(error, "setenv failed");
        exit(EXIT_FAILURE);
    }
    test_init_state("test", stdin, stdout, stderr);
}

static void test_init_state(const char *expected_prompt, FILE *in, FILE *out, FILE *err)
{
    struct state state;
    int next_state;
    long line_length;

    state.sin = in;
    state.sout = out;
    state.serr = err;
    line_length = sysconf(_SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
//    printf("Getenv \"PS1\" %s\n", getenv("PS1"));
    next_state = init_state(environ, error, &state);
    assert_false(dc_error_has_error(error));
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    assert_that(state.sin, is_equal_to(in));
    assert_that(state.sout, is_equal_to(out));
    assert_that(state.serr, is_equal_to(err));
    assert_that(state.in_redirect_regex, is_not_null);
    assert_that(state.out_redirect_regex, is_not_null);
    assert_that(state.err_redirect_regex, is_not_null);
    assert_that(state.prompt, is_equal_to_string(expected_prompt));
    assert_that(state.path, is_not_null);
    assert_that(state.max_line_length, is_equal_to(line_length));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
    assert_that(state.fatal_error, is_false);
}

Ensure(shell_impl, destroy_state)
{
    test_destroy_state((bool *) true);
    test_destroy_state(false);

}

static void test_destroy_state(const bool *expected_fatal)
{
    struct state state;
    int next_state;
    long line_length;

    state.sin = stdin;
    state.sout = stdout;
    state.serr = stderr;
    init_state(environ, error, &state);
    state.fatal_error = expected_fatal;
    next_state = destroy_state(environ, error, &state);
    assert_that(next_state, is_equal_to(DC_FSM_EXIT));
    assert_that(state.sin, is_equal_to(stdin));
    assert_that(state.sout, is_equal_to(stdout));
    assert_that(state.serr, is_equal_to(stderr));
    assert_false(dc_error_has_error(error));
    assert_false(state.fatal_error);
    assert_that(state.in_redirect_regex, is_null);
    assert_that(state.out_redirect_regex, is_null);
    assert_that(state.err_redirect_regex, is_null);
    assert_that(state.prompt, is_null);
    assert_that(state.path, is_null);
    assert_that(state.max_line_length, is_equal_to(0));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
}


Ensure(shell_impl, reset_state)
{
    dc_unsetenv(environ, error, "PS1");
    test_reset_state("$ ", false);

    dc_setenv(environ, error, "PS1", "test", true);
    test_reset_state("test", false);

    dc_unsetenv(environ, error, "PS1");
    test_reset_state("$ ", true);

    dc_setenv(environ, error, "PS1", "!>", true);
    test_reset_state("!>", true);
}
static void test_reset_state(const char* expected_prompt,const bool expected_fatal)
{
    struct state state;
    int next_state;
    long line_length;

    state.sin = stdin;
    state.sout = stdout;
    state.serr = stderr;
    line_length = sysconf(_SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
    init_state(environ, error, &state);
    state.fatal_error = expected_fatal;
    next_state = reset_state(environ, error, &state);
    assert_that(state.sin, is_equal_to(stdin));
    assert_that(state.sout, is_equal_to(stdout));
    assert_that(state.serr, is_equal_to(stderr));
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    assert_false(dc_error_has_error(error));
    assert_false(state.fatal_error);
    assert_that(state.in_redirect_regex, is_not_null);
    assert_that(state.out_redirect_regex, is_not_null);
    assert_that(state.err_redirect_regex, is_not_null);
    assert_that(state.prompt, is_equal_to_string(expected_prompt));
    assert_that(state.path, is_not_null);
    assert_that(state.max_line_length, is_equal_to(line_length));
    assert_that(state.current_line, is_null);
    assert_that(state.current_line_length, is_equal_to(0));
    assert_that(state.command, is_null);
}

Ensure(shell_impl, read_commands)
{
    test_read_commands("hello", "hello", SEPARATE_COMMANDS);
    test_read_commands("hello\n", "hello", SEPARATE_COMMANDS);
    test_read_commands("\n", "", RESET_STATE);
}
static void test_read_commands(const char *command, const char *expected_command, int expected_return)
{

    char *in_buf;
    char out_buf[1024];
    FILE *in;
    FILE *out;
    struct state state;
    int next_state;
    char *cwd;
    char *prompt;

    in_buf = strdup(command);
    in = fmemopen(in_buf, strlen(in_buf) + 1, "r");
    out = fmemopen(out_buf, sizeof(out_buf ), "w");
    state.sin = in;
    state.sout = out;
    state.serr = stderr;
    dc_unsetenv(environ, error, "PS1");
    next_state = init_state(environ, error, &state);
    assert_false(dc_error_has_error(error));
    assert_false(state.fatal_error);
    assert_that(next_state, is_equal_to(READ_COMMANDS));
    next_state = read_commands(environ, error, &state);
    assert_that(next_state, is_equal_to(expected_return));
    assert_false(dc_error_has_error(error));
    cwd = dc_get_working_dir(environ, error);
    // [current working directory]$ state.prompt
    prompt = malloc(1 + strlen(cwd) + 1 + 2 + strlen(state.prompt) + 1);
    sprintf(prompt, "[%s] %s", cwd, state.prompt);
    assert_that(out_buf, is_equal_to_string(prompt));
    free(cwd);
    free(prompt);
    assert_that(state.current_line, is_equal_to_string(expected_command));
    assert_that(state.current_line_length, is_equal_to(strlen(expected_command)));

}

Ensure(shell_impl, parse_commands)
{

}

Ensure(shell_impl, execute_commands)
{

}

Ensure(shell_impl, do_exit)
{

}

Ensure(shell_impl, handle_error)
{

}

TestSuite *shell_impl_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, shell_impl, init_state);
    add_test_with_context(suite, shell_impl, destroy_state);
    add_test_with_context(suite, shell_impl, reset_state);
    add_test_with_context(suite, shell_impl, read_commands);
    add_test_with_context(suite, shell_impl, parse_commands);
    add_test_with_context(suite, shell_impl, execute_commands);
    add_test_with_context(suite, shell_impl, do_exit);
    add_test_with_context(suite, shell_impl, handle_error);

    return suite;
}

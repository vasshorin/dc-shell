#include <unistd.h>
#include "tests.h"
#include "util.h"
#include "shell_impl.h"
#include "state.h"
#include "shell.h"

static void test_init_state(const char *expected_prompt);
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
    test_init_state("$ ");
    if(dc_setenv(environ, error, "PS1", "test", true) != 0)
    {
        dc_error_set_reporting(error, "setenv failed");
        exit(EXIT_FAILURE);
    }
    test_init_state("test");
}

static void test_init_state(const char *expected_prompt)
{
    struct state state;
    int next_state;
    long line_length;

    line_length = sysconf(_SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
//    printf("Getenv \"PS1\" %s\n", getenv("PS1"));
    next_state = init_state(environ, error, &state);
    assert_false(dc_error_has_error(error));
    assert_that(next_state, is_equal_to(READ_COMMANDS));
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
    struct state state;
    int next_state;
    long line_length;

    line_length = sysconf(_SC_ARG_MAX);
    assert_that_expression(line_length >= 0);
    init_state(environ, error, &state);
    next_state = destroy_state(environ, error, &state);
    assert_that(next_state, is_equal_to(DC_FSM_EXIT));
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

}

Ensure(shell_impl, read_commands)
{

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
//    add_test_with_context(suite, shell_impl, destroy_state);
    add_test_with_context(suite, shell_impl, reset_state);
    add_test_with_context(suite, shell_impl, read_commands);
    add_test_with_context(suite, shell_impl, parse_commands);
    add_test_with_context(suite, shell_impl, execute_commands);
    add_test_with_context(suite, shell_impl, do_exit);
    add_test_with_context(suite, shell_impl, handle_error);

    return suite;
}

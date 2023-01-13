#include "tests.h"
#include "util.h"


static void check_state_reset(const struct dc_env *env, const struct state *state);

Describe(util);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(util)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(util)
{
    dc_error_reset(error);
}

Ensure(util, get_prompt)
{
    char *prompt;

    prompt = dc_getenv(environ, "PS1");

    if(prompt != NULL)
    {
        dc_setenv(environ, error, "PS1", NULL, true);
    }

    prompt = get_prompt(environ, error);
    assert_that(prompt, is_equal_to_string("$ "));

    dc_setenv(environ, error, "PS1", "test", true);
    prompt = get_prompt(environ, error);
    assert_that(prompt, is_equal_to_string("test"));
}

Ensure(util, get_path)
{
    static const char *paths[] = {
            ".",
            "abc",
            "def",
            "abc:def",
            "usr/bin",
            ".:/usr/bin",
            "/bin:/usr/bin",
            "/bin:/usr/bin:/usr/local/bin",
            "/bin:/usr/bin:/usr/local/bin:/usr/local/sbin",
            NULL

    };
    char *path;

//    dc_setenv(environ, error, "PATH", NULL, true);
//    path = get_path(environ, error);
//    assert_that(path, is_null);

    for (int i = 0; paths[i]; i++)
    {

        dc_setenv(environ, error, "PATH", paths[i], true);
        path = get_path(environ, error);
        assert_that(path, is_equal_to_string(paths[i]));
    }
}

Ensure(util, parse_path)
{

}



Ensure(util, reset_state)
{
    struct state state;
    char *str;

    state.in_redirect_regex = NULL;
    state.out_redirect_regex = NULL;
    state.err_redirect_regex = NULL;
    state.path = NULL;
    state.prompt = NULL;
    state.max_line_length = 0;
    state.current_line = NULL;
    state.current_line_length = 0;
    state.command = NULL;
    state.fatal_error = false;

    do_reset_state(environ, error, &state);
    check_state_reset(environ, &state);

    state.current_line = strdup("");
    state.current_line_length = strlen(state.current_line);
    do_reset_state(environ, error, &state);
    check_state_reset(environ, &state);

    state.current_line = strdup("ls");
    state.current_line_length = strlen(state.current_line);
    do_reset_state(environ, error, &state);
    check_state_reset(environ, &state);

    state.current_line = strdup("ls");
    state.current_line_length = strlen(state.current_line);
    state.command = calloc(1, sizeof(struct command));
    do_reset_state(environ, error, &state);
    check_state_reset(environ, &state);

    do_reset_state(environ, error, &state);
    check_state_reset(environ, &state);
}

static void check_state_reset(const struct dc_env *env, const struct state *state)
{
    assert_that(state->current_line, is_null);
    assert_that(state->current_line_length, is_equal_to(0));
    assert_that(state->command, is_null);
}

Ensure(util, display_state)
{


}

Ensure(util, state_to_string)
{
    struct state state;
    char *str;

    state.in_redirect_regex = NULL;
    state.out_redirect_regex = NULL;
    state.err_redirect_regex = NULL;
    state.path = NULL;
    state.prompt = NULL;
    state.max_line_length = 0;
    state.current_line = NULL;
    state.current_line_length = 0;
    state.command = NULL;
    state.fatal_error = false;

    state.fatal_error = false;
    str = state_to_string(environ, &state);
    assert_that(str, is_equal_to_string("current_line = NULL, fatal_error = 0"));
    free(str);

    state.current_line = "";
    state.fatal_error = false;
    str = state_to_string(environ, &state);
    assert_that(str, is_equal_to_string("current_line = \"\", fatal_error = 0"));
    free(str);

    state.current_line = "hello";
    state.fatal_error = false;
    str = state_to_string(environ, &state);
    assert_that(str, is_equal_to_string("current_line = \"hello\", fatal_error = 0"));
    free(str);

    state.current_line = "world";
    state.fatal_error = true;
    str = state_to_string(environ, &state);
    assert_that(str, is_equal_to_string("current_line = \"world\", fatal_error = 1"));
    free(str);
}


TestSuite *util_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, util, get_prompt);
    add_test_with_context(suite, util, get_path);
    add_test_with_context(suite, util, parse_path);
    add_test_with_context(suite, util, reset_state);
    add_test_with_context(suite, util, display_state);
    add_test_with_context(suite, util, state_to_string);

    return suite;
}

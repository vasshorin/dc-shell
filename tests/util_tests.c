#include "tests.h"
#include "util.h"

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
            "/bin",
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

    str = state_to_string(environ, &state);
    assert_that(str, is_equal_to_string("current_line = NULL, fatal_error = 0"));
    free(str);

    state.current_line = "";
    str = state_to_string(environ, &state);
    assert_that(str, is_equal_to_string("current_line = "", fatal_error = 0"));
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

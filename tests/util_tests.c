#include "command.h"
#include "state.h"
#include "tests.h"
#include "util.h"
#include <dc_util/strings.h>
#include <dc_env/env.h>
#include <dc_c/dc_stdlib.h>




Describe(util);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(util)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach()
{
//    dc_error_reset(error);
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
}

TestSuite *util_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, util, get_prompt);

    return suite;
}

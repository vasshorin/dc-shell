//
// Created by Vasily Shorin on 2023-01-20.
//
#include <dc_error/error.h>
#include <dc_env/env.h>
#include "tests.h"
#include "shell.h"

static void test_run_shell(const char *in, const char *out, const char *err);

Describe(shell);
static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(shell)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(shell)
{
    dc_error_reset(error);

}

Ensure(shell, run_shell)
{
//    test_run_shell("exit\n", "[/Users/vasilyshorin/Desktop/COMP4981/dc-shell/cmake-build-debug] $ ", "");
    test_run_shell("cd /\nexit\n", "[/Users/vasilyshorin/Desktop/COMP4981/dc-shell/cmake-build-debug] $ ", "");
}

static void test_run_shell(const char *in, const char *expected_out, const char *expected_err)
{
    char out_buf[1024];
    char err_buf[1024];
    FILE *in_file;
    FILE *out_file;
    FILE *err_file;
    int exit_code;

    memset(out_buf, 0, sizeof out_buf);
    memset(err_buf, 0, sizeof err_buf);
    in_file = fmemopen(in, strlen(in), "r");
    out_file = fmemopen(out_buf, sizeof(out_buf), "w");
    err_file = fmemopen(err_buf, sizeof(err_buf), "w");

    exit_code = run_shell(environ, error, in_file, out_file, err_file);
    assert_that(exit_code, is_equal_to(0));
    assert_that(out_buf, is_equal_to_string(expected_out));
    fclose(in_file);
    fclose(out_file);
    fclose(err_file);

}

TestSuite *shell_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, shell, run_shell);

    return suite;
}
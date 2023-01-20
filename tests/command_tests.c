#include <dc_posix/dc_stdlib.h>
#include <dc_c/dc_string.h>
#include <dc_util/path.h>
#include <dc_util/strings.h>
#include "tests.h"
#include "command.h"
#include "shell_impl.h"


static void expand_path(const char *expected_file, char **expanded_file);

static void test_parse_command(const char *expected_line,
                               const char *expected_command,
                               size_t expected_argc,
                               char **expected_argv,
                               const char *expected_stdin_file,
                               const char *expected_stdout_file,
                               bool expected_stdout_overwrite,
                               const char *expected_stderr_file,
                               bool expected_stderr_overwrite);

Describe(command);

static struct dc_env *environ;
static struct dc_error *error;

BeforeEach(command)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(command)
{
    dc_error_reset(error);
}

Ensure(command,parse_command)
{
    char **argv;
    argv = dc_strs_to_array(environ, error, 1, NULL);
    test_parse_command("hello",
                       "hello",
                       1,
                       argv,
                       NULL,
                       NULL,
                       false,
                       NULL,
                       false);
//    argv = dc_strs_to_array(environ, error, 1, NULL);
//    test_parse_command("hello > ~/out.txt",
//                       "hello",
//                       1, argv,
//                       NULL,
//                       "~/out.txt",
//                       true,
//                       NULL,
//                       false);
//    argv = dc_strs_to_array(environ, error, 1, NULL);
//    test_parse_command("hello 2>> ~/err.txt",
//                       "hello",
//                       1,
//                       argv,
//                       NULL,
//                       NULL,
//                       false,
//                       "~/err.txt",
//                       true);
    argv = dc_strs_to_array(environ, error, 1, NULL);
    test_parse_command("/usr/bin/ls/ > out.txt",
                       "/usr/bin/ls/",
                       1,
                       argv,
                       NULL,
                       "out.txt",
                       true,
                       NULL,
                       false);
    argv = dc_strs_to_array(environ, error, 1, NULL);
//    test_parse_command("./a.out 2>> ~/err.txt",
//                          "./a.out",
//                          1,
//                          argv,
//                          NULL,
//                          NULL,
//                          false,
//                          "~/err.txt",
//                          false);
//    argv = dc_strs_to_array(environ, error, 4, NULL, "b", "c", NULL);
//    test_parse_command("a b c",
//                       "a",
//                       3,
//                       argv,
//                       NULL,
//                       NULL,
//                       false,
//                       NULL,
//                       false);
//    argv = dc_strs_to_array(environ, error, 1, NULL);
    argv = dc_strs_to_array(environ, error, 1, NULL);
    test_parse_command("./a.out << ~/in.txt",
                       "./a.out",
                       1,
                       argv,
                       "~/in.txt",
                       NULL,
                       false,
                       NULL,
                       false);
    argv = dc_strs_to_array(environ, error, 1, NULL);
    test_parse_command("./a.out > out.txt",
                       "./a.out",
                       1,
                       argv,
                       NULL,
                       "out.txt",
                       true,
                       NULL,
                       false);
    argv = dc_strs_to_array(environ, error, 1, NULL);
    test_parse_command("./a.out < ~/abc/in.txt > ~/abc/out.txt 2>> ~/abc/err.txt",
                       "./a.out",
                       1,
                       argv,
                       "~/abc/in.txt",
                       "~/abc/out.txt",
                       true,
                       "~/abc/err.txt",
                       false);
//    argv = dc_strs_to_array(environ, error, 3, NULL, NULL, "-l");
//    test_parse_command("ls -l >  ~/out.txt",
//                       "ls",
//                          2,
//                       argv,
//                       NULL,
//                       "~/out.txt",
//                       true,
//                       NULL,
//                       false);
    argv = dc_strs_to_array(environ, error, 1, NULL);
    test_parse_command("./a.out < in.txt > ~/out.txt 2>>      ~/err.txt",
                       "./a.out",
                          1,
                       argv,
                       "in.txt",
                       "~/out.txt",
                       true,
                       "~/err.txt",
                       false);
}


static void test_parse_command(const char *expected_line,
                               const char *expected_command,
                               size_t expected_argc,
                               char **expected_argv,
                               const char *expected_stdin_file,
                               const char *expected_stdout_file,
                               bool expected_stdout_overwrite,
                               const char *expected_stderr_file,
                               bool expected_stderr_overwrite)
{

    struct state state;
    char *expanded_stdin_file;
    char *expanded_stdout_file;
    char *expanded_stderr_file;

    expand_path(expected_stdin_file, &expanded_stdin_file);
    expand_path(expected_stdout_file, &expanded_stdout_file);
    expand_path(expected_stderr_file, &expanded_stderr_file);

    state.sin = NULL;
    state.sout = NULL;
    state.serr = NULL;

    init_state(environ, error, &state);
    state.command = calloc(1, sizeof(struct command));
    state.command->line = strdup(expected_line);

    parse_command(environ, error, &state, state.command);
    assert_that(state.command->line, is_equal_to_string(expected_line));
    assert_that(state.command->command, is_equal_to_string(expected_command));
    assert_that(state.command->argc, is_equal_to(expected_argc));
    assert_that(state.command->argv[0], is_null);
    assert_that(state.command->argv[expected_argc], is_null);
    for(size_t i = 1; i < expected_argc; i++)
    {
        assert_that(state.command->argv[i], is_equal_to_string(expected_argv[i]));
    }
//
    assert_that(state.command->stdin_file, is_equal_to_string(expanded_stdin_file));
    assert_that(state.command->stdout_file, is_equal_to_string(expanded_stdout_file));
    assert_that(state.command->stdout_overwrite, is_equal_to(expected_stdout_overwrite));
    assert_that(state.command->stderr_file, is_equal_to_string(expanded_stderr_file));
    assert_that(state.command->stderr_overwrite, is_equal_to(expected_stderr_overwrite));
    free(expanded_stdin_file);
    free(expanded_stdout_file);
    free(expanded_stderr_file);
    destroy_state(environ, error ,&state);
}

static void expand_path(const char *expected_file, char **expanded_file)
{
    if(expected_file == NULL)
    {
        *expanded_file = NULL;
    }
    else
    {
        dc_expand_path(environ, error, expanded_file, expected_file);

        if(dc_error_has_error(error))
        {
            fail_test("dc_expand_path failed");
        }
    }
}

TestSuite *command_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, command, parse_command);

    return suite;
}

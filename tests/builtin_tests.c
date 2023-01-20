#include "tests.h"
#include "builtins.h"
#include "util.h"
#include "dc_util/strings.h"
#include <dc_util/filesystem.h>
#include <dc_util/path.h>
#include <unistd.h>

static struct dc_env *environ;
static struct dc_error *error;
static void test_buildin_cd(const char *line, const char *cmd, size_t argc, char **argv, char *expected_dir, char *expected_error);

Describe(builtin);

BeforeEach(builtin)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(builtin)
{
    dc_error_reset(error);
}

Ensure(builtin, builtin_cd)
{
    char **argv;
    char *path;
    argv = dc_strs_to_array(environ, error, 3, NULL, "/", NULL);
    test_buildin_cd("cd /\n", "cd", 2, argv, "/", NULL);


    dc_expand_path(environ, error, &path, "~");
    argv = dc_strs_to_array(environ, error, 3, NULL, path, NULL);
    test_buildin_cd("cd ~\n", "cd", 2, argv, path, NULL);

    dc_expand_path(environ, error, &path, "~/");
    // remove the trailing slash
    path[strlen(path) - 1] = '\0';
    argv = dc_strs_to_array(environ, error, 3, NULL, path, NULL);
    test_buildin_cd("cd ~\n", "cd", 2, argv, path, NULL);

    chdir("/");
    dc_expand_path(environ, error, &path, "~");
    argv = dc_strs_to_array(environ, error, 2, NULL, NULL);
    test_buildin_cd("cd\n", "cd", 1, argv, path, NULL);

    chdir("/private/tmp");
    argv = dc_strs_to_array(environ, error, 3, NULL, "/dev/null", NULL);
    test_buildin_cd("cd /dev/null\n", "cd", 2, argv, "/private/tmp",  "     asdsad");

    char template[] = "/private/tmp/dc_test_XXXXXX";
    int fd;

    fd = mkdtemp(template);
    argv = dc_strs_to_array(environ, error, 3, NULL, template, NULL);
    test_buildin_cd("cd /dev/null\n", "cd", 2, argv, template,  NULL);
}

static void test_buildin_cd(const char *line, const char *cmd, size_t argc, char **argv, char *expected_dir, char *expected_error)
{
    struct command command;
    char *working_dir;
    char *message[1024];
    FILE *stderr_file;

    memset(&command, 0, sizeof(struct command));
    command.line = strdup(line);
    command.command = strdup(cmd);
    command.argc = argc;
    command.argv = argv;
    memset(message, 0, sizeof(message));
    stderr_file = fmemopen(message, sizeof(message), "w");

    builtin_cd(environ, error, &command, stderr_file);

    if(dc_error_has_error(error))
    {
        assert_that(message, is_equal_to_string(dc_error_get_message(error)));
    }
    working_dir = dc_get_working_dir(environ, error);

    assert_that(working_dir, is_equal_to_string(expected_dir));
    fclose(stderr_file);

}

TestSuite *builtin_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, builtin, builtin_cd);

    return suite;
}

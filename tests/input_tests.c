#include <dc_c/dc_string.h>
#include <dc_c/dc_stdlib.h>
#include "tests.h"
#include "input.h"

static void do_test_read_command_line(FILE *stream, size_t *n, const char *expected);
static void test_read_command_line(const char *data, ...);

Describe(input);
static struct dc_env *environ;
static struct dc_error *error;
BeforeEach(input)
{
    environ = dc_env_create(error, false, NULL);
    error = dc_error_create(false);
}

AfterEach(input)
{
    dc_error_reset(error);
}

Ensure(input, read_command_line)



{
    test_read_command_line("hello\n", "hello");
    test_read_command_line("hello\nworld", "hello");
    test_read_command_line("hello\nworld\n", "hello", "world", NULL);

}

static void test_read_command_line(const char *data, ...)
{
    FILE *strsteam;
    va_list strings;
    size_t buf_size;
    char *str;
    const char *expected_line;

    buf_size = strlen(data) + 1;
    str = strdup(data);
    strsteam = fmemopen(str, buf_size, "r");
    dc_strcpy(environ, str, data);

    //va loop
    va_start(strings, data);

    do
    {
        char *line;
        size_t line_size;

        line_size = buf_size;
        line = read_command_line(environ, error, strsteam, &line_size);
        expected_line = va_arg(strings, const char *);
        if(expected_line == NULL)
        {
            assert_that(line, is_equal_to_string(""));
            assert_that(line_size, is_equal_to(0));

        }
        else
        {
            assert_that(line, is_equal_to_string(expected_line));
            assert_that(line_size, is_equal_to(sizeof(line)));
        }
        dc_free(environ, line);
    } while(expected_line != NULL);

    va_end(strings);

    fclose(strsteam);
    free(str);
}


TestSuite *input_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, input, read_command_line);

    return suite;
}

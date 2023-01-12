#include "tests.h"
#include "input.h"

Describe(input);

BeforeEach(input)
{

}

AfterEach(input)
{

}

Ensure(input, read_command_line)
{

}

TestSuite *input_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, input, read_command_line);

    return suite;
}

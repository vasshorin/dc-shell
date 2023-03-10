#include "tests.h"
#include "execute.h"

Describe(execute);

BeforeEach(execute)
{

}

AfterEach(execute)
{

}

Ensure(execute, execute)
{
    TestSuite *suite;

    suite = create_test_suite();
    add_test_with_context(suite, execute, execute);

}

TestSuite *execute_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, execute, execute);

    return suite;
}

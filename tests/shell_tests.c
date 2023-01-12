#include "tests.h"
#include "shell.h"

Describe(shell);

BeforeEach(shell)
{

}

AfterEach(shell)
{

}

Ensure(shell, run_shell)
{

}

TestSuite *shell_tests(void)
{
    TestSuite  *suite;

    suite = create_test_suite();
    add_test_with_context(suite, shell, run_shell);

    return suite;
}

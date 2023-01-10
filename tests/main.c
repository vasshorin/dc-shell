#include "tests.h"

int main(int argc, char *argv[])
{
    TestSuite *suite;
    TestReporter *reporter;
    int suite_result;

    suite = create_test_suite();
    reporter = create_text_reporter();

//    add_suite(suite, );


    return suite_results;
}

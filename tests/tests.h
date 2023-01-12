//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_TESTS_H
#define DC_SHELL_TESTS_H
#include <cgreen/cgreen.h>

TestSuite *builtin_tests(void);
TestSuite *command_tests(void);
TestSuite *execute_tests(void);
TestSuite *input_tests(void);
TestSuite *shell_impl_tests(void);
TestSuite *util_tests(void);

#endif //DC_SHELL_TESTS_H

//
// Created by Vasily Shorin on 2023-01-10.
//

#include <dc_error/error.h>
#include <dc_env/env.h>
#include <stdio.h>
#include "shell.h"

static void trace_reporter(const struct dc_env *env,
                           const char *file_name,
                           const char *function_name,
                           size_t line_number);

static int run(const struct dc_env *env, struct dc_error *err);


int main(int argc, char** argv)
{
    dc_env_tracer tracer;
    struct dc_error *err;
    struct dc_env *env;

    tracer = trace_reporter;
    err = dc_error_create(false);
    env = dc_env_create(err, false, NULL);

    run(env, err);
    return 0;
}


static void trace_reporter(const struct dc_env *env, const char *file_name,
                           const char *function_name, size_t line_number)
{
    fprintf(stderr, "TRACER: %s : %s @ %zu\n", file_name, function_name, line_number);
}

static int run(const struct dc_env *env, struct dc_error *err)
{
    int ret_val;

    DC_TRACE(env);
    ret_val = run_shell(env, err, stdin, stdout, stderr);
    return ret_val;
}

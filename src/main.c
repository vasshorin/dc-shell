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
    env = dc_env_create(err, false, tracer);

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



// ###### OLD

//int main(int argc, char** argv)
//{
//    dc_env_tracer tracer;
//    struct dc_error *err;
//    struct dc_env *env;
//    struct dc_application_settings *settings;
//    struct dc_application_info *info;
//
//    tracer = trace_reporter;
//    err = dc_error_create(false);
//    env = dc_env_create(err, false, tracer);
//    int ret_val;
////    struct state state;
////    struct dc_fsm_info *fsm;
////
////    static struct dc_fsm_transition transition[] = {
////            {DC_FSM_INIT,       INIT_STATE,        init_state},
////            {INIT_STATE,        READ_COMMANDS,     read_commands},
////            {INIT_STATE,        ERROR,             handle_error},
////            {READ_COMMANDS,     RESET_STATE,       reset_state},
////            {READ_COMMANDS,     SEPARATE_COMMANDS, separate_commands},
////            {READ_COMMANDS,     ERROR,             handle_error},
////            {SEPARATE_COMMANDS, PARSE_COMMANDS,    (dc_fsm_state_func) parse_commands},
////            {SEPARATE_COMMANDS, ERROR,             handle_error},
////            {PARSE_COMMANDS,   EXECUTE_COMMANDS,  execute_commands},
////            {PARSE_COMMANDS,   ERROR,             handle_error},
////            {EXECUTE_COMMANDS, RESET_STATE,       reset_state},
////            {EXECUTE_COMMANDS, ERROR,             handle_error},
////            {EXECUTE_COMMANDS, EXIT,              do_exit},
////            {RESET_STATE, READ_COMMANDS, reset_state},
////            {EXIT, DESTROY_STATE, destroy_state},
////            {ERROR, RESET_STATE, reset_state},
////            {ERROR, DESTROY_STATE, destroy_state},
////            {DESTROY_STATE, DC_FSM_EXIT, NULL}
////    };
////
////    ret_val = EXIT_SUCCESS;
////    fsm = dc_fsm_info_create(env, err, "state");
//    run(env, err);
//////    if(dc_error_has_error(err))
//////    {
////        int from_state;
////        int to_state;
////        ret_val = dc_fsm_run(env, err,fsm, &from_state, &to_state,&state,transition);
////        dc_fsm_info_destroy(env, &fsm);
//////    }
////
////    return ret_val;
//}
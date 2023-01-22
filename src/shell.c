//
// Created by Vasily Shorin on 2023-01-10.
//

#include <stdlib.h>
#include <dc_error/error.h>
#include <dc_env/env.h>
#include "shell.h"
#include "state.h"
#include "shell_impl.h"


int run_shell(const struct dc_env *env, struct dc_error *err, FILE *in, FILE *out, FILE *err_out)
{
    int ret_val;
    struct state state;
    struct dc_fsm_info *fsm;

    state.sin = in;
    state.sout = out;
    state.serr = err_out;

    static struct dc_fsm_transition transition[] = {
            {DC_FSM_INIT,       INIT_STATE,        init_state},
            {INIT_STATE,        READ_COMMANDS,     read_commands},
            {INIT_STATE,        ERROR,             handle_error},
            {READ_COMMANDS,     RESET_STATE,       reset_state},
            {READ_COMMANDS,     SEPARATE_COMMANDS, separate_commands},
            {READ_COMMANDS,     ERROR,             handle_error},
            {SEPARATE_COMMANDS, PARSE_COMMANDS,     parse_commands},
            {SEPARATE_COMMANDS, ERROR,             handle_error},
            {PARSE_COMMANDS,   EXECUTE_COMMANDS,  execute_commands},
            {PARSE_COMMANDS,   ERROR,             handle_error},
            {EXECUTE_COMMANDS, RESET_STATE,       reset_state},
            {EXECUTE_COMMANDS, ERROR,             handle_error},
            {EXECUTE_COMMANDS, EXIT,              do_exit},
            {RESET_STATE, READ_COMMANDS, reset_state},
            {EXIT, DESTROY_STATE, destroy_state},
            {ERROR, RESET_STATE, reset_state},
            {ERROR, DESTROY_STATE, destroy_state},
            {DESTROY_STATE, DC_FSM_EXIT, NULL}
    };

    ret_val = EXIT_SUCCESS;
    fsm = dc_fsm_info_create(env, err, "state");
    if(dc_error_has_no_error(err))
    {
        int from_state;
        int to_state;
        ret_val = dc_fsm_run(env, err,fsm, &from_state, &to_state,&state,transition);
        dc_fsm_info_destroy(env, &fsm);
    }

    return ret_val;
}

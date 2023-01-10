//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_SHELL_H
#define DC_SHELL_SHELL_H
#include <dc_fsm/fsm.h>

enum shell_state
{
    INIT_STATE = DC_FSM_USER_START, // initial state
    READ_COMMANDS,                  // accept user input
    SEPARATE_COMMANDS,              // separate the commands
    PARSE_COMMANDS,                 // parse the commands
    EXECUTE_COMMANDS,               // execute the commands
    EXIT,                           // exit the shell
    RESET_STATE,                    // reset the stae
    ERROR,                          // handle errors
    DESTROY_STATE,                  // destroy the state
};

/**
 * Run the shell FSM.
 *
 * @param env the posix environment
 */
int run_shell(const struct dc_posix_env *env, struct dc_error *err);

#endif //DC_SHELL_SHELL_H

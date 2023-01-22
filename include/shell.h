//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_SHELL_H
#define DC_SHELL_SHELL_H
#include <dc_fsm/fsm.h>
#include <stdio.h>
#include "dc_env/env.h"
#include "dc_error/error.h"

enum shell_state
{
    INIT_STATE = DC_FSM_USER_START, // initial state            //2
    READ_COMMANDS,                  // accept user input        //3
    SEPARATE_COMMANDS,              // separate the commands    //4
    PARSE_COMMANDS,                 // parse the commands       //5
    EXECUTE_COMMANDS,               // execute the commands     //6
    EXIT,                           // exit the shell           //7
    RESET_STATE,                    // reset the state          //8
    ERROR,                          // handle errors            //9
    DESTROY_STATE,                  // destroy the state        //10
};

/**
 * Run the shell FSM.
 *
 *
 * @param env the posix environment
 * @param err the error object
 * @param in the stream to read from
 * @param out the keaboard stdout file
 * @param err the keyboard stderr file
 */
int run_shell(const struct dc_env *env, struct dc_error *err, FILE *in, FILE *out, FILE *err_out);

#endif //DC_SHELL_SHELL_H

//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_SHELL_IMPL_H
#define DC_SHELL_SHELL_IMPL_H

#include "dc_env/env.h"
#include "dc_error/error.h"
#include "dc_c/dc_stdio.h"
#include "dc_posix/dc_regex.h"
#include "dc_posix/dc_unistd.h"
#include "dc_util/filesystem.h"
#include "state.h"
#include "shell.h"
/**
* Set up the inital state:
* - in_redirect_regex  "[ \t\f\v]<.*"
* - out_redirect_regex "[ \t\f\v][1^2]?>[>]?.*"
* - err_redirect_regex "[ \t\f\v]2>[>]?.*"
* - path the PATH env cat separated into directories
* - promt the PS1 env var or "$" if PS1 not set
* - max_line_length the value of _SC_ARG_MAX (see sysconf)
*
* @param env the posix environment
* @param err the error object
* @param arg the current struct state
* @return READ_COMMANDS or INIT_ERROR
*/
int init_state(const struct dc_env *env, struct dc_error *err, void *arg);

/**
* Free any dynamically allocated memory in the state and sets variables to NULL, 0 , or false.
*
* @param env the posix environment.
* @param err the error object.
* @param arg the current struct state
* @return DC_FSM_EXIT
*/
int destroy_state(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * Reset the state for the next read (see do_reset_state, free any dynamically allocated memory.
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param arg the current struct state
 * @return READ_COMMANDS
 */
int reset_state(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * Prompt the user and read the command line (see read_command_line).
 * Sets the state-->current_line and current_line_length.
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param arg the current struct state
 * @return SEPARATE_COMMANDS or READ_COMMANDS
 */
int read_commands(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * Parse the command line into separate commands (see parse_commands).
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param arg the current struct state
 * @return EXECUTE_COMMANDS or PARSE_ERROR
 */
int parse_commands(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * Execute the commands (see execute_commands).
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param arg the current struct state
 * @return EXIT if(command->command is exit), RESET_STATE or EXECUTE_ERROR
 */
int execute_commands(const struct dc_env *env, struct dc_error *err, void *arg);

/**
 * Handle the exit command (see do_reset_state)
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param arg the current struct state
 * @return DESTROY_STATE
 */
int do_exit(const struct dc_env *env, struct dc_error *err, void *arg);


/**
 * Print the err->message to stderr and reset the err (see dc_error_reset).
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param arg the current struct state
 * @return RESET_STATE or DESTROY_STATE (if state->fatal_error is true)
 */
int handle_error(const struct dc_env *env, struct dc_error *err, void *arg);

#endif //DC_SHELL_SHELL_IMPL_H

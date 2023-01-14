//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_UTIL_H
#define DC_SHELL_UTIL_H

#include "dc_env/env.h"
#include "dc_error/error.h"
#include "dc_posix/dc_stdlib.h"
#include "dc_c/dc_stdlib.h"
#include <dc_c/dc_string.h>
#include <dc_util/strings.h>
#include <dc_c/dc_stdio.h>
#include "state.h"
#include <string.h>

/**
* Get the prompt to use.
*
* @param env the posix environment.
* @param err the error object.
* @return var of the PS1 env var or "$ " if PS1 is not set.
*/
const char *get_prompt(const struct dc_env *env, struct dc_error *err);

/**
* Get the PATH env var.
*
* @param env the posix environment.
* @param err the error object.
* @return the PATH env var
*/
char *get_path(const struct dc_env *env, struct dc_error *err);

/**
* Separate a path (eg. PATH env var) into separate directories.
* Directories are separated with a ':' character.
* Any directories with ~ converted to the users home directories.
*
* @param env the posix environment.
* @param err the error object.
* @param path_str the string to separate
* @return the directories that make up the path
*/
char **parse_path(const struct dc_env *env, struct dc_error *err, const  char *path_str);

/**
* Reset the state for the next read, free any dynamically allocated memory.
*
* @param env the posix environment.
* @param err the error object.
*/
void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state);

/**
 * Displays the state values to the given stream.
 *
 * @param env the posix environment.
 * @param state the state to display.
 * @param stream the strea to display the state on.
 */
 void display_state(const struct dc_env *env, struct state *state, FILE *stream);

/**
* Disp
*
* @param env the posix environment.
* @param state the state to display.
* @param stream the strea to display the state on.
*/
void *state_to_string(const struct dc_env *env, struct state *state);


#endif //DC_SHELL_UTIL_H

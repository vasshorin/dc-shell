//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_SHELL_IMPL_H
#define DC_SHELL_SHELL_IMPL_H


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
 int init_state(const struct dc_posix_env *env, struct dc_error *err, void *arg);

 /**
  * Free any dynamically allocated memory in the state.
  *
  * @param env the posix environment.
  */
 void free_state(const struct dc_posix_env *env);
#endif //DC_SHELL_SHELL_IMPL_H

//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_BUILTINS_H
#define DC_SHELL_BUILTINS_H

#include "command.h"
#include "dc_error/error.h"
#include "dc_env/env.h"

/**
 * Change the working directory.
 * ~ is converted to the users home directory.
 * - no arguments is converted to the users home directory.
 * The command->exit_code is set to 0 on success or err->errno_code on failure.
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param command the command information.
 */
 void builtin_cd(const struct dc_env *env, struct dc_error *err, struct command *command, FILE *errstream);

#endif //DC_SHELL_BUILTINS_H

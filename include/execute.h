//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_EXECUTE_H
#define DC_SHELL_EXECUTE_H
#include "dc_env/env.h"
#include "dc_error/error.h"
#include "command.h"

/**
 * Create a child process, exec the command with any redirection, set the exit code.
 * If there is an error executing the command print an error message.
 * If the command cannot be found set the command->exit_code to 127.
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param command the command to execute.
 * @param path the directories to search for the commands
 */

void execute(const struct dc_env *env, struct dc_error *err, struct command *command, char *path);

#endif //DC_SHELL_EXECUTE_H

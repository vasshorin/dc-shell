//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_COMMAND_H
#define DC_SHELL_COMMAND_H
#include "dc_error/error.h"
#include "dc_env/env.h"
#include "state.h"

struct command
{
    char *line;             // the current command line
    char *command;          // the program/builtin to run
    size_t argc;            // the number of arguments
    char **argv;            // the arguments to the command
    char *stdin_file;       // the file to redirect stdin to
    char *stdout_file;      // the file to redirect stdout to
    bool stdout_overwrite;  // append or overwrite the stdout file (true = overwrite)
    char *stderr_file;      // the file to redirect stderr to
    bool stderr_overwrite;       // append or overwrite the stderr file (true = overwrite)
    int exit_code;
};

/**
 * Parse the command. Take the command->line and use it to full in all of the fields.
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param state the current state, to set the fatal_error and access the command line and regex for redirection
 * @param command the command to parse.
 */
 void parse_command(struct dc_env *env, struct dc_error *err, struct state *state, struct command *command);
#endif //DC_SHELL_COMMAND_H

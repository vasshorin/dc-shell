//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_INPUT_H
#define DC_SHELL_INPUT_H
#include "dc_env/env.h"
#include "dc_error/error.h"
#include "stdlib.h"
#include "stdio.h"

/**
 * Read the command line from the user.
 *
 * @param env the posix environment.
 * @param err the error object.
 * @param strea the stream to read from (stdin).
 * @param line_size the maximum line to read.
 * @return the comand lie that the user entered.
 */
 char *read_command_line(const struct dc_env *env, struct dc_error *err, FILE *stream, size_t line_size);
#endif //DC_SHELL_INPUT_H

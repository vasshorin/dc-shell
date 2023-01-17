//
// Created by Vasily Shorin on 2023-01-10.
//


#include <dc_c/dc_stdlib.h>
#include <string.h>
#include "input.h"



char *read_command_line(const struct dc_env *env, struct dc_error *err, FILE *stream, size_t *line_size)
{
    DC_TRACE(env);
    char *line = NULL;
    size_t len = 0;

    if ((dc_getline(env, err, &line, &len, stream)) != -1)
    {
        dc_str_trim(env,line);
        *line_size = strlen(line);
    }
    else
    {
    *line_size = 0;
    }

    return line;
}

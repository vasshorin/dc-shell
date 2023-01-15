//
// Created by Vasily Shorin on 2023-01-10.
//

#include "../include/input.h"


char *read_command_line(const struct dc_env *env, struct dc_error *err, FILE *stream, size_t *line_size)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stream);
    if (read == -1) {
//        dc_error_set(err, "Failed to read command line");
        printf("Failed to read command line");
        return NULL;
    }
    *line_size = read;
    return line;
}
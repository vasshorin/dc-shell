//
// Created by Vasily Shorin on 2023-01-10.
//

#include "util.h"
#include <dc_c/dc_string.h>


const char *get_prompt(const struct dc_env *env, struct dc_error *err) {
    const char *prompt = dc_getenv(env, "PS1");
    if (prompt == NULL) {
        prompt = "$ ";
    }
    return prompt;
}

char *get_path(const struct dc_env *env, struct dc_error *err)
{
char *path = dc_getenv(env, "PATH");
    return path;
}

void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state)
{
    DC_TRACE(env);

    dc_free(env, state->current_line);
    state->current_line = NULL;

    if(state->command)
    {
        dc_free(env, state->command->line);
    }
#pragma unroll 1
    for(size_t i = 0; i < state->command->argc; i++)
    {
        if(state->command->argv[i])
        {
            dc_free(env, state->command->argv[i]);
        }
    }

    dc_free(env, state->command->argv);
    dc_free(env, state->command);
    state->command = NULL;
}

void *state_to_string(const struct dc_env *env, struct state *state)
{
    size_t len;
    char *line;

    if(state->current_line == NULL)
    {
        len = strlen("current_line = NULL ");
    }
    else
    {
        len = strlen("current line = ");
        len += state->current_line_length;
        len += strlen(", current_line_length = ");
    }


    len += strlen(", fatal_error = ");
    line = malloc(len + 1);

if(state->current_line == NULL)
    {
        sprintf(line, "current_line = NULL, fatal_error = %d", state->fatal_error);
    }
else
    {
        sprintf(line, "current_line = %s, fatal_error = %d", state->current_line, state->fatal_error);
    }

    return line;
}

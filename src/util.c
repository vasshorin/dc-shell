//
// Created by Vasily Shorin on 2023-01-10.
//

#include "util.h"
#include <dc_c/dc_string.h>


char *get_path(const struct dc_env *env, struct dc_error *err) {
    DC_TRACE(env);
    char *path = dc_getenv(env, "PATH");

    if (path == NULL) {
        // Get the default path from the system.
        path = getenv("PATH");
        if (path == NULL) {
            path = strdup("/bin:/usr/bin:/usr/local/bin:/usr/local/sbin");
        } else {
            path = strdup(path);
        }
    }

    return path;
}

void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state) {
    DC_TRACE(env);

    if (state->current_line) {
        dc_free(env, state->current_line);
        state->current_line = NULL;
        state->current_line_length = 0;
    }

    if (state->command) {
        dc_free(env, state->command->line);
        dc_free(env, state->command->command);
        dc_free(env, state->command->stdin_file);
        dc_free(env, state->command->stdout_file);
        dc_free(env, state->command->stderr_file);
        for (size_t i = 0; i < state->command->argc; i++) {
            dc_free(env, state->command->argv[i]);
        }
        dc_free(env, state->command->argv);
        dc_free(env, state->command);
    }
    state->command = NULL;
}



void display_state(const struct dc_env *env, struct state *state, FILE *stream)
{
    char *str;

    str = state_to_string(env, state);
    fprintf(stream, "%s", str);
    free(str);
}

void *state_to_string(const struct dc_env *env, struct state *state)
{
    size_t len;
    char *line;

    if(state->current_line == NULL)
    {
        len = strlen("current_line = NULL");
    }
    else
    {
        len = strlen("current line = \"\"");
        len += state->current_line_length;
    }


    len += strlen(", fatal_error = ");
    // + 1 for 0 or 1 for the fatal_error + 1 for the null byte
    line = malloc(len + 1 + 1);

    if(state->current_line == NULL)
        {
            sprintf(line, "current_line = NULL, fatal_error = %d", state->fatal_error);
        }
    else
        {
            sprintf(line, "current_line = \"%s\", fatal_error = %d", state->current_line, state->fatal_error);
        }

        return line;
}

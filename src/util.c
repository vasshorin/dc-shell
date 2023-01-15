//
// Created by Vasily Shorin on 2023-01-10.
//

#include "util.h"
#include <dc_c/dc_string.h>


const char *get_prompt(const struct dc_env *env, struct dc_error *err)
{
    DC_TRACE(env);

    const char *prompt;

    if ( (prompt = dc_getenv(env, "PS1")) == NULL) {
        prompt = "$ ";
    }
    else
    {
        prompt = strdup(prompt);
    }

    return prompt;
}

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

char **parse_path(const struct dc_env *env, struct dc_error *err, const char *path_str)
{
    DC_TRACE(env);
    char **path = NULL;
    char *path_str_copy = strdup(path_str);
    char *path_str_copy_ptr = path_str_copy;
    char *token = NULL;
    char *state = NULL;
    size_t path_len = 0;
    char **list;

    while ((token = strtok_r(path_str_copy_ptr, ":", &state)) != NULL)
    {
        path_str_copy_ptr = NULL;
        path_len++;
        list = realloc(path, sizeof(char *) * path_len);
        if (list == NULL)
        {
            dc_error_set_reporting(err, "realloc");
            return NULL;
        }
        path = list;
        path[path_len - 1] = strdup(token);
    }
    free(path_str_copy);
    return path;
}


void do_reset_state(const struct dc_env *env, struct dc_error *err, struct state *state) {
    DC_TRACE(env);

    if (state->current_line) {
        dc_free(env, state->current_line);
        state->current_line = NULL;
        state->current_line_length = 0;
        state->fatal_error = false;
    }

    state->fatal_error = false;

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

    fprintf(stream, "%s", str);
    free(str);
}

void *state_to_string(const struct dc_env *env,struct dc_error *err, struct state *state)
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

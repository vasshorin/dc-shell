//
// Created by Vasily Shorin on 2023-01-10.
//

#include <unistd.h>
#include <dc_c/dc_stdlib.h>
#include "shell_impl.h"
#include "util.h"
#include "state.h"


int init_state(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);
//    printf("Shell impl - PS1 value : %s\n", dc_getenv(env, "PS1"));
    struct state *state = (struct state *) arg;

    state->max_line_length = sysconf(_SC_ARG_MAX);
    if (state->max_line_length < 0)
    {
        dc_error_set_reporting(err, "sysconf(_SC_ARG_MAX) failed");
        state->fatal_error = true;
        return ERROR;
    }

    state->in_redirect_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->in_redirect_regex, "[ \t\f\v]<.*", REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for in_redirect_regex failed");
        state->fatal_error = true;
        return ERROR;
    }

    state->out_redirect_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->out_redirect_regex, "[ \t\f\v][1^2]?>[>]?.*", REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for out_redirect_regex failed");
        state->fatal_error = true;
        return ERROR;
    }

    state->err_redirect_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->err_redirect_regex, "[ \t\f\v]2>[>]?.*", REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for err_redirect_regex failed");
        state->fatal_error = true;
        return ERROR;
    }

    char *path_env = dc_getenv(env, "PATH");
    if (path_env == NULL)
    {
        dc_error_set_reporting(err, "getenv for PATH failed");
        state->fatal_error = true;
        return ERROR;
    }
    else
    {
        state->path = parse_path(env, err, path_env);
    }

    char *prompt_env = dc_getenv(env, "PS1");
    if (prompt_env == NULL)
    {
        state->prompt = "$ ";
    }
    else
    {
        state->prompt = prompt_env;
    }

    state->current_line = NULL;
    state->current_line_length = 0;
    state->command = NULL;
    state->fatal_error = false;
//    printf("Current line %s\n", state->current_line);
//    printf("Current line length %ld\n", state->current_line_length);
//    printf("Command %s\n", state->command);
//    printf("Prompt %s\n", state->prompt);
    return READ_COMMANDS;
}

int destroy_state(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);

    struct state *state = (struct state *) arg;
//    printf("#########DESTROY STATE###########");
//    printf("Current line %s\n", state->current_line);
//    printf("Current line length %ld\n", state->current_line_length);
//    printf("Command %s\n", state->command);
//    printf("Prompt %s\n", state->prompt);

    if(state->in_redirect_regex != NULL)
    {
//        printf("Inside in_redirect_regex \n");
        dc_regfree(env, state->in_redirect_regex);
        dc_free(env, state->in_redirect_regex);
        state->in_redirect_regex = NULL;
    }

    if(state->out_redirect_regex != NULL)
    {
//        printf("Inside out_redirect_regex\n");
        dc_regfree(env, state->out_redirect_regex);
        dc_free(env, state->out_redirect_regex);
        state->out_redirect_regex = NULL;
    }

    if(state->err_redirect_regex != NULL)
    {
//        printf("Inside err_redirect_regex\n");
        dc_regfree(env, state->err_redirect_regex);
        dc_free(env, state->err_redirect_regex);
        state->err_redirect_regex = NULL;
    }

    if(state->path != NULL)
    {
//        printf("Inside path\n");
        dc_free(env, state->path);
        state->path = NULL;
    }

    if(state->prompt != NULL)
    {
//        printf("Inside prompt %s\n", state->prompt);
        state->prompt = NULL;
//        dc_free(env, state->prompt);
    }

    if(state->current_line != NULL)
    {
//        printf("Inside current line free\n");
        dc_free(env, state->current_line);
        state->current_line = NULL;
    }

    if(state->max_line_length != 0)
    {
//        printf("Inside max line length free\n");
        state->max_line_length = 0;
    }

    if(state->command != NULL)
    {
//        printf("Inside command free\n");
        dc_free(env, state->command);
        state->command = NULL;
    }

    if(state->fatal_error != false)
    {
//        printf("Inside fatal error free\n");
        state->fatal_error = false;
    }
    return DC_FSM_EXIT;

}


int reset_state(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);
    struct state *state = (struct state *) arg;
    do_reset_state(env, err, state);

    return READ_COMMANDS;
}

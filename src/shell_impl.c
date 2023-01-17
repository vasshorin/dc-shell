//
// Created by Vasily Shorin on 2023-01-10.
//

#include <unistd.h>
#include <dc_c/dc_stdlib.h>
#include <dc_posix/dc_stdio.h>
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
        state->prompt = strdup("$ ");
    }
    else
    {
        // state->promt is set to $ + PS1
//        state->prompt = calloc(1, strlen(prompt_env) + 3);
//        strcat(state->prompt, "$ ");
//        strcat(state->prompt, prompt_env);
        state->prompt = strdup(prompt_env);
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
        dc_regfree(env, state->in_redirect_regex);
        dc_free(env, state->in_redirect_regex);
        state->in_redirect_regex = NULL;
    }

    if(state->out_redirect_regex != NULL)
    {
        dc_regfree(env, state->out_redirect_regex);
        dc_free(env, state->out_redirect_regex);
        state->out_redirect_regex = NULL;
    }

    if(state->err_redirect_regex != NULL)
    {
        dc_regfree(env, state->err_redirect_regex);
        dc_free(env, state->err_redirect_regex);
        state->err_redirect_regex = NULL;
    }

    if(state->path != NULL)
    {
        dc_free(env, state->path);
        state->path = NULL;
    }

    if(state->prompt != NULL)
    {
        state->prompt = NULL;
//        dc_free(env, state->prompt);
    }

    if(state->current_line != NULL)
    {
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


int read_commands(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);
    struct state *state = (struct state *) arg;
    char *cwd;
    char output[1024];

    // if an error getting the current working directory, set fatal_error to true and return ERROR
    if((cwd = dc_get_working_dir(env, err)) == NULL)
    {
        dc_error_set_reporting(err, "getcwd failed");
        state->fatal_error = true;
        return ERROR;
    }
    //    print “[current working directory] state.prompt” to stdout
    sprintf(output, "[%s] %s", cwd, state->prompt);
    fprintf(state->sout, "[%s] %s", cwd, state->prompt);

    //    read the input from state.stdin into state.current_line
    if(dc_getline(env, err, &state->current_line, &state->max_line_length, state->sin) < 0)
    {
        dc_error_set_reporting(err, "getline failed");
        state->fatal_error = true;
        return ERROR;
    }

    dc_str_trim(env, state->current_line);
    //    If empty string
    //    return RESET_STATE
    if(state->current_line[0] == '\0')
    {
        return RESET_STATE;
    }
    state->current_line_length = strlen(state->current_line);

    return SEPARATE_COMMANDS;

}


int separate_commands(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);

    struct state *state = (struct state *) arg;
    char *command = state->current_line;

    //If any errors occur
    //set state.fatal_error to true and return ERROR
    if(state->fatal_error == true)
    {
        return ERROR;
    }
    // Copy the state.current_line to the state.command.line
    state->command = dc_malloc(env,err, sizeof(struct command));
    // Set all other fields to zero, NULL, or false
    state->command->line = dc_malloc(env, err, state->current_line_length + 1);
    dc_strcpy(env, state->command->line, state->current_line);
    state->command->argc = 0;
    state->command->argv = NULL;
    state->command->stdout_overwrite = NULL;
    state->command->stdin_file= NULL;
    state->command->stderr_file = NULL;
    state->command->stdout_file = NULL;
    state->command->stderr_overwrite = NULL;

    return PARSE_COMMANDS;
}

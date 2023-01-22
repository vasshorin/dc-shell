//
// Created by Vasily Shorin on 2023-01-10.
//

#include <unistd.h>
#include <dc_c/dc_stdlib.h>
#include <dc_posix/dc_stdio.h>
#include "shell_impl.h"
#include "util.h"
#include "state.h"
#include "builtins.h"
#include "execute.h"


int init_state(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);
    struct state *state = (struct state *) arg;
    dc_memset(env, state, 0, sizeof(struct state));
    if(dc_error_has_error(err))
    {
        state->fatal_error = true;
        return ERROR;
    }
    long max_line = 0;
    max_line = dc_sysconf(env, err, _SC_ARG_MAX);
    if(dc_error_has_no_error(err))
    {
        state->max_line_length = (size_t)max_line;
    }
    if(state->max_line_length < 0)
    {
        DC_ERROR_RAISE_SYSTEM(err, "Blah blah", 1);
        state->fatal_error = true;
        return ERROR;
    }
    state->in_redirect_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->in_redirect_regex, "([ \t\f\v]<.*)", REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for in_redirect_regex failed");
        state->fatal_error = true;
        return ERROR;
    }
    state->out_redirect_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->out_redirect_regex, "([ \t\f\v][1^2]?>[>]?.*)", REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for out_redirect_regex failed");
        state->fatal_error = true;
        return ERROR;
    }
    state->err_redirect_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->err_redirect_regex, "([ \t\f\v]2>>?)([ \t\f\v].*)"
            , REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for err_redirect_regex failed");
        state->fatal_error = true;
        return ERROR;
    }
    state->command_regex = calloc(1, sizeof(regex_t));
    if (regcomp(state->command_regex, "\"([^<>]*).*\"", REG_EXTENDED) != 0)
    {
        dc_error_set_reporting(err, "regcomp for command_regex failed");
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
        state->prompt = strdup(prompt_env);
    }
    state->current_line = NULL;
    state->current_line_length = 0;
    state->command = NULL;
    state->fatal_error = false;
    state->sout = stdout;
    state->sin = stdin;
    state->serr = stderr;

    return READ_COMMANDS;
}

int destroy_state(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);

    struct state *state = (struct state *) arg;

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
        dc_free(env, state->prompt);
    }

    if(state->current_line != NULL)
    {
        dc_free(env, state->current_line);
        state->current_line = NULL;
    }

    if(state->max_line_length != 0)
    {
        state->max_line_length = 0;
    }

    if(state->command != NULL)
    {
        dc_free(env, state->command);
        state->command = NULL;
    }

    if(state->fatal_error != false)
    {
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

    if((cwd = dc_get_working_dir(env, err)) == NULL)
    {
        DC_ERROR_RAISE_SYSTEM(err, "getcwd failed", 1);
        state->fatal_error = true;
        return ERROR;
    }
    fprintf(state->sout, "[%s] %s", cwd, state->prompt);

    if(dc_getline(env, err, &state->current_line, &state->max_line_length, state->sin) < 0)
    {
        DC_ERROR_RAISE_SYSTEM(err, "get_line failed\n", 1);
        state->fatal_error = true;
        return ERROR;
    }

    dc_str_trim(env, state->current_line);
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

    if(state->fatal_error == true)
    {
        return ERROR;
    }
    state->command = dc_calloc(env,err, 1, sizeof(struct command));
    state->command->line = dc_calloc(env, err, state->current_line_length, sizeof(char*));
    dc_strcpy(env, state->command->line, state->current_line);
    state->command->argc = 0;
    state->command->argv = NULL;
    state->command->stdout_overwrite = false;
    state->command->stdin_file= NULL;
    state->command->stderr_file = NULL;
    state->command->stdout_file = NULL;
    state->command->stderr_overwrite = false;

    return PARSE_COMMANDS;
}


int parse_commands(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);
    struct state *state = (struct state *) arg;
    struct command *command = state->command;
    parse_command(env, err, state, command);
    if(state->fatal_error == true)
    {
        return ERROR;
    }

    return EXECUTE_COMMANDS;
}

int execute_commands(const struct dc_env *env, struct dc_error *err, void *arg)
{
    DC_TRACE(env);
    struct state *state = (struct state *) arg;
    struct command *command = state->command;
    if(command->command != NULL && dc_strcmp(env, command->command, "cd") == 0)
    {
        builtin_cd(env, err, command, stderr);
    }
    else if (command->command != NULL && dc_strcmp(env, command->command, "exit") == 0)
    {
        printf("going into exit\n");
        return EXIT;
    }
    else
    {
        execute(env, err, command, *state->path);
        if(state->fatal_error == true)
        {
            fprintf(stderr, "Error: %d", command->exit_code);
            return ERROR;
        }
    }
    if(state->fatal_error == true)
    {
        return ERROR;
    }
    fprintf(stdout, "%d\n", command->exit_code);
    return RESET_STATE;
}


int do_exit(const struct dc_env *env, struct dc_error *err, void *arg)
{
//    Call do_reset_state()
    struct state *state = (struct state *) arg;
    do_reset_state(env, err, state);

    return DESTROY_STATE;
}


int handle_error(const struct dc_env *env, struct dc_error *err, void *arg) {
    DC_TRACE(env);
    struct state *state = (struct state *) arg;
    if(state->current_line == NULL)
    {
        fprintf(state->sout, "Internal error %d error", dc_errno_get_errno(err));
    }
    else
    {
        fprintf(state->sout, "Internal error %d error message: %s", dc_errno_get_errno(err), state->current_line);
    }

    if(state->fatal_error)
    {
        return DESTROY_STATE;
    }

    return RESET_STATE;
}

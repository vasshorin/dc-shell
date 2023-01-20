//
// Created by Vasily Shorin on 2023-01-10.
//

#include <dc_c/dc_string.h>
#include <string.h>
#include <stdlib.h>
#include <wordexp.h>
#include <dc_c/dc_stdlib.h>
#include <dc_util/path.h>
#include "command.h"
#include "util.h"
#include "shell_impl.h"
#define REDIRECT_IN 1
#define REDIRECT_OUT 2
#define REDIRECT_ERR 3


static void expand_path(struct dc_env *env, struct dc_error *err,const char *expected_file, char **expanded_file);
char *remove_pattern(struct dc_env *pEnv, struct dc_error *pError, char *line, regex_t *regex);
static void regex(struct dc_env *env, struct dc_error *err, const struct state *state, struct command *command, int type);
static void extract_command_name_and_arguments(struct dc_env *env, struct dc_error *error, struct state *state, struct command *command);
static char *expand_tilde(char *file, struct dc_env *env);


void parse_command(struct dc_env *env, struct dc_error *err, struct state *state, struct command *command)
{
    DC_TRACE(env);
    char *comm;
    char *args_start;
    regmatch_t match[2];
    int status;
    comm = strdup(command->line);
//    printf("Parsing command: %s\n", comm);
    // get the command name
    char *command_name = strtok(comm, " \t\f\v");

//     STDERR redirection
    status = regexec(state->err_redirect_regex, state->command->line, 2, match, 0);
//    printf("\t\t\tREGERR status is %d\n", status);
    if (status == 0)
    {
        char *subexpression = strndup(state->command->line + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
        char *operator = strtok(subexpression, " ");
        char *file_path = strtok(NULL, " ");
//        printf("Operator: %s\n", operator);
//        printf("File path: %s\n", file_path);
        if (!strcmp(operator, "2>>")) {
            command->stderr_overwrite = false;
        } else if (!strcmp(operator, "2>")) {
            command->stderr_overwrite = true;
        }
        command->stderr_file = strdup(file_path);
        expand_path(env, err, file_path, &command->stderr_file);
        free(subexpression);
    } else {
        command->stderr_file = NULL;
    }
//     STDOUT redirection
    status = regexec(state->out_redirect_regex, state->command->line, 2, match, 0);
//    printf("\t\t\tREGOUT status is %d\n", status);
    if (status == 0)
    {
        char *subexpression = strndup(state->command->line + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
        char *operator = strtok(subexpression, " ");
        char *file_path = strtok(NULL, " ");
//        printf("Operator: %s\n", operator);
//        printf("File path: %s\n", file_path);
        if (!strcmp(operator, ">>")) {
            command->stdout_overwrite = false;
        } else if (!strcmp(operator, ">")) {
            command->stdout_overwrite = true;
        }
        command->stdout_file = strdup(file_path);
        expand_path(env, err, file_path, &command->stdout_file);
        free(subexpression);
    } else {
        command->stdout_file = NULL;
    }
//    printf("\t\t\tStdout fle %s\n", command->stdout_file);
    //     STDIN redirection
    status = regexec(state->in_redirect_regex, state->command->line, 2, match, 0);
//    printf("STDIN status %d\n", status);
    if (status == 0)
    {
        char *subexpression = strndup(state->command->line + match[0].rm_so, match[0].rm_eo - match[0].rm_so);
        char *operator = strtok(subexpression, " ");
        char *file_path = strtok(NULL, " ");
//        printf("Operator: %s\n", operator);
//        printf("File path: %s\n", file_path);
        command->stdin_file = strdup(file_path);
        if(command->stdin_file)
        {
            expand_path(env, err, file_path, &command->stdin_file);
        }
        free(subexpression);
    } else {
        command->stdin_file = NULL;
    }
    char *new_string = strdup(command->line);
    char *args = malloc(strlen(new_string) + 1);
    int i = 0, j = 0;
    while(new_string[i] != '\0')
    {
        if(new_string[i] == '>' || new_string[i] == '<')
        {
            break;
        }
        args[j] = new_string[i];
        i++;
        j++;
    }
    args[j] = '\0';
    printf("\t\tnew string is %s\n", args);

    wordexp_t we;
    int ret = wordexp(args, &we, 0);
    state->command->argc = we.we_wordc;
    //set state.command.argv to we_wordv
//    int ret = wordexp(command_name, &result, 0);
    if (ret != 0) {
        // Handle the error
        switch (ret) {
            case WRDE_BADCHAR:
                printf("Error: Illegal occurrence of newline or one of |, &, ;, <, >, (, ), {, }\n");
                break;
            case WRDE_BADVAL:
                printf("Error: Undefined shell variable referenced\n");
                break;
            case WRDE_CMDSUB:
                printf("Error: Command substitution not enabled\n");
                break;
            case WRDE_NOSPACE:
                printf("Error: Out of memory\n");
                break;
            case WRDE_SYNTAX:
                printf("Error: Syntax error in the expression\n");
                break;
            default:
                printf("Error: Unknown error\n");
        }
        // Exit program or return error
        exit(1);
    }
    // Find the number of arguments in the command line before the first IO symbol
    int num_args = 0;
    for (int i = 0; i < we.we_wordc; i++) {
        status = regexec(state->command_regex, we.we_wordv[i], 2, match, 0);
        if (status != 0) {
            num_args++;
        } else {
            break;
        }
    }
    command->argc = num_args;
//    state->command->argc = result.we_wordc;
    printf("argc: %zu\n", state->command->argc);
    // Allocate memory for argv
    state->command->argv = calloc(state->command->argc + 2, sizeof(char *) );
    if (state->command->argv == NULL) {
        state->fatal_error = true;
//        return ERROR;
    }
    // Copy the arguments to argv
    for (size_t i = 1; i < we.we_wordc; i++) {
        state->command->argv[i + 1] = strdup(we.we_wordv[i]);
    }
    // Set the command name
    state->command->command = strdup(we.we_wordv[0]);
    // Set argv[0] to the command name

    // Set argv[argc + 1] to NULL
    state->command->argv[state->command->argc + 1] = NULL;
    // Free the wordexp result
    wordfree(&we);
    free(comm);

//    return EXECUTE_COMMANDS;

}

static void expand_path(struct dc_env *env, struct dc_error *err,const char *expected_file, char **expanded_file)
{
    if(expected_file == NULL)
    {
        expanded_file = NULL;
    }
    else
    {
        dc_expand_path(env, err, expanded_file, expected_file);
    }
}



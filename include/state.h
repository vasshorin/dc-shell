//
// Created by Vasily Shorin on 2023-01-10.
//

#ifndef DC_SHELL_STATE_H
#define DC_SHELL_STATE_H


#include <regex.h>
#include "command.h"
#include <stdbool.h>
#include <stdio.h>

/*! \struct state
*  \brief The state of the shell.
 *
 *  The state of the shell.
 */
struct state
{
    FILE *sin;                   /*!< The standard input stream. */
    FILE *sout;                  /*!< The standard output stream. */
    FILE *serr;                  /*!< The standard error stream. */
    regex_t *in_redirect_regex;  /*!< The regex for in redirection. */
    regex_t *out_redirect_regex; /*!< The regex for out redirection. */
    regex_t *err_redirect_regex; /*!< The regex for err redirection. */
    char **path;                 /*!< The PATH env var. */
    char *prompt;                /*!< The PS1 env var. */
    size_t max_line_length;      /*!< The max line length. */
    char *current_line;          /*!< The current line. */
    size_t current_line_length;  /*!< The current line length. */
    struct command *command;     /*!< The current command. */
    bool fatal_error;            /*!< A fatal error has occurred. */
};

#endif //DC_SHELL_STATE_H

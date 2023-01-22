//
// Created by Vasily Shorin on 2023-01-10.
//

#include <string.h>
#include <dc_posix/dc_unistd.h>
#include <dc_util/path.h>
#include <stdlib.h>
#include "builtins.h"

void builtin_cd(const struct dc_env *env, struct dc_error *err, struct command *command, FILE *errstream)
{
    DC_TRACE(env);
    char *path;
    int exit_code = 0;
    char *dir = NULL;

    if (command->argv[1] == NULL)
    {
        dir = strdup("~/");
    } else
    {
        dir = strdup(command->argv[1]);
    }
    dc_expand_path(env, err, &path, dir);

    if (dc_chdir(env, err, path) != 0)
    {
        switch (errno)
        {
            case EACCES:
                fprintf(stderr, "EACCES: %s: %s\n", path, strerror(errno));
                exit_code = 1;
                break;
            case ELOOP:
                printf("ELOOP: %s: %s\n", path, strerror(errno));
                exit_code = 1;
                break;
            case ENAMETOOLONG:
                printf("ENAMETOOLONG: %s: %s\n", path, strerror(errno));
                exit_code = 1;
                break;
            case ENOENT:
                fprintf(stderr, "ENOENT: %s: %s\n", path, strerror(errno));
                exit_code = 1;
                break;
            case ENOTDIR:
                printf("ENOTDIR: %s: %s\n", path, strerror(errno));
                exit_code = 1;
                break;
            default:
                dc_error_set_reporting(err, "Unknown error in builtin_cd");
                exit_code = 1;
                break;
        }
    }
    command->exit_code = exit_code;
    free(dir);
    free(path);
}

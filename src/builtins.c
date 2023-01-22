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
    int result = chdir(path);
    if (result != 0)
    {
        switch (errno)
        {
            case EACCES:
                printf("cd: %s: Permission denied\n", path);
                exit_code = 1;
                break;
            case ELOOP:
                printf("cd: %s: Too many symbolic links\n", path);
                exit_code = 1;
                break;
            case ENAMETOOLONG:
                printf("cd: %s: File name too long\n", path);
                exit_code = 1;
                break;
            case ENOENT:
                printf("cd: %s: No such file or directory\n", path);
                exit_code = 1;
                break;
            case ENOTDIR:
                printf("cd: %s: Not a directory\n", path);
                exit_code = 1;
                break;
            default:
                printf("cd: %s: Unknown error\n", path);
                exit_code = 1;
                break;
        }
    }
    command->exit_code = exit_code;
    free(dir);
    free(path);
}

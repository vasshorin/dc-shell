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
    // Initialize variables
    int exit_code = 0;
    char *dir = NULL;

    // Check if command->argv[1] is NULL and set path to "~/" if it is
    printf("Command->argv[0] = %s\n", command->argv[0]);
    printf("command->argv[1] = %s\n", command->argv[1]);
    if (command->argv[1] == NULL)
    {
        dir = strdup("~/");
    } else
    {
        dir = strdup(command->argv[1]);
    }
    // Expand the path to change ~ to the user's home directory
    dc_expand_path(env, err, &path, dir);

    // Call chdir
    if (dc_chdir(env, err, path) != 0)
    {
        // Handle chdir errors
        switch (errno)
        {
            case EACCES:
                printf("EACCES: %s: %s\n", path, strerror(errno));
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
                printf("ENOENT: %s: %s\n", path, strerror(errno));
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
    printf("Exit code %d", command->exit_code);
    command->exit_code = exit_code;
    // Clean up
    free(dir);
    free(path);
}

//
// Created by Vasily Shorin on 2023-01-10.
//

#include <sys/types.h>
#include <dc_posix/dc_unistd.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <string.h>
#include <dc_c/dc_string.h>
#include <dc_c/dc_stdlib.h>
#include "execute.h"
#include "shell_impl.h"


static void redirect(struct command *command);
static void run(struct command *command, char *path);
static int handle_run_error(struct dc_error *err, struct command *command);
void execute(const struct dc_env *env, struct dc_error *err, struct command *command, char *path)
{
    DC_TRACE(env);
        pid_t pid;
        int status;

        pid = fork();
        if(pid < 0)
        {
            dc_error_set_reporting(err, "Fork failed");
            return;
        }
        else if(pid == 0)
        {
            redirect(command);
            run(command, path);
            status = handle_run_error(err, command);
            exit(status);
        }
        else
        {
            waitpid(pid, &status, 0);
            command->exit_code = WEXITSTATUS(status);
        }
//        if (pid == 0) { // child process
//            redirect(command); // perform I/O redirection
//            run(command, &path); // run the command
////            Set status to return from call handle_run_error()
////            call exit(status)
//
//
//
//            if (strchr(command->command, '/') != NULL) {
//                command->argv[0] = command->command;
//                execv(command->command, command->argv);
//            } else {
//                if (path == NULL) {
//                    printf("Command not found: %s\n", command->command);
//                    exit(127);
//                } else {
//                    char *cmd = malloc(strlen(path) + strlen(command->command) + 2);
//                    for (int i = 0; path[i]; i++) {
//                        strcpy(cmd, &path[i]);
//                        strcat(cmd, "/");
//                        strcat(cmd, command->command);
//                        command->argv[0] = cmd;
//                        execv(cmd, command->argv);
//                        if (errno != ENOENT) {
//                            break;
//                        }
//                    }
//                    free(cmd);
//                    printf("Command not found: %s\n", command->command);
//                    exit(127);
//                }
//            }
//        } else { // parent process
//            waitpid(pid, &status, 0);
//            command->exit_code = WEXITSTATUS(status);
//        }

}

static void redirect(struct command *command)
{
    int fd;

    if(command->stdin_file != NULL) {
//        printf("Child process before redirecting stdin");
        fd = open(command->stdin_file, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "Failed to open file %s for reading", command->stdin_file);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if(command->stdout_file != NULL) {
//        printf("Child process before redirecting stdout");
        if(command->stdout_overwrite) {
            fd = open(command->stdout_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        } else {
            fd = open(command->stdout_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }
        if (fd < 0) {
            fprintf(stderr, "Error opening output file: %s", command->stdout_file);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    if(command->stderr_file != NULL) {
//        printf("Child process before redirecting stderr");
        if(command->stderr_overwrite) {
            fd = open(command->stderr_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        } else {
            fd = open(command->stderr_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }
        if (fd < 0) {
            fprintf(stderr, "Error opening error file");
        }
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
//    printf("Child process after redirecting stderr");
}

static void run (struct command *command, char *path)
{
    if (strchr(command->command, '/') != NULL) {
        command->argv[0] = command->command;
        execv(command->command, command->argv);
    } else {
        if (path == NULL) {
            printf("Command not found: %s\n", command->command);
            exit(127);
        } else {
            char *cmd = malloc(strlen(path) + strlen(command->command) + 2);
            for (int i = 0; path[i]; i++) {
                strcpy(cmd, &path[i]);
                strcat(cmd, "/");
                strcat(cmd, command->command);
                command->argv[0] = cmd;
                execv(cmd, command->argv);
                if (errno != ENOENT) {
                    break;
                }
            }
            free(cmd);
            printf("Command not found: %s\n", command->command);
            exit(127);
        }
    }
}

//static int run(struct command *command, char **path)
//{
//    char cmd[_SC_ARG_MAX];
//    printf("Going to run: %s\n", command->command);
//    if (strchr(command->command, '/') != NULL) {
//        printf("Found slash\n");
//        command->argv[0] = command->command;
//        if (execv(command->command, command->argv) == -1) {
//            perror("execve");
//        }
//    } else {
//        if (path == NULL) {
//            errno = ENOENT;
//        } else {
//            for (int i = 0; path[i]; i++) {
//                strcpy(cmd, path[i]);
//                strcat(cmd, "/");
//                strcat(cmd, command->command);
//                printf("Trying: %s\n", cmd);
//                command->argv[0] = cmd;
//                if (execv(cmd, command->argv) != -1) {
//                    break;
//                }
//                if (errno != ENOENT) {
//                    perror("execv");
//                    break;
//                }
//            }
//        }
//    }
//    return 0;
//}

static int handle_run_error(struct dc_error *err, struct command *command)
{
    if (errno == ENOENT)
    {
        dc_error_is_errno(err, 127);
        return 127;
    }
    else if (errno == EACCES)
    {
        dc_error_is_errno(err, 2);
        return 2;
    }
    else if(command->exit_code == 126)
    {
        dc_error_is_errno(err, 126);
        return 126;
    }
    else if(command->exit_code == 127)
    {
        dc_error_is_errno(err, 127);
        return 127;
    }
    else
    {
        dc_error_is_errno(err, 1);
        return 1;
    }
    {
        dc_error_is_errno(err, 1);
        return 1;
    }
}

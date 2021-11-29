/*
 * run_command.c :    do the fork, exec stuff, call other functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"

void run_command(char **myArgv) {
    pid_t pid = 0;
    int stat = 0;
    int ret = 0;
    int i = 0;

    /* Create a new child process.*/
    pid = fork();

    switch (pid) {

        /* Error. */
        case -1 :
            perror("fork");
            exit(errno);

        /* Parent. */
        default :
            if (!is_background(myArgv)) {
                /* Wait for child to terminate.*/
                wait(&stat);

                /* Optional: display exit status.  (See wstat(5).)*/
                printf("child process pid = %d, status = %d\n", pid, stat);
            }

            return;

        /* Child. */
        case 0 :
            /*Check have "&" or not, and turn "&" to NULL*/
            if (is_background(myArgv)) {
                while (myArgv[i]) {
                    i++;
                }
                if (strcmp(myArgv[i - 1], "&") == 0) {
                    myArgv[i - 1] = NULL;
                }
            }

            /* Run command in child process.*/
            ret = execvp(myArgv[0], myArgv);

            fflush(stdout);
            /* Handle error return from exec */
			exit(ret);
    }
}

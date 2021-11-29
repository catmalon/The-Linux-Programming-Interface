/* 
 * pipe_command.c  :  deal with pipes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "shell.h"

#define STD_OUTPUT 1
#define STD_INPUT  0

void pipe_and_exec(char **myArgv) {
  	int pipe_argv_index = pipe_present(myArgv);
  	int pipefds[2];
	char **left_argv = NULL;
	char **right_argv = NULL;
	pid_t pid = 0;
	
	if (pipe_argv_index == -1) {
		/* Pipe at beginning or at end of argv;  See pipe_present(). */
		fputs ("Missing command next to pipe in commandline.\n", stderr);
		errno = EINVAL;	/* Note this is NOT shell exit. */
	} else if (pipe_argv_index == 0) {
		/* No pipe found in argv array or at end of argv array.
		 * See pipe_present().  Exec with whole given argv array. */
		if (execvp(myArgv[0], myArgv) == -1) {
			perror("execvp");
			exit(errno);
		}
	} else {
		/* Pipe in the middle of argv array.  See pipe_present(). */
		/* Split arg vector into two where the pipe symbol was found.
		 * Terminate first half of vector. */
		left_argv = myArgv;
		right_argv = &myArgv[pipe_argv_index + 1];
		myArgv[pipe_argv_index] = NULL;

		/* Create a pipe to bridge the left and right halves of the vector. */
		if (pipe(pipefds) < 0) {
			perror("pipe");
			exit(errno);
		}
		
		/* Create a new process for the right side of the pipe.
			* (The left side is the running "parent".) */
		if ((pid = fork()) < 0) {
			perror("fork");
			exit(errno);
		} 

		/* Talking parent.  Remember this is a child forked from shell. */
		if (pid == 0) {
			/* - Redirect output of "parent" through the pipe.
			* - Don't need read side of pipe open.  Write side dup'ed to stdout.
			* - Exec the left command. */
			close(pipefds[0]); /* close read */
			dup2(pipefds[1], STDOUT_FILENO);
			execvp(left_argv[0], left_argv);
		}

		/* Listening child. */
		if (pid > 0) {
			/* - Redirect input of "child" through pipe.
			* - Don't need write side of pipe. Read side dup'ed to stdin.
			* - Exec command on right side of pipe and recursively deal with other pipes */
			close(pipefds[1]); /*close write*/
			dup2(pipefds[0], STDIN_FILENO);
			pipe_and_exec(&myArgv[pipe_argv_index+1]);
		}
	}
}

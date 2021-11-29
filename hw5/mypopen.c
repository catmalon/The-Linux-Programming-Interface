#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "mypopen.h"

static char** getarg(const char *command) {
	char buffer[BUFSIZ] = {'\0'};
	const char *delim = " \t\n";
  	int count = 0;
  	char *token = NULL;
  	char **newArgv = NULL;

  	if (command == NULL) {
    	return NULL;
  	}

	strcpy(buffer, command);
	if ((token = strtok(buffer, " ")) == NULL) {
		return NULL;
	}
	newArgv = (char**) realloc(newArgv, sizeof(char*) * ++count);
	newArgv[0] = token;

	while (token = strtok(NULL, delim)) {
		newArgv = (char**) realloc(newArgv, sizeof(char*) * ++count);
		newArgv[count - 1] = token;
	}

	newArgv = (char**) realloc(newArgv, sizeof(char*) * ++count);
	newArgv[count - 1] = NULL;
  	return newArgv;
}


FILE* mypopen(const char *command, const char *open_mode) 
{
	FILE *ret = NULL;
	pid_t pid = -1;
	int pipefd[2] = {0};
	int fd_mode = 0;
	char **arg = NULL;

	arg = getarg(command);
	if (arg == NULL) {
		perror("getarg error");
		exit(errno);
	}

	if (pipe(pipefd) < 0) {
		perror("pipe");
		exit(errno);
	}

	switch (pid = fork())
	{
    case -1:
		perror("fork");
		exit(errno);
    case 0:
		if (strcmp(open_mode, "r") == 0) {
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			if (execvp(arg[0], arg) == -1) {
				perror("execvp");
				exit(errno);
			}
		} else if (strcmp(open_mode, "w") == 0) {
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			if (execvp(arg[0], arg) == -1) {
				perror("execvp");
				exit(errno);
			}
		}
	default:
		break;
	}

	if (strcmp(open_mode, "r") == 0) {
		close(pipefd[1]);
		ret = fdopen(pipefd[0], open_mode);
		chile_pid[pipefd[0]] = pid;
		printf("fdopen_ret = %p, pipefd = %d\n", ret, pipefd[0]);

	}  else if (strcmp(open_mode, "w") == 0) {
		close(pipefd[0]);
		ret = fdopen(pipefd[1], open_mode);
		chile_pid[pipefd[1]] = pid;
	}
	
	return ret;
}

int mypclose (FILE *stream_to_close)
{
	int fd = 0;
	int status = 0;

	if ((fd = fileno(stream_to_close)) == 0) {
		return -1;
	}
	
	return (waitpid(chile_pid[fd], &status, 0) == -1) ? -1 : 0;
}
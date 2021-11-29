#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>


int main () {
    int pipein[2], pipeout[2];
    pid_t pid = 0;
	char buff[1024];
	char *s = buff;
	int status;
	pid_t ret;

	while (1) {
		// open 2 pipe for communication
		if (pipe(pipein) < 0) {
			perror("pipein");
			exit(errno);
		}
		if (pipe(pipeout) < 0) {
			perror("pipeout");
			exit(errno);
		}

		// fork
		switch (pid = fork()) {
			// error
			case -1:
				perror("fork");
				exit(errno);
			
			// child
			case 0:
				// read stdin from parent by pipein
				close(pipein[1]);
				read(pipein[0], buff, sizeof(buff));
				while (*s) {
					*s = toupper((unsigned char) *s);
					s++;
				}
				// write to parent by pipeout
				close(pipeout[0]);
				write(pipeout[1], buff, sizeof(buff));
				return 0;
				break;

			default:
				break;
		}

		close(pipein[0]);
		printf("Please enter the word you want to change to uppercase:\n");
		scanf("%s", buff);
		write(pipein[1], buff, sizeof(buff));

		if (waitpid(-1, &status, 0) < 0) {
			perror("wait error");
			exit(errno);
		}

		close(pipeout[1]);
		read(pipeout[0], buff, sizeof(buff));
		printf("The result:%s\n", buff);
	}

	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main() 
{
	pid_t pid = 0;
	int pfd[2];
	char buff[1024];

	/* create pipe */
	if (pipe(pfd) < 0) {
		perror("pipe");
		exit(errno);
	}

	/* fork */
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(errno);
	}

	/* child process. exec "ls" write to pipe */
	if (pid == 0) {
		/* close pipe read */
		close(pfd[0]);
		/* change pipe as stdout */
		dup2(pfd[1], STDOUT_FILENO); 
		close(pfd[1]);

		execlp("ls", "ls", "-l", NULL);
		exit(0);
	}

	/* parent process. read pipe and print */
	/* close pipe write */
	close(pfd[1]); 
	read(pfd[0], buff, sizeof(buff));
	printf("%s\n", buff);

	close(pfd[0]);
	return 0;
}
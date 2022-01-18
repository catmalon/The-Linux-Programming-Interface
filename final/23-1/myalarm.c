#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

static void handler(int sig, siginfo_t *si, void *uc)
{
	printf("Got SIGALRM signal %d\n", sig);
}

int main(int argc, char *argv[]) 
{
	struct itimerval val;
	struct sigaction sa;

	if (argc != 2 || strcmp(argv[1], "--help")== 0) {
		fprintf(stderr,"Usage : %s seconds \n",argv[0]);
		exit(EXIT_FAILURE);
	}

	val.it_value.tv_sec = atoi(argv[1]);

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGALRM, &sa, NULL) == -1) {
		perror("sigaction");
		exit(errno);
	}
	
	setitimer(ITIMER_REAL, &val, NULL);

	printf("Pause %ld second\n", val.it_value.tv_sec);
	pause();
	printf("Times up (%ld second)\n", val.it_value.tv_sec);
	
	exit(EXIT_SUCCESS);
}
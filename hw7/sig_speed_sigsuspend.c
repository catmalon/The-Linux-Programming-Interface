/* sig_speed_sigsuspend.c

   This program times how fast signals are sent and received.

   The program forks to create a parent and a child process that alternately
   send signals to each other (the child starts first). Each process catches
   the signal with a handler, and waits for signals to arrive using
   sigsuspend().

   Usage: $ time ./sig_speed_sigsuspend num-sigs

   The 'num-sigs' argument specifies how many times the parent and
   child send signals to each other.

   Child                                  Parent

   for (s = 0; s < numSigs; s++) {        for (s = 0; s < numSigs; s++) {
       send signal to parent                  wait for signal from child
       wait for a signal from parent          send a signal to child
   }                                      }
*/
#include <sys/types.h> /* Type definitions used by many programs */
#include<sys/wait.h>
#include <stdio.h>     /* Standard I/O functions */
#include <stdlib.h>    /* Prototypes of commonly used library functions,
                           plus EXIT_SUCCESS and EXIT_FAILURE constants */
#include <unistd.h>    /* Prototypes for many system calls */
#include <errno.h>     /* Declares errno and defines error constants */
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#define TESTSIG SIGUSR1
#define PRINT_PER_TIMES 100000

int scnt = 0;

static void handler(int sig, siginfo_t *info, void *context)
{
	if ((scnt % PRINT_PER_TIMES) == 0) {
		printf("\tCaught signal %d (%s) from pid: %d\n", sig, strsignal(sig), info->si_pid);
	}
}

int main(int argc, char *argv[])
{
	int numSigs = 0;
	int status = 0;
	struct sigaction sa;
	sigset_t blockedMask, emptyMask;
	pid_t childPid;
	clock_t begin, end;
	double cost;
	
	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fflush(stdout);           /* Flush any pending stdout */
		fprintf(stderr, "Usage: %s num-sigs\n", argv[0]);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	numSigs = atoi(argv[1]);
	
	printf("Start to exchange signals for %d time.\n", numSigs);
	begin = clock();

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	if (sigaction(TESTSIG, &sa, NULL) == -1) {
		perror("sigaction");
		exit(errno);
	}

	/* Block the signal before fork(), so that the child doesn't manage
		to send it to the parent before the parent is ready to catch it */

	sigemptyset(&blockedMask);
	sigaddset(&blockedMask, TESTSIG);
	if (sigprocmask(SIG_SETMASK, &blockedMask, NULL) == -1) {
		perror("sigprocmask");
		exit(errno);
	}

	sigemptyset(&emptyMask);

	switch (childPid = fork()) {
	case -1: 
		perror("fork");
		exit(errno);

	case 0:     /* child */
		for (scnt = 0; scnt < numSigs; scnt++) {
			if (kill(getppid(), TESTSIG) == -1) {
				perror("kill");
				exit(errno);
			}
			if ((scnt % PRINT_PER_TIMES) == 0) {
				printf("SIG Number %d from child (%d)\n", scnt+1, getpid());
			}
			if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
				perror("sigsuspend");
				exit(errno);
			}
		}
		exit(EXIT_SUCCESS);

	default: 
		break;
	}

	/* parent */
	for (scnt = 0; scnt < numSigs; scnt++) {
		if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
			perror("sigsuspend");
			exit(errno);
		}
		if (kill(childPid, TESTSIG) == -1) {
			perror("kill");
			exit(errno);
		}
		if ((scnt % PRINT_PER_TIMES) == 0) {
			printf("SIG Number %d from parent (%d)\n", scnt+1, getpid());
		}
	}
	wait(&status);
	end = clock();
	cost = (double)(end - begin)/CLOCKS_PER_SEC;
	printf("Time cost of exchanging signals is: %lf secs\n", cost);

	exit(EXIT_SUCCESS);
}
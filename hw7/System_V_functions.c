#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

typedef void (*sighandler_t)(int);
#define SIG_HOLD ((sighandler_t)5)
// #define SIG_IGN ((sighandler_t)1)


// The sighold() function adds a signal to the process signal mask.
int sighold(int sig)
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, sig);

	printf("The signal %d (%s) has been added.\n", sig, strsignal(sig));
	return sigprocmask(SIG_BLOCK, &set, NULL);
}

// The sigignore() function sets a signal’s disposition to ignore.
int sigignore(int sig)
{
	if (signal(sig, SIG_IGN) == SIG_ERR) {
		return -1;
	}
	printf("The signal %d (%s) has been ignored.\n", sig, strsignal(sig));
	return 0;
}

// the sigset() call (with a prototype similar to that of signal()). 
// As with signal(), the handler argument for sigset() can be specified
// as SIG_IGN, SIG_DFL, or the address of a signal handler. 
// Alternatively, it can be specified as SIG_HOLD, 
// to add the signal to the process signal mask while leaving the 
// disposition of the signal unchanged.

sighandler_t sigset(int sig, sighandler_t disposition)
{
	struct sigaction old_set;

	if (disposition == SIG_HOLD) {
		sighold(sig);
		sigaction(sig, NULL, &old_set);
		return old_set.sa_handler;
	}
	if (disposition == SIG_IGN) {
		sigignore(sig);
		sigaction(sig, NULL, &old_set);
		return old_set.sa_handler;
	}
	if (disposition == SIG_DFL) {
		signal(sig, SIG_DFL);
		sigaction(sig, NULL, &old_set);
		printf("The signal %d (%s) is restored to default.\n", sig, strsignal(sig));
		return old_set.sa_handler;
	}
	
	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = disposition;
	sigaction(sig, &act, &old_set);
	printf("The handler of signal %d (%s) has been change.\n", sig, strsignal(sig));

	return old_set.sa_handler;
}

// The sigrelse() function removes a signal from the signal mask.
int sigrelse(int sig)
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, sig);

	printf("The signal %d (%s) has been deleted.\n", sig, strsignal(sig));
	return sigprocmask(SIG_UNBLOCK, &set, NULL);
}

// The sigpause() function is similar to sigsuspend(), 
// but removes just one signal from the process signal mask 
// before suspending the process until the arrival of a signal.
int sigpause(int sig)
{
	sigset_t old_set;
	if (sigprocmask(SIG_BLOCK, NULL, &old_set) == -1) {
		return -1;
	}
	sigdelset(&old_set, sig);
	printf("The signal %d (%s) has been paused.\n", sig, strsignal(sig));
	return sigsuspend(&old_set);
}

void handler(int sig)
{
    printf("The handler has been called.\n"); // it's unsafe
}

int main()
{
	sigset(SIGTERM, SIG_IGN);
	sigset(SIGTERM, SIG_DFL);
	sigset(SIGTERM, handler);
	sighold(SIGABRT);
	sigrelse(SIGABRT);
	sigignore(SIGABRT);
	sighold(SIGTERM);
	raise(SIGTERM);
	sigpause(SIGTERM);

    return 0;
}
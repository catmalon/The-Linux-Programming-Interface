#include "svmsg_file.h"
int serverId;
int seqNum;

static void /* SIGCHLD handler */
grimReaper(int sig)
{
	int savedErrno;

	savedErrno = errno; /* waitpid() might change 'errno' */
	while (waitpid(-1, NULL, WNOHANG) > 0)
		continue;
	errno = savedErrno;
}

static void /* Executed in child process: serve a single client */
serveRequest(const struct requestMsg *req)
{
	struct responseMsg resp;

	resp.mtype = req->ClientId;
	resp.seqNum = seqNum;
	
	msgsnd(serverId, &resp, sizeof(struct responseMsg) - sizeof(long), 0);
}

void
removeQueue(int sig)
{
	if (msgctl(serverId, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(errno);
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	struct requestMsg req;
	pid_t pid;
	ssize_t msgLen;
	struct sigaction sa;

	/* Create server message queue */
	serverId = msgget(SERVER_KEY, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR | S_IRGRP |S_IWGRP);
	if (serverId == -1) {
		perror("msgget");
		exit(errno);
	}
	printf("Server msget success.\n");

	/* Establish SIGCHLD handler to reap terminated children */

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = grimReaper;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(errno);
	}
	if (signal(SIGINT, removeQueue) == SIG_ERR) {
		perror("signal-SIGINT");
		exit(errno);

	}
	if (signal(SIGQUIT, removeQueue) == SIG_ERR) {
		perror("signal-SIGQUIT");
		exit(errno);
	}
	if (signal(SIGTERM, removeQueue) == SIG_ERR) {
		perror("signal-SIGQUIT");
		exit(errno);
	}
	


	/* Read requests, handle each in a separate child process */

	while(1) 
	{
		msgLen = msgrcv(serverId, &req, sizeof(struct requestMsg) - sizeof(long), 1, 0);
		if (msgLen == -1) {
			if (errno == EINTR) { /* Interrupted by SIGCHLD handler? */
				continue;       /* ... then restart msgrcv() */
			}
			perror("msgrcv"); /* Some other error */
			break;              /* ... so terminate loop */
		}
		printf("Received Client(%d) message, sequence lenght = %d.\n", req.ClientId, req.seqLen);

		pid = fork(); /* Create child process */
		if (pid == -1) {
			perror("fork");
			break;
		}

		if (pid == 0) { /* Child handles request */
			serveRequest(&req);
			exit(EXIT_SUCCESS);
		}
		/* Parent loops to receive next client request */
		seqNum += req.seqLen;
		printf("Now sequence num = %d\n", seqNum);
	}

	/* If msgrcv() or fork() fails, remove server MQ and exit */

	if (msgctl(serverId, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(errno);
	}
	exit(EXIT_SUCCESS);
}
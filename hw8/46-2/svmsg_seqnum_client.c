#include "svmsg_file.h"


int
main(int argc, char *argv[])
{
	struct requestMsg req;
	struct responseMsg resp;
	int serverId, numMsgs;
	int clientPid;
	ssize_t msgLen, totBytes;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		fprintf(stderr,"Usage : %s [seq-len]\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Get server's queue identifier */
	serverId = msgget(SERVER_KEY, S_IRUSR | S_IWUSR);
	if (serverId == -1) {
		perror("msgget - server message queue");
		exit(errno);
	}

	/* Send message */
	clientPid = getpid();
	req.mtype = 1; 					/* To Server */
	req.ClientId = clientPid;
	req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;

	if (msgsnd(serverId, &req, sizeof(struct requestMsg) - sizeof(long), 0) == -1) {
		perror("msgsnd");
		exit(errno);
	}

	/* Get response with mtype == pid */
	msgLen = msgrcv(serverId, &resp, sizeof(struct responseMsg) - sizeof(long), clientPid, 0);
	if (msgLen == -1) {
		perror("msgrcv");
		exit(errno);
	}
	printf("\tReceived seqNum = %d from server.\n", resp.seqNum);

	exit(EXIT_SUCCESS);
}
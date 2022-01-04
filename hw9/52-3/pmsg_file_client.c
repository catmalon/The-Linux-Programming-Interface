#include "pmsg_file.h"


int
main(int argc, char *argv[])
{
	struct requestMsg req;
	struct responseMsg resp;
	mqd_t sqd, cqd;
	int sendPrio;
    int  numMsgs;
	unsigned int prio;
    ssize_t msgLen, totBytes;
	char clientMQ[CLIENT_MQ_NAME_LEN];

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr,"Usage: %s pathname [priority order]\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strlen(argv[1]) > sizeof(req.pathname) - 1) {
		fprintf(stderr,"pathname too long (max: %ld bytes)\n", (long) sizeof(req.pathname) - 1);
		exit(EXIT_FAILURE);
	}
	/*FIXME: above: should use %zu here, and remove (long) cast */

	sendPrio = (argc == 3)? atoi(argv[2]) : 0;

	snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TMP, (long) getpid());

	// Creat and Open Client MQ
	cqd = mq_open(clientMQ, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH  | S_IWOTH, NULL);
	if (cqd == (mqd_t) -1) {
		perror("Client mq_open");
		exit(errno);
	}

	// Open Server MQ
	sqd = mq_open(SERVER_MQ, O_RDWR);
	if (cqd == (mqd_t) -1) {
		perror("server mq_open");
		exit(errno);
	}

	/* Send message asking for file named in argv[1] */
	req.clientId = getpid();
	strncpy(req.pathname, argv[1], sizeof(req.pathname) - 1);
	req.pathname[sizeof(req.pathname) - 1] = '\0'; /* Ensure string is terminated */

	if (mq_send(sqd, (char*) &req, sizeof(req), sendPrio) == -1) {
		perror("mq_send");
		exit(errno);
	}

	/* Get first response, which may be failure notification */
	msgLen = mq_receive(cqd, (char*) &resp, RESP_MSG_SIZE, &prio);
	if (msgLen == -1) {
		perror("mq_receive");
		exit(errno);
	}

	if (resp.type == RESP_MT_FAILURE) {
		printf("%s\n", resp.data);      /* Display msg from server */
		exit(EXIT_FAILURE);
	}

	/* File was opened successfully by server; process messages
		(including the one already received) containing file data */
	totBytes = msgLen - sizeof(resp.type);                  /* Count first message */
	for (numMsgs = 1; resp.type == RESP_MT_DATA; numMsgs++) {
		msgLen = mq_receive(cqd, (char*) &resp, RESP_MSG_SIZE, &prio);
		if (resp.type == RESP_MT_DATA) {
			if (msgLen == -1) {
			perror("mq_receive");
			exit(errno);
			}
		}
		totBytes += msgLen - sizeof(resp.type);
	}

	printf("\tReceived %ld bytes (%d messages)\n", len, numMsgs);
	/*FIXME: above: should use %zd here, and remove (long) cast (or perhaps
		better, make totBytes size_t, and use %zu)*/

	if (mq_unlink(clientMQ) == -1) {
		perror("mq_unlink");
		exit(errno);
	}
	exit(EXIT_SUCCESS);
}
#include "pmsg_file.h"

static void             /* SIGCHLD handler */
grimReaper(int sig)
{
    int savedErrno;
    savedErrno = errno;                 /* waitpid() might change 'errno' */
    while (waitpid(-1, NULL, WNOHANG) > 0) {
		continue;
	}
    errno = savedErrno;
}

static void /* Executed in child process: serve a single client */
serveRequest(const void *buffer)
{
	int fd;
	mqd_t cqd;
	ssize_t numRead;
	struct requestMsg *req;
	struct responseMsg resp;
	char clientMQ[RESP_MSG_SIZE];

	req = (struct requestMsg *) buffer;
	printf("Received request from client: %d\n", req->clientId);
	printf("PATHNAME: %s\n", req->pathname);

	snprintf(clientMQ, CLIENT_MQ_NAME_LEN, CLIENT_MQ_TMP, (long)req->clientId);
	cqd = mq_open(clientMQ, O_RDWR);
	if (cqd == (mqd_t) -1) {
		perror("client mq_open");
		exit(errno);
	}

	fd = open(req->pathname, O_RDONLY);
	if (fd == -1) { /* Open failed: send error text */
		resp.type = RESP_MT_FAILURE;
		snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
		if (mq_send(cqd, (char *) &resp, sizeof(resp), 0) == -1) {
			perror("mq_send");
			exit(errno);
		}
    }

	/* Transmit file contents in messages with type RESP_MT_DATA. We don't
		diagnose read() and msgsnd() errors since we can't notify client. */
	resp.type = RESP_MT_DATA;
	while ((numRead = read(fd, resp.data, RESP_MSG_SIZE-((int)sizeof(resp.type)))) > 0) {
		if (mq_send(cqd, (char *) &resp, sizeof(resp.type) + numRead, 0) == -1) {
			break;
		}
	}
	resp.type = RESP_MT_END;
	mq_send(cqd,(char *) &resp, sizeof(resp.type), 0);
}


int main(int argc, char *argv[])
{
	pid_t pid;
	mqd_t sqd;
	unsigned int prio;
	void *buffer;
	struct mq_attr attr;
	ssize_t msgLen;
	struct requestMsg req;
	struct sigaction sa;
	
	sqd = mq_open(SERVER_MQ, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH  | S_IWOTH, NULL);
	if (sqd == (mqd_t) -1) {
		perror("Server mq_open");
		exit(errno);
	}

    if (mq_getattr(sqd, &attr) == -1) {
        perror("mq_getattr");
		exit(errno);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
		exit(errno);
    }

	/* Establish SIGCHLD handler to reap terminated children */

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(errno);
	}

	while(1) {
		// memset(buffer, NULL, sizeof(buffer));
		msgLen = mq_receive(sqd, buffer, attr.mq_msgsize, &prio);
		if (msgLen == -1) {
			if (errno == EINTR) {
				continue;
			}
			perror("mq_receive");
			break;
		}
		printf("Receive %ld bytes; priority = %u\n", (long) msgLen, prio);


		/*FIXME: above: should use %zd here, and remove (long) cast */

		pid = fork(); /* Create child process */
		if (pid == -1) {
			perror("fork");
			break;
		}
		if (pid == 0) { /* Child handles request */
			serveRequest(buffer);
			exit(EXIT_SUCCESS);
		}
		/* Parent loops to receive next client request */
	}
    exit(EXIT_SUCCESS);
}
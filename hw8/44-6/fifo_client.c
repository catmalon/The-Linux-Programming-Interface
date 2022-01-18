#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "fifo_seqnum.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];
static void             /* Invoked on exit to delete client FIFO */
removeFifo(void)
{
    unlink(clientFifo);
}
int
main(int argc, char *argv[])
{
    int serverFd, clientFd;
    struct request req;
    struct response resp;

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,"Usage : %s [seq-len]\n",argv[0]);
		exit(EXIT_FAILURE);
    }

    /* Create our FIFO (before sending request, to avoid a race) */

    umask(0);                   /* So we get the permissions we want */
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,
            (long) getpid());
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(errno);
    }

    if (atexit(removeFifo) != 0) {
        perror("atexit");
        exit(errno);
    }
    /* Construct request message, open server FIFO, and send message */

    req.pid = getpid();

    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;

    serverFd = open(SERVER_FIFO, O_WRONLY);
    if (serverFd == -1) {
        perror("open");
        exit(errno);
    }

    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
        printf("Can't write to server\n");
    }
    printf("\tpID %d send the request (length = %d) to server.\n", req.pid, req.seqLen);

    /* Open our FIFO, read and display response */

    clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1) {
        perror("open");
        exit(errno);
    }

    if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
        printf("Can't read response from server\n");
    }

    printf("\tClient %d receive the response from server.\n", getpid());
    printf("\tresp.seqNum = %d\n", resp.seqNum);
    exit(EXIT_SUCCESS);
}
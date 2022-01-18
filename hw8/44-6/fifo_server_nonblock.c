#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#include "fifo_seqnum.h"
int
main(int argc, char *argv[])
{
    int serverFd, dummyFd, clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    int seqNum = 0;                     /* This is our "service" */

    /* Create well-known FIFO, and open it for reading */

    umask(0);                           /* So we get the permissions we want */
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(errno);
    }
    serverFd = open(SERVER_FIFO, O_RDONLY);
    if (serverFd == -1) {
        perror("open");
        exit(errno);
    }

    /* Open an extra write descriptor, so that we never see EOF */

    dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1) {
        perror("open");
        exit(errno);
    }

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
        perror("signal");
        exit(errno);
    }

    while(1) {                          /* Read requests and send responses */
        // printf("Read for new request!\n");
        if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;                   /* Either partial read or error */
        }

        /* Open client FIFO (previously created by client) */

        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);

        /* Try to open the client fifo for 20000 times, or give up this client */
        for (int i = 0; i < 20000; i++) {
            if ((clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK)) == -1 && errno == ENXIO) {
                continue;
            }
            printf("Server opened the Client(%d) request.\n", req.pid);
            break;
        }

    
        /* Send response and close FIFO */
        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response)) {
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        }
        printf("The server(%d) response the client(%d).\n", getpid(), req.pid);

        if (close(clientFd) == -1) {
            perror("close");
            exit(errno);
        }
        seqNum += req.seqLen;           /* Update our sequence number */
        printf("seqNum = %d\n", seqNum);
    }
    return(EXIT_SUCCESS);
}
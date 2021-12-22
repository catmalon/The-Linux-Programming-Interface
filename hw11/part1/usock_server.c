/*
 * usock_server : listen on a Unix socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "dict.h"

int main(int argc, char **argv) {
	struct sockaddr_un server;
	int sd,cd,n;
	pid_t pid;
	ssize_t numRead;
	Dictrec tryit;
	char buf[BUF_SIZE];

	if (argc != 3) {
		fprintf(stderr,"Usage : %s <dictionary source>"
			"<Socket name>\n",argv[0]);
		exit(errno);
	}

	/* Setup socket. */
	sd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket");
		exit(errno);
	}

	/* Initialize address. */
	if (remove(argv[2]) == -1 && errno != ENOENT) {
		perror("remove");
		exit(errno);
	}
	memset(&server, 0, sizeof(struct sockaddr_un));
	server.sun_family = AF_UNIX;
	strncpy(server.sun_path, argv[2], sizeof(server.sun_path) - 1);
	if (bind(sd, (struct sockaddr *) &server, sizeof(server)) == -1) {
		perror("bind");
		exit(errno);
	}

	/* Name and activate the socket. */
	if (listen(sd, BACKLOG) == -1) {
		perror("listen");
		exit(errno);
	}

	/* main loop : accept connection; fork a child to have dialogue */
	for (;;) {
		/* Wait for a connection. */
		cd = accept(sd, NULL, NULL);
		if (cd == -1) {
			perror("accept");
			exit(errno);
		}

		/* Handle new client in a subprocess. */
		pid = fork();
		if (pid == -1) {
			DIE("fork");
		} else if (pid == 0) {
			close (sd);	/* Rendezvous socket is for parent only. */
			/* Get next request. */
			memset(&tryit, 0, sizeof(tryit));
			// printf("read %s\n", tryit.word);
			while ((numRead = read(cd, &tryit, sizeof(tryit))) > 0) {
				// printf("read %s\n", tryit.word);
				/* Lookup request. */
				switch(lookup(&tryit,argv[1])) {
					/* Write response back to client. */
					case FOUND: 
						if (write(cd, &tryit.text, sizeof(tryit.text)) <= 0 ){
							perror("server write");
							exit(errno);
						}
						// printf("Server write %s\n", tryit.text);
						break;
					case NOTFOUND: 
						strcpy(tryit.text, "Not Found!");
						if (write(cd, &tryit.text, sizeof(tryit.text)) <= 0 ){
							perror("server write");
							exit(errno);
						}
						break;
					case UNAVAIL:
						DIE(argv[1]);
				} /* end lookup switch */
			} /* end of client dialog */
			/* Terminate child process.  It is done. */
			exit(0);
		} else {
			close(cd);
		}
    } /* end forever loop */
} /* end main */

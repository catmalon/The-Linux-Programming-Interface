/*
 * isock_server : listen on an internet socket ; fork ;
 *                child does lookup ; replies down same socket
 * argv[1] is the name of the local datafile
 * PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#include "dict.h"

int main(int argc, char **argv) {
	static struct sockaddr_in server;
	int sd,cd,n;
	Dictrec tryit;
	pid_t pid;
	ssize_t numRead;

	if (argc != 2) {
		fprintf(stderr,"Usage : %s <datafile>\n",argv[0]);
		exit(1);
	}

	/* Create the socket. */
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		perror("socket");
		exit(errno);
	}

	/* Initialize address. */
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	/* Name and activate the socket. */
	if (bind(sd, (struct sockaddr *) &server, sizeof(server)) == -1) {
		perror("bind");
		exit(errno);
	}
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
			while ((numRead = recv(cd, &tryit, sizeof(tryit), 0)) > 0) {
				// printf("recv %s\n", tryit.word);
				/* Lookup request. */
				switch(lookup(&tryit,argv[1])) {
					/* Write response back to client. */
					case FOUND: 
						if (send(cd, &tryit.text, TEXT, 0) <= 0 ){
							perror("server write");
							exit(errno);
						}
						// printf("Server write %s\n", tryit.text);
						break;
					case NOTFOUND: 
						strcpy(tryit.text, "Not Found!");
						if (send(cd, &tryit.text, TEXT, 0) <= 0 ){
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
		} else {    /* Parent */
			close(cd);
		}
    } /* end forever loop */
} /* end main */
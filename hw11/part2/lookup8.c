/*
 * lookup8 : does no looking up locally, but instead asks
 * a server for the answer. Communication is by Internet TCP Sockets
 * The name of the server is passed as resource. PORT is defined in dict.h
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include "dict.h"

int lookup(Dictrec * sought, const char * resource) {
	static int sockfd;
	static struct sockaddr_in server;
	struct hostent *host;
	static int first_time = 1;

	if (first_time) {        /* connect to socket ; resource is server name */
		first_time = 0;

		/* Set up destination address. */
		
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1) {
			perror("socket client");
			exit(errno);
		}

		/* Allocate socket. */
		memset(&server, 0, sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		host = gethostbyname(resource);
		server.sin_port = htons(PORT);
		memcpy((char*)&server.sin_addr, host->h_addr_list[0], host->h_length);

		/* Connect to the server. */
		if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
			perror("connect server");
			exit(errno);
		}

	}

	/* write query on socket ; await reply */
	send(sockfd, &sought->word, WORD, 0);
	recv(sockfd, &sought->text, TEXT, 0);

	if (strcmp(sought->text, "Not Found!") != 0) {
		return FOUND;
	}

	return NOTFOUND;
}

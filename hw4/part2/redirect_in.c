/*
 * redirect_in.c  :  check for <
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"

/* Look for "<" in myArgv, then redirect input to the file.
 * Returns 0 on success, sets errno and returns -1 on error. */
int redirect_in(char ** myArgv) {
  	int i = 0;
  	int fd = 0;
	int inputFlags;

  	/* search forward for < */
	while (myArgv[i]) {
		if (strcmp(myArgv[i], "<") == 0) {
			break;
		}
		i++;
	}

  	if (myArgv[i]) {	/* found "<" in vector. */

    	/* 1) Open file.
     	 * 2) Redirect stdin to use file for input.
   		 * 3) Cleanup / close unneeded file descriptors.
   		 * 4) Remove the "<" and the filename from myArgv. */

		inputFlags = O_RDONLY;
		if ((fd = open(myArgv[++i], inputFlags)) < 0) {
			perror("open");
			exit(errno);
		}

		dup2(fd, STDIN_FILENO);
		close(fd);

		while (myArgv[++i]) {
			myArgv[i-2] = myArgv[i];
		}
		myArgv[i-2] = NULL;
  	}
  	return 0;
}

/*
 * redirect_out.c   :   check for >
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"

/*
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error. */
int redirect_out(char ** myArgv) {
	int i = 0;
  	int fd = 0;
	int outputFlags;
	mode_t outputMode;

  	/* search forward for > */
	while (myArgv[i]) {
		if (strcmp(myArgv[i], ">") == 0) {
			break;
		}
		i++;
	}

  	if (myArgv[i]) {	/* found ">" in vector. */

    	/* 1) Open file.
    	 * 2) Redirect to use it for output.
    	 * 3) Cleanup / close unneeded file descriptors.
    	 * 4) Remove the ">" and the filename from myArgv. */
		outputFlags = O_WRONLY | O_CREAT | O_TRUNC;
		outputMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

		if ((fd = open(myArgv[++i], outputFlags, outputMode)) < 0) {
			perror("open");
			exit(errno);
		}

		dup2(fd, STDOUT_FILENO);
		close(fd);

		while (myArgv[++i]) {
			myArgv[i-2] = myArgv[i];
		}
		myArgv[i-2] = NULL;
  	}

  	return 0;
}

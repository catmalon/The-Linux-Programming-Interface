/*
 *  pipe_present.c :  check for |
 */

#include <stdio.h>
#include <string.h>
#include "shell.h"

/*
 * Return index offset into argv of where "|" is,
 * -1 if in an illegal position (first or last index in the array),
 * or 0 if not present. */
int pipe_present(char ** myCurrentArgv) {
	int ret = 0;
	int index = 0;

  	/* Search through myCurrentArgv for a match on "|". */
	while (myCurrentArgv[index]){
		if (strcmp(myCurrentArgv[index], "|") == 0) {
			ret = (index == 0 || myCurrentArgv[index+1] == NULL) ? -1 : index;
			goto Exit;
		}
		index++;
	}
Exit:
	return ret;
}

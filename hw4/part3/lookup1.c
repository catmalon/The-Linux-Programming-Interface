/*
 * lookup1 : straight linear search through a local file
 * 	         of fixed length records. The file name is passed
 *	         as resource.
 */
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "dict.h"

int lookup(Dictrec *sought, const char *resource) {
	Dictrec dr;
	static FILE *in = NULL;
	static int first_time = 1;

	if (first_time) { 
		first_time = 0;
		// open up the file
		if ((in = fopen(resource, "r")) == NULL) {
			perror(resource);
			exit(errno);
		}
	}

	// read from top of file, looking for match
	rewind(in);
	while(fread(&dr, sizeof(Dictrec), 1, in) == 1) {
		if (strncmp(sought->word, dr.word, strlen(dr.word)) == 0) {
			memcpy(sought, &dr, sizeof(Dictrec));
			return FOUND;
		}
	}

	return NOTFOUND;
}

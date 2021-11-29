/*
 * convert.c : take a file in the form 
 *  word1
 *  multiline definition of word1
 *  stretching over several lines, 
 * followed by a blank line
 * word2....etc
 * convert into a file of fixed-length records
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "dict.h"
#define BIGLINE 512

int main(int argc, char **argv) {
	FILE *in = NULL;
	FILE *out = NULL;
	char line[BIGLINE];
	static Dictrec dr, blank;
	size_t offset = 0;

	if (argc != 3) {
		printf("Usage: convert [input file] [output file].\n");
		return -1;
	}

	if ((in = fopen(argv[1], "r")) == NULL) {
		perror(argv[1]);
		exit(errno);
	}
	if ((out = fopen(argv[2], "w")) == NULL) {
		perror(argv[2]);
		exit(errno);
	}

	/* Loop through the whole file. */
	while (!feof(in)) {
		memset(&dr, 0, sizeof(Dictrec));

		// Read word and put in record.  Truncate at the end of the "word" field.
		if (fgets(line, BIGLINE, in) == NULL) {
			break;
		}
		strncpy(dr.word, line, strlen(line) - 1);

		// Read definition, line by line, and put in record.
		offset = 0;
		while (fgets(line, BIGLINE, in) != NULL) {
			if (strlen(line) == 1) {
				break;
			}

			strncpy(dr.text + offset, line, strlen(line) - 1);
			offset += strlen(line) - 1;
		}

		// Write record out to file.
		fwrite(&dr, sizeof(Dictrec), 1, out);
	}

	fclose(in);
	fclose(out);
	return 0;
}

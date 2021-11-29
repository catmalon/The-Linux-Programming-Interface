/*
 * parse.c : use whitespace to tokenise a line
 * Initialise a vector big enough
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"

/* Parse a commandline string into an argv array. */
char ** parse(char *line) {

  	static char delim[] = " \t\n"; /* SPACE or TAB or NL */
  	int count = 0;
  	char *token = NULL;
  	char **newArgv = NULL;

  	/* Nothing entered. */
  	if (line == NULL || strcmp(line,"\n")==0) {
    	return NULL;
  	}

  	/* Init strtok with commandline, then get first token.
     * Return NULL if no tokens in line.
     */
	if (!(token = strtok(line, delim))) {
		return NULL;
	}

  	/* Create array with room for first token. */
	newArgv = (char**) realloc(newArgv, sizeof(char*) * ++count);
	newArgv[0] = token;

  	/* While there are more tokens...
	 *
	 *  - Get next token.
	 *	- Resize array.
	 *  - Give token its own memory, then install it.
	 */
	while (token = strtok(NULL, delim)) {
		newArgv = (char**) realloc(newArgv, sizeof(char*) * ++count);
		newArgv[count - 1] = token;
	}


  	/* Null terminate the array and return it. */
	newArgv = (char**) realloc(newArgv, sizeof(char*) * ++count);
	newArgv[count - 1] = NULL;

  	return newArgv;
}


/*
 * Free memory associated with argv array passed in.
 * Argv array is assumed created with parse() above.
 */
void free_argv(char **oldArgv) {
	free(oldArgv);
}

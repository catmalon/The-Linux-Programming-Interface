/*
 * header file for progressive dictionary exercise
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define WORD 32
#define TEXT 480

#define FOUND 		0
#define NOTFOUND 	1
#define UNAVAIL 	2

#define BUF_SIZE 1024
#define BACKLOG 	5

/* Used in socket examples. */
#define PORT 5678

#define DIE(x) perror(x),exit(1)

/* standard lookup structure */
typedef struct {
  char word[WORD];
  char text[TEXT];
} Dictrec;

int lookup(Dictrec * ,const char *);

/* Template for the layout of shared memory */
typedef struct {
  int numrec;
  Dictrec table[1];
} Memory;

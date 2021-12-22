/*
 * lookup5 : local file ; setup a memory map of the file
 *           Use bsearch. We assume that the words are already
 *           in dictionary order in a file of fixed-size records
 *           of type Dictrec
 *           The name of the file is what is passed as resource
 */

#include <sys/mman.h>
#include <sys/stat.h>

#include "dict.h"

/*
 * This obscure looking function will be useful with bsearch
 * It compares a word with the word part of a Dictrec
 */

int dict_cmp(const void *a,const void *b) {
    return strcmp((char *)a,((Dictrec *)b)->word);
}

int lookup(Dictrec * sought, const char * resource) {
	static Dictrec * table;
	static int numrec;
	Dictrec * found = NULL;
	static int first_time = 1;
	struct stat sb = {0};
	ssize_t numRead;
	Memory* dict;
	char buf[BUF_SIZE];

	if (first_time) {  /* table ends up pointing at mmap of file */
		int fd;
		long filsiz;

		first_time = 0;

		/* Open the dictionary. */
		if ((fd = open(resource, O_RDONLY)) == -1) {
			perror("open dict");
			exit(errno);
		}
		
 		/* Get record count for building the tree. */
		filsiz = lseek(fd, 0L, SEEK_END);
		numrec = filsiz / sizeof(Dictrec);

		/* mmap the data. */
		dict = malloc(sizeof(Memory) + (numrec - 1) * sizeof(Dictrec));
		dict->numrec = numrec;
		
		lseek(fd, 0L, SEEK_SET);
		for (int i = 0; i < numrec; i++) {
			numRead = read(fd, buf, sizeof(Dictrec));
			if (numRead > 0) {
				memcpy(&(dict->table[i]), buf, numRead);
			}
		}
		if (close(fd) == -1){
			perror("close");
			exit(errno);
		}
	}

	/* search table using bsearch */

	found = bsearch(sought->word, dict->table, numrec, sizeof(Dictrec), dict_cmp);

	if (found) {
		strcpy(sought->text, found->text);
		return FOUND;
	}
	return NOTFOUND;
}
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZ 1024
#define MAX_FILE 128

void help (const char *programName, int status) {
	fprintf(stderr, "Usage: %s [-a] <file1> <file2> ... <fileN>\n", programName);
	exit(status);
}

void failure (const char *funcCall) {
	perror(funcCall);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	int opt = 0;
	int append = 0;

	int fd, flags;
	int fds[MAX_FILE];
	mode_t mode;

	char buf[BUF_SIZ] = {'\0'};
	ssize_t numRead;

	int i = 0, numFile = 0;

	// getopt
	opterr = 0;
	while ( (opt = getopt(argc, argv, ":ah")) != -1 ) {
		switch (opt) {
			case '?': 
				help(argv[0], EXIT_FAILURE);
				break;
			case 'h':
				help(argv[0], EXIT_SUCCESS);
				break;
			case 'a':
				append = 1;
				break;
		}
	}

	if (optind >= argc) {
		help(argv[0], EXIT_FAILURE);
	}

	// open flag
	flags = O_CREAT | O_WRONLY;
	mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

	if (append) {
		flags |= O_APPEND;
	} else {
		flags |= O_TRUNC;
	}

	// open output file to fds
	for (i = optind; i < argc; i++) {
		fds[i - optind] = fd = open(argv[i], flags, mode);

		if (fd == -1) {
			failure("open");
		}

		numFile++;
	}

	// read stdin and write stdout and file
	while ((numRead = read(STDIN_FILENO, buf, BUF_SIZ)) > 0) {
		if (write(STDOUT_FILENO, buf, numRead) != numRead) {
			failure("write");
		}

		for (i = 0; i < numFile; i++) {
			if (write(fds[i], buf, numRead) != numRead) {
				failure("write");
			}
		}
	}

	// read failure
	if (numRead == -1) {
		failure("read");
	}

	// close file
	for (i = 0; i < numFile; i++) {
		if(close(fds[i]) == -1) {
			failure("close");
		}
	}

	return 0;
}
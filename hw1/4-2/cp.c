#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>

#define BUF_SIZ 1024

void help(const char *programName, int status) {
	fprintf(stderr, "Usage: %s <file> <newfile>\n", programName);
	exit(status);
}

void failure(const char *funcCall) {
	perror(funcCall);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		help(argv[0], EXIT_FAILURE);
	}

	int inputFd, outputFd, inputFlags, outputFlags;
	mode_t outputMode;
	char *inputFile, *outpurFile;

	ssize_t numRead;
	char buf[BUF_SIZ];
	char zeroes[BUF_SIZ];

	inputFlags = O_RDONLY;
	outputFlags = O_WRONLY | O_CREAT | O_TRUNC;

	outputMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

	// open intput file
	if ((inputFd = open(argv[1], inputFlags)) == -1) {
		failure("open");
	}

	// open output file
	if ((outputFd = open(argv[2], outputFlags, outputMode)) == -1) {
		failure("open");
	}

	// copy intput file to output file
	while ((numRead = read(inputFd, buf, BUF_SIZ)) > 0) {
		if (numRead == -1) {
			failure("read");
		}

		if (memcmp(buf, zeroes, numRead) == 0) {
			// find hole and do not copy
			lseek(outputFd, numRead, SEEK_CUR);
		} else {
			if (write(outputFd, buf, numRead) != numRead) {
				failure("write");
			}
		}
	}

	// close file
	if (close(inputFd) == -1) {
		failure("close");
	}
	if (close(outputFd) == -1) {
		failure("close");
	}
	return 0;
}
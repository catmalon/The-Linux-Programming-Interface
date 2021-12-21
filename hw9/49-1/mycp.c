#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	char *srcaddr, *dstaddr;
	int srcfd, dstfd;
	struct stat sb, dstsb;

	if (argc != 3 || strcmp(argv[1], "--help")== 0) {
		fprintf(stderr,"Usage : %s sourceFILE destinationFILE\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	// open the source file
	if ((srcfd = open(argv[1], O_RDONLY)) == -1) {
		perror("open src");
		exit(errno);
	}
	if (fstat(srcfd, &sb) == -1) {
		perror("fstat");
		exit(errno);
	}
	if (sb.st_size == 0) {
		exit(EXIT_SUCCESS);
	}
	printf("File size = %ld\n", sb.st_size);

	// Mapping the source file to MM
	srcaddr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, srcfd, 0);
	if (srcaddr == MAP_FAILED) {
		perror("mmap src");
		exit(errno);
	}
	close(srcfd);

	// Open the destination file rw-rw-rw-
	if ((dstfd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC 
					| S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) {
		perror("open dst");
		exit(errno);
	}
	if (ftruncate(dstfd, sb.st_size) == -1) {
		perror("ftruncate");
		exit(errno);
	}
	if (fstat(dstfd, &dstsb) == -1) {
		perror("fstat");
		exit(errno);
	}
	printf("file size = %ld\n", sb.st_size);
	
	// Mapping the destination file to MM
	dstaddr = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, dstfd, 0);
	if (dstaddr == MAP_FAILED) {
		perror("mmap dst");
		exit(errno);
	}
	// Copy the src to dst

	memcpy(dstaddr, srcaddr, sb.st_size);

	if (msync(dstaddr, sb.st_size, MS_SYNC) == -1) {
		perror("msync");
		exit(errno);
	}
	close(dstfd);
	printf("Copy OK!\n");
	exit(EXIT_SUCCESS);
}
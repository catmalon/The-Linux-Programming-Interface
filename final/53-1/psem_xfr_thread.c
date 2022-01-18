#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

char buf[BUFSIZ];
int endfile = 0;
static sem_t sem;
char dstfile[BUFSIZ];
int totByte = 0, totXfrs = 0;


static void * /* Loop 'arg' times incrementing 'glob' */
threadWrite(void *arg)
{
	int bytes, xfrs;
	int loops = *((int *) arg);
	size_t numRead;

	for (xfrs = 0, bytes = 0; ; xfrs++, bytes += numRead) {
		if (sem_wait(&sem) == -1) {
			perror("writer_sem_wait");
			exit(errno);
		}
		// printf("thread write xfrs = %d, bytes = %d\n", xfrs, bytes);

		numRead = read(STDIN_FILENO, buf, BUFSIZ);
		if (numRead == -1) {
			perror("writer_read");
			exit(errno);
		}
		// printf("endfile = %d\n", endfile);
		if (sem_post(&sem) == -1) {
			perror("writer_sem_post");
			exit(errno);
		}
		if (numRead == 0) {
			endfile = 1;
			break;
		}
		totByte += numRead;
		totXfrs++;
		sleep(1);
	}
	return NULL;
}
static void *
threadRead(void *arg)
{
	int bytes, xfrs;
	size_t numRead;
	int dstfd;
	if ((dstfd = open(dstfile, O_RDWR | O_CREAT | O_APPEND
					| S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)) == -1) {
		perror("open dst");
		exit(errno);
	}
	// printf("thread read open dstfile xfrs = %d\n",xfrs);

	for (xfrs = 0, bytes = 0; ; xfrs++, bytes += numRead) {
		if (sem_wait(&sem) == -1) {
			perror("reader_sem_wait");
			exit(errno);
		}
		// printf("thread read xfrs = %d, bytes = %d\n", xfrs, bytes);
		// printf("read endfile = %d\n", endfile);
		if (endfile) {
			bytes += sizeof(buf);
			break;
		}
		if (write(dstfd, buf, BUFSIZ) != BUFSIZ) {
			perror("reader_write");
			exit(errno);
		}
		if (sem_post(&sem) == -1) {
			perror("sem_post");
			exit(errno);
		}
		sleep(1);
	}

	close(dstfd);
	return NULL;
}

int main(int argc, char *argv[]) 
{
	pthread_t t1, t2;
	int loops, s;
	int xfrs, bytes;
	size_t numRead;

	loops = 10000000;

	if (argc != 2 || strcmp(argv[1], "--help")== 0) {
		fprintf(stderr,"Usage : %s destinationFILE\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	strcpy(dstfile, argv[1]);
	printf("Dstfile = %s\n", dstfile);
	
	/* Initialize a thread-shared mutex with the value 1 */
	if (sem_init(&sem, 0, 1) == -1) {
		perror("sem_init");
		exit(errno);
	}
	/* Create two threads that increment 'glob' */
	s = pthread_create(&t1, NULL, threadWrite, &loops);
	if (s != 0) {
		perror("pthread_create1");
		exit(errno);
	}
	s = pthread_create(&t2, NULL, threadRead, &loops);
	if (s != 0) {
		perror("pthread_create2");
		exit(errno);
	}

	s = pthread_join(t1, NULL);
	if (s != 0) {
		perror("pthread_join1");
		exit(errno);
	}
	s = pthread_join(t2, NULL);
	if (s != 0) {
		perror("pthread_join2");
		exit(errno);
	}
	printf("Received %d bytes (%d xfrs)\n", totByte, totXfrs);
	exit(EXIT_SUCCESS);
}
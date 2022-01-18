#include <limits.h>
#include "program.h"

int dataIdx = 0;

static void 
handlerUSR1(int sig, siginfo_t *siginfo, void *context)
{
	dataIdx = siginfo->si_value.sival_int;
	if ((dataIdx % 1000) == 0) {
		printf("pid = %d, dataIdx = %d\n", getpid(), dataIdx);
	}
	// printf("tsival_int = %d %d\n", siginfo->si_value.sival_int, dataIdx);
}

static void 
handlerINT(int sig, siginfo_t *siginfo, void *context)
{
	dataIdx = INT_MAX;
	printf("SIGINT\n");
}

int consumeData(int i, pid_t pid, int buffNum, int dataNum)
{
	char *addr = NULL;
	int fd = 0;
	int msgidx = 0;
	int receNum = 0;
	struct stat sb = {0};
	struct sigaction act = {0};
	struct sigaction act2 = {0};
	sigset_t old, new, wait;
	char buf[MESSAGE_SIZE] = {'\0'};
	int errorNum = 0;

	printf("pid = %d, consumeData\n", getpid());

	fd = shm_open(SHM_PATHNAME, O_RDONLY, 0); /* Open existing object */
	if (fd == -1) {
		perror("shm_open child");
		exit(errno);
	}
	// printf("pid = %d, fd = %d\n", getpid(), fd);
	/* Use shared memory object size as length argument for mmap()
	and as number of bytes to write() */
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		exit(errno);
	}
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(errno);
	}
	// printf("pid = %d, addr=%p\n", getpid(), addr);
	/* 'fd' is no longer needed */
	if (close(fd) == -1){
		perror("close");
		exit(errno);
	}

	sigemptyset(&act.sa_mask);
	act.sa_sigaction = handlerUSR1;
	act.sa_flags = SA_SIGINFO;

	sigemptyset(&act2.sa_mask);
	act2.sa_sigaction = handlerINT;
	act2.sa_flags = SA_SIGINFO;


	if (sigaction(SIGUSR1, &act, NULL) == -1) {
		perror("sigaction");
		exit(errno);
	}
	if (sigaction(SIGINT, &act2, NULL) == -1) {
		perror("sigaction");
		exit(errno);
	}

	for(int i = 0; i < dataNum; i++) {
		// block SIGUSR1
		sigemptyset(&new);
		sigaddset(&new, SIGUSR1);
		sigprocmask(SIG_BLOCK, &new, &old);
		
		sigfillset(&wait);
		sigdelset(&wait, SIGUSR1);
		sigdelset(&wait, SIGINT);

		if (dataIdx == INT_MAX) {
			break;
		}

		if (sigsuspend(&wait) != -1) {
			perror("sigsuspend");
		}
		if (dataIdx == INT_MAX) {
			break;
		}
		memcpy(buf, addr + MESSAGE_SIZE * (dataIdx % buffNum), MESSAGE_SIZE);

		sscanf(buf, "%*s %*s %*s %d", &msgidx);
		if (msgidx != dataIdx) {
			errorNum++;
			continue;
		}
		receNum++;
		if (msgidx%1000 == 0) {
			printf("Child %d receive NO.%d message (%d char)\n",getpid(), msgidx, MESSAGE_SIZE);
		}
	}
	printf("Child %d has %d wrong msg\n", getpid(), errorNum);
	return receNum;
}

int main(int argc, char *argv[])
{
	char *addr;
	int shmfd;
	int pipefd[2];
	int numRead;
	pid_t pid;
	struct stat sb;
	union sigval val;
	int dataNum, trxInterval, consumerNum, buffNum;
	int oneChild = 0, allChild = 0;
	pid_t pidList[CONSUMER_MAX] = {0};
	char message[MESSAGE_SIZE] = {'\0'};
	char buff[16];
	double loss = 0.0;

	if (argc != 5 || strcmp(argv[1], "--help")== 0) {
		fprintf(stderr,"Usage : %s DataNum TrxInterval(ms) ConsumerNum BuffNum\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	dataNum = atoi(argv[1]);
	trxInterval = atoi(argv[2]);
	consumerNum = atoi(argv[3]);
	buffNum = atoi(argv[4]);
	printf("read %d %d %d %d\n", dataNum, trxInterval, consumerNum, buffNum);

	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(errno);
	}

	// Open and mapping the share memory path
	if ((shmfd = shm_open(SHM_PATHNAME, O_RDWR | O_CREAT | O_TRUNC 
					| S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH, 0)) == -1) {
		perror("shm_open parent");
		exit(errno);
	}
	if (ftruncate(shmfd, sizeof(message)*buffNum) == -1) {
		perror("ftruncate");
		exit(errno);
	}

	if (fstat(shmfd, &sb) == -1) {
		perror("fstat");
		exit(errno);
	}
	printf("file size = %ld\n", (long)sb.st_size);

	addr = mmap(NULL, sb.st_size, PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap src");
		exit(errno);
	}

	/* 'shmfd' is no longer needed */
	if (close(shmfd) == -1) { 
		perror("close");
		exit(errno);
	}

	// fork
	for (int i = 0; i < consumerNum; i++) {
		int childrec = 0;
		pid = fork();
		if (pid == -1) {
			perror("fork");
			exit(errno);
		} else if (pid == 0) {   /* Clild. */
			// printf("child %d:\n", getpid());
			childrec = consumeData(i, pid, buffNum, dataNum);
			// printf("childrec = %d\n", childrec);
			sprintf(buff, "%d", childrec);
			// printf("buff in child = %s\n", buff);
			close(pipefd[0]);
			write(pipefd[1], buff, sizeof(buff));
			close(pipefd[1]);
			exit(childrec);
		} else {   /* Parent. */
			pidList[i] = pid;
			continue;
		}
	}
	// read child pipe
	close(pipefd[1]);

	sleep(1);
	for (int i = 0; i < dataNum; i++) {
		snprintf(message, sizeof(message), "This is message %d", i);
		memcpy(addr + sizeof(message)*(i % buffNum), message, sizeof(message));

		val.sival_int = i;
		for (int c = 0; c < consumerNum; c++) {
			if (sigqueue(pidList[c], SIGUSR1, val) == -1) {
				perror("sigqueue");
			}
		}
		usleep(trxInterval);
	}
	for (int i = 0; i < consumerNum; i++) {
		if (sigqueue(pidList[i], SIGINT, val) == -1) {
			perror("sigqueue");
			break;
		}
		if (waitpid(pidList[i], NULL, 0) > 0) {
			numRead = read(pipefd[0], buff, sizeof(buff));
			if (numRead <= 0) {
				break;
			} else {
				sscanf(buff, "%d", &oneChild);
				printf("%d: child %d receive %d\n",i+1, pidList[i], oneChild);
				allChild += oneChild;
			}
		}
	}
	close(pipefd[0]);

	loss = (double)allChild / (double)(dataNum*consumerNum);
	printf("M = %d\tR = %d ms\tN = %d\n", dataNum, trxInterval, consumerNum);
	printf("--------------------------------\n");
	printf("Total message: %d (%d * %d)\n", dataNum*consumerNum, dataNum, consumerNum);
	printf("Sum of received message by all consumer: %d\n", allChild);
	printf("Loss rate: %.3f\n", loss*100);
	printf("--------------------------------\n");

	if (shm_unlink(SHM_PATHNAME) == -1) {
		perror("shm_unlink");
		exit(errno);
	}

exit:
	exit(EXIT_SUCCESS);
}
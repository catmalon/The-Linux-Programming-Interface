#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define MESSAGE_SIZE 80
#define CONSUMER_MAX 2000
#define sa_sigaction __sigaction_handler.sa_sigaction
#define SHM_PATHNAME "/hw10_12"   /* Pathname for shared memory segment */
// #define FIFO "/tmp/return"
#define PATH_LEN (sizeof(SHM_PATHNAME) + 20)
/*  svshm_xfr_writer.c
+
+   Read buffers of data data from standard input into a System V shared memory
+   segment from which it is copied by svshm_xfr_reader.c
+
+   We use a pair of binary semaphores to ensure that the writer and reader have
+   exclusive, alternating access to the shared memory. (I.e., the writer writes
+   a block of text, then the reader reads, then the writer writes etc). This
+   ensures that each block of data is processed in turn by the writer and
+   reader.
+
+   This program needs to be started before the reader process as it creates the
+   shared memory and semaphores used by both processes.
+
+   Together, these two programs can be used to transfer a stream of data through
+   shared memory as follows:
+
+        $ svshm_xfr_writer < infile &
+        $ svshm_xfr_reader > out_file
*/
#include "semun.h"              /* Definition of semun union */
#include "pshm_xfr.h"

int bsUseSemUndo = 0;
int bsRetryOnEintr = 1;

int                     /* Initialize semaphore to 1 (i.e., "available") */
initSemAvailable(int semId, int semNum)
{
    union semun arg;

    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}
int                     /* Initialize semaphore to 0 (i.e., "in use") */
initSemInUse(int semId, int semNum)
{
    union semun arg;

    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}
/* Reserve semaphore (blocking), return 0 on success, or -1 with 'errno'
   set to EINTR if operation was interrupted by a signal handler */

int                     /* Reserve semaphore - decrement it by 1 */
reserveSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;

    while (semop(semId, &sops, 1) == -1)
        if (errno != EINTR || !bsRetryOnEintr)
            return -1;

    return 0;
}
int                     /* Release semaphore - increment it by 1 */
releaseSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;

    return semop(semId, &sops, 1);
}

int
main(int argc, char *argv[])
{
    int semid, shmid, bytes, xfrs;
    struct shmseg *shmp;
    union semun dummy;

    /* Create set containing two semaphores; initialize so that
       writer has first access to shared memory. */

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    if (semid == -1) {
		perror("semget");
		exit(errno);
	}

    if (initSemAvailable(semid, WRITE_SEM) == -1) {
		perror("initSemAvailable");
		exit(errno);
	}
    if (initSemInUse(semid, READ_SEM) == -1) {
		perror("initSemInUse");
		exit(errno);
	}

    /* Create shared memory; attach at address chosen by system */

    shmid = shm_open(SHM_PATHNAME, O_CREAT | O_EXCL | O_RDWR, OBJ_PERMS);
    if (shmid == -1) {
		perror("shm_open");
		exit(errno);
	}

    if (ftruncate(shmid, sizeof(struct shmseg)) == -1) {
        perror("ftruncate");
		exit(errno);
    }
    
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_WRITE, MAP_SHARED, shmid, 0);
    if ((void *)shmp == MAP_FAILED) {
		perror("mmap");
		exit(errno);
	}

    if (close(shmid) == -1) {
        perror("close");
		exit(errno);
    }

    /* Transfer blocks of data from stdin to shared memory */

    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
        if (reserveSem(semid, WRITE_SEM) == -1){         /* Wait for our turn */
			perror("reserveSem");
			exit(errno);
		}

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);
        if (shmp->cnt == -1) {
			perror("read");
			exit(errno);
		}

        if (releaseSem(semid, READ_SEM) == -1) {          /* Give reader a turn */
			perror("releaseSem");
			exit(errno);
		}

        /* Have we reached EOF? We test this after giving the reader
        a turn so that it can see the 0 value in shmp->cnt. */

        if (shmp->cnt == 0) {
			break;
		}
    }

    /* Wait until reader has let us have one more turn. We then know
    reader has finished, and so we can delete the IPC objects. */

    if (reserveSem(semid, WRITE_SEM) == -1) {
		perror("reserveSem");
		exit(errno);
	}

    if (semctl(semid, 0, IPC_RMID, dummy) == -1) {
		perror("semctl");
		exit(errno);
	}
    if (shm_unlink(SHM_PATHNAME) == -1) {
		perror("shm_unlink");
		exit(errno);
	}

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
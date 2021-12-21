#include "pshm_xfr.h"

int bsUseSemUndo = 0;
int bsRetryOnEintr = 1;

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
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;

    /* Get IDs for semaphore set and shared memory created by writer */

    semid = semget(SEM_KEY, 0, 0);
    if (semid == -1) {
        perror("semget");
		exit(errno);
    }

    shmid = shm_open(SHM_PATHNAME, O_RDONLY, 0);
    if (shmid == -1) {
        perror("shm_open");
		exit(errno);
    }

    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, shmid, 0);
    if ((void*)shmp == MAP_FAILED) {
        perror("mmap");
		exit(errno);
    }

    if (close(shmid) == -1) {
        perror("close");
		exit(errno);
    }

    /* Transfer blocks of data from shared memory to stdout */

    for (xfrs = 0, bytes = 0; ; xfrs++) {          /* Wait for our turn */
        if (reserveSem(semid, READ_SEM) == -1){
            perror("reserveSem");
		    exit(errno);
        }

        if (shmp->cnt == 0)                     /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
            fprintf(stderr, "partial/failed write\n");
        }

        if (releaseSem(semid, WRITE_SEM) == -1){         /* Give writer a turn */
            perror("releaseSem");
		    exit(errno);
        }
    }

    /* Give writer one more turn, so it can clean up */

    if (releaseSem(semid, WRITE_SEM) == -1) {
        perror("releaseSem");
		exit(errno);
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
/* pmsg_file.h

   Header file for pmsg_server.c and pmsg_client.c.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_MQ "/mq_sv"
#define CLIENT_MQ_TMP "/mq_cl.%ld"
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_TMP) + 20) //TMP + (long)

struct requestMsg {                     /* Requests (client to server) */
    pid_t  clientId;                      /* ID of client's message queue */
    char pathname[PATH_MAX];            /* File to be returned */
};

/* REQ_MSG_SIZE computes size of 'mtext' part of 'requestMsg' structure.
   We use offsetof() to handle the possibility that there are padding
   bytes between the 'clientId' and 'pathname' fields. */


#define RESP_MSG_SIZE 8192

struct responseMsg {                    /* Responses (server to client) */
    int type;
    char data[RESP_MSG_SIZE];           /* File content / response message */
};

#define RESP_MT_FAILURE 1               /* File couldn't be opened */
#define RESP_MT_DATA    2               /* Message contains file data */
#define RESP_MT_END     3               /* File data complete */

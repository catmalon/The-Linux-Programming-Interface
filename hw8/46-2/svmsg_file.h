/* svmsg_file.h

   Header file for svmsg_file_server.c and svmsg_file_client.c.
*/
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
 #include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define SERVER_KEY 0x1aaaaaa6           /* Key for server's message queue */

struct requestMsg {                     /* Requests (client to server) */
    long mtype;                         /* To server = 1 */
    int ClientId;                       /* who send req */
    int seqLen;                         /* Length of desired sequence */
};

struct responseMsg {                    /* Responses (server to client) */
    long mtype;                         /* To client = pID */
    int seqNum;                         /* response message */
};
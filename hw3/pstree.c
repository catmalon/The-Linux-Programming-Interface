#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

uid_t userIdFromName(const char *name);
int getStatus(const char *pid, uid_t *Uid, char *Name, pid_t *PPid, pid_t *Pid);
int stringIsDigit(const char *d_name);
void dfs(int index, int depth, int *pids[], char *pidsname[], int count[]);


int main (int argc, char* argv[])
{
    int uid = -1;
    DIR* dir = NULL;
    struct dirent* ptr = NULL;
    uid_t statusUID = 0;
    char cmd_name[1024] = {'\0'};
    pid_t PPid = 0;
    pid_t pid = 0;
    int *pids[65536] = {NULL};
    char *pidsname[65536] = {'\0'};
    int count[65536] = {0};
    
    if (argc != 2) {
        printf("[Usage] %s USER_NAME\n", argv[0]);
        return 0;
    }

    /*get uid*/
    uid = userIdFromName(argv[1]);
    if (uid == -1) {
        printf("[Usage] %s USER_NAME\n", argv[0]);
        return 1;
    }

    printf("user name = %s, uid = %d\n", argv[1], uid);
    /*open and read dir*/
    dir = opendir("/proc");
    while ((ptr = readdir(dir)) != NULL) {

        /*check if dir and digit*/
        if (ptr->d_type != DT_DIR || !stringIsDigit(ptr->d_name)) {
            continue;
        }
        if (!getStatus(ptr->d_name, &statusUID, cmd_name, &PPid, &pid)) {
            continue;
        }
        
        /*check uid is the user*/
        if (statusUID == uid) {
            // printf("pid = %d, ppid = %d\n", pid, PPid);
            pidsname[pid] = strdup(cmd_name);
            pids[PPid] = (int*) realloc(pids[PPid], (count[PPid]+1)* sizeof(int));
            pids[PPid][count[PPid]] = pid;
            count[PPid]++;
        }
    }
    closedir(dir);
    
    /*printf the tree*/
    dfs(1, 0, pids, pidsname, count);

    return 0;
}


/* Return UID corresponding to 'name', or -1 on error */
uid_t userIdFromName(const char *name)
{
    struct passwd *pwd;
    uid_t u;
    char *endptr;

    /* On NULL or empty string */
    if (name == NULL || *name == '\0') {
        return -1;
    }
    u = strtol(name, &endptr, 10);
    if (*endptr == '\0') {
        return u;
    }
    pwd = getpwnam(name);
    if (pwd == NULL){
        return -1;
    }
    return pwd->pw_uid;
}

int getStatus(const char *pid, uid_t *Uid, char *Name, pid_t *PPid, pid_t *Pid)
{
    int ret = 0;
    FILE *file = NULL;
    char pathname[PATH_MAX] = {'\0'};
    char buf[1024] = {'\0'};
    int getuid = 0, getcmdname = 0, getppid = 0, getpid = 0;

    /*open the stat file by pathname*/
    sprintf(pathname, "/proc/%s/status", pid);
    
    if ((file = fopen(pathname, "r")) == NULL) {
        goto Exit;
    }

    /*copy the info from status*/
    while (fgets(buf, sizeof(buf), file) != NULL) {
        if (strncmp(buf, "Name:", 5) == 0) {
            if (sscanf(buf, "Name: %s", Name)) {
                getcmdname = 1;
            }
        } else if (strncmp(buf, "Uid:", 4) == 0) {
            if (sscanf(buf, "Uid: %d", Uid)) {
                getuid = 1;
            }
        } else if (strncmp(buf, "PPid:", 5) == 0) {
            if (sscanf(buf, "PPid: %d", PPid)) {
                getppid = 1;
            }
        } else if (strncmp(buf, "Pid:", 4) == 0) {
            if (sscanf(buf, "Pid: %d", Pid)) {
                getpid = 1;
            }
        }
        if (getuid && getcmdname && getppid && getpid) {
            ret = 1;
            goto Exit;
        }
    }

Exit:
    fclose(file);
    return ret;
}

int stringIsDigit(const char *d_name)
{
    while (*d_name != '\0') {
        if (!isdigit(*d_name)) {
            return 0;
        }
        d_name++;
    }
    return 1;
}

void dfs(int index, int depth, int *pids[], char *pidsname[], int count[])
{
    if (pids[index] == NULL) {
        return;
    }

    if (depth > 0) {
        for (int i = 0; i < depth; i++) {
            printf("  ");
        }
    }
    if (pidsname[index]) {
        printf("-> %s(%d) \n", pidsname[index], index);
    } else {
        printf("NONAME\n");
    }
    for (int i = 0; i < count[index]; i++) {
        dfs(pids[index][i], depth+1, pids, pidsname, count);
    }
}
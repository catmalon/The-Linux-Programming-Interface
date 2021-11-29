#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/limits.h>

int getStatus(const char *pid, char *Name, pid_t *Pid);
int stringIsDigit(const char *d_name);


int main (int argc, char* argv[])
{
    DIR *dir1 = NULL, *dir2 = NULL;
    struct dirent *ptr1 = NULL, *ptr2 = NULL;
    char cmd_name[1024] = {'\0'};
    char pathname[PATH_MAX] = {'\0'};
    pid_t Pid = 0;
    char buf[1024] = {'\0'};
    
    if (argc != 2) {
        printf("[Usage] %s PATH_NAME\n", argv[0]);
        return 0;
    }

    printf("pathname = %s\n", argv[1]);

    /* open and read dir: /proc */
    dir1 = opendir("/proc");
    while ((ptr1 = readdir(dir1)) != NULL) {

        /*check if dir and digit*/
        if (ptr1->d_type != DT_DIR || !stringIsDigit(ptr1->d_name)) {
            continue;
        }
        if (!getStatus(ptr1->d_name, cmd_name, &Pid)) {
            continue;
        }

        /*open and read dir: /proc/.../fd */
        sprintf(pathname, "/proc/%s/fd", ptr1->d_name);
        dir2 = opendir(pathname);
        while ((ptr2 = readdir(dir2)) != NULL) {
            char fd_path[PATH_MAX] = {'\0'};
            if (ptr2->d_type != DT_LNK || !stringIsDigit(ptr2->d_name)) {
                continue;
            }
            sprintf(fd_path, "/proc/%s/fd/%s", ptr1->d_name, ptr2->d_name);
            if (readlink(fd_path, buf, sizeof(buf)) < 0) {
                continue;
            }
            if (strcmp(argv[1], buf) == 0) {
                printf("%s(%d) open the file pathname = %s\n", cmd_name, Pid, buf);
            }
        }
        closedir(dir2);

    }
    closedir(dir1);

    return 0;
}



int getStatus(const char *pid, char *Name, pid_t *Pid)
{
    int ret = 0;
    FILE *file = NULL;
    char pathname[PATH_MAX] = {'\0'};
    char buf[1024] = {'\0'};
    int  getcmdname = 0, getpid = 0;

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
        } else if (strncmp(buf, "Pid:", 4) == 0) {
            if (sscanf(buf, "Pid: %d", Pid)) {
                getpid = 1;
            }
        }
        if (getcmdname && getpid) {
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

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ftw.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <linux/limits.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (10 * EVENT_SIZE + NAME_MAX + 1)

struct watch {
	int wd;                     /* Watch descriptor (-1 if slot unused) */
	char path[PATH_MAX];        /* Cached pathname */
};
struct watch *wArray = NULL;   /* Array of cached items */
int NumWatch = 0;
int inotifyFd;

void save_watch_to_array(int wd, const char *pathname) 
{
	NumWatch++;
	wArray = realloc(wArray, NumWatch * sizeof(struct watch));
	if (wArray == NULL) {
		perror("realloc");
		exit(errno);
	}
	wArray[NumWatch-1].wd = wd;
	strncpy(wArray[NumWatch-1].path, pathname, PATH_MAX);
}

static int add_watch(const char *fpath) 
{
	int wd = 0;

	wd = inotify_add_watch(inotifyFd, fpath, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
	if (wd == -1) {
		perror("inotyfy_add_watch");
		exit(errno);
	}
	save_watch_to_array(wd, fpath);
	return wd;
}

static int is_dir(char *fpath)
{
	struct stat sb;
	if (stat(fpath, &sb) == -1) {
		perror("stat");
		exit(errno);
	}
	return S_ISDIR(sb.st_mode);
}

int init_watch(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) 
{
	int wd = 0;
	if (tflag != FTW_D) {
		return 0;
	}
	wd = add_watch(fpath);
	printf("Watching %s using wd %d\n", fpath, wd);
	return 0;
}

static void remove_watch_from_array(int wd)
{
	int i = 0;
	while (i < NumWatch) {
		if (wArray[i].wd == wd){
			break;
		}
		i++;
	}
	memcpy(&wArray[i], &wArray[--NumWatch], sizeof(struct watch));
}

void handle_event(struct inotify_event *event)
{
	char fpath[PATH_MAX * 2];
	char buf[PATH_MAX];
	int i = 0, j = 0;

	if (event->mask & IN_IGNORED) {
		printf("Delect event ignore\n");
		return;
	}

	while (i < NumWatch) {
		if (wArray[i].wd == event->wd) {
			strcpy(buf, wArray[i].path);
			break;
		}
		i++;
	}
	if (i == NumWatch) {
		fprintf(stderr,"can't find wd: %d\n", event->wd);
		exit(EXIT_FAILURE);
	}

	sprintf(fpath, "%s/%s", buf, event->name);

	// create event
		if (event->mask & IN_CREATE) {
		int newWd = 0;
		printf("wd = %d, [%s] IN_CREATE\n\tpathname = %s\n", event->wd, event->name, fpath);
		if (is_dir(fpath)) {
			newWd = add_watch(fpath);
			printf("Watching %s using wd %d\n", fpath, newWd);
		}
	}
	// delete event
	if (event->mask & IN_DELETE) {
		printf("wd = %d, [%s] IN_DELETE\n\tpathname = %s\n", event->wd, event->name, fpath);
		int deleDirWd = 0;
		while (j < NumWatch) {
			if (strcmp(fpath, wArray[j].path) == 0) {
				deleDirWd = wArray[j].wd;
				break;
			}
			j++;
		}
		if (j != NumWatch) {
			inotify_rm_watch(inotifyFd, deleDirWd);
			remove_watch_from_array(deleDirWd);
			printf("Stop watching %s using wd %d\n", wArray[i].path, deleDirWd);

		}
	}
	// event rename from
	if (event->mask & IN_MOVED_FROM) {
		printf("wd = %d, [%s] IN_MOVED_FROM\n\tpathname = %s\n", event->wd, event->name, fpath);
		while (j < NumWatch) {
			if (strcmp(fpath, wArray[j].path) == 0) {
				break;
			}
			j++;
		}
		if (j != NumWatch) {
			printf("Change wd %d from %s\n", wArray[j].wd, wArray[j].path);
			sprintf(wArray[j].path, "%d", event->cookie);
		}
	}
	//event rename to
	if (event->mask & IN_MOVED_TO) {
		printf("wd = %d, [%s] IN_MOVED_TO\n\tpathname = %s\n", event->wd, event->name, fpath);
		if (is_dir(fpath)) {
			char cookiebuf[10];
			sprintf(cookiebuf, "%d", event->cookie);
			while (j < NumWatch) {
				if (strcmp(cookiebuf, wArray[j].path) == 0) {
					break;
				}
				j++;
			}
			if (j != NumWatch) {
				printf("Change wd %d to %s\n", wArray[j].wd, fpath);
				strcpy(wArray[j].path, fpath);
			}
		}
	}
}


int main (int argc, char* argv[])
{
	char buf[EVENT_BUF_LEN];
	struct inotify_event *event;
	ssize_t readSiz;
	char *p;

	if (argc < 2 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
		fprintf(stderr,"Usage : %s ROOT_DIR\n",argv[0]);
		exit(1);
	}

	inotifyFd = inotify_init();
	if (inotifyFd == -1) {
		perror("inotify_init");
		exit(errno);
	}

	if (nftw(argv[1], init_watch, 20, 0) == -1) {
		perror("nftw");
		exit(EXIT_FAILURE);
	}

	// read events
	while(1) {
		readSiz = read(inotifyFd, buf, EVENT_BUF_LEN);
		if (readSiz == 0) {
			fprintf(stderr,"read() from inotify fd returned 0!\n");
		}
		if (readSiz == -1) {
			perror("read");
			exit(errno);
		}
		printf("Read %ld bytes from inotify fd\n", (long) readSiz);
		for (p = buf; p < buf + readSiz; ) {
			event = (struct inotify_event *) p;
			handle_event(event);
			p += sizeof(struct inotify_event) + event->len;
		}
	}
	return 0;
}
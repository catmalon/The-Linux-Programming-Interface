#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>

int main() 
{
	struct utsname uts;
	long hostid = 0;

	if (uname(&uts) == -1) {
		perror("hostinfo.c:main:uname");
		exit(errno);
	}
	if ((hostid = gethostid()) == -1) {
		perror("hostinfo.c:main:hostid");
		exit(1);
	}

	printf("hostname: %s\n", uts.nodename);
	printf("%s\n", uts.release);
	printf("host id: %ld (%lx)\n", hostid, hostid);

	return 0;
}

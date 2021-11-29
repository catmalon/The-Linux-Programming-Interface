#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <linux/limits.h>
#include <errno.h>

int main() 
{
	char *dir = NULL;
	
	if ((dir = getcwd((char *)NULL, PATH_MAX + 1)) == NULL) {
		perror("getcwd");
		exit(errno);
	}

	printf("%s\n", dir);
	free(dir);
	return 0;
}

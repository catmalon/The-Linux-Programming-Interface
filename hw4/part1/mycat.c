#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	FILE *file = NULL;
	char buf[1024] = {'\0'};
	
	if (argc != 2) {
		printf("[Usage] %s FILENAME\n", argv[0]);
		return 0;
	}

	// open the file
	if ((file = fopen(argv[1], "r")) == NULL) {
		perror("fopen");
		exit(errno);
	}

	// print the file to STDOUT
	while (fgets(buf, sizeof(buf), file) != NULL) {
		printf("%s", buf);
	}
	fclose(file);
	return 0;
}
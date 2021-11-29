#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>

int main() 
{
	time_t rawtime;
	struct tm *info;
	char buffer[80];

	time(&rawtime);
	info = localtime(&rawtime);

	strftime(buffer, sizeof(buffer),"%b %d(%a), %Y %I:%M %p", info);
	printf("%s\n", buffer);
	return 0;
}
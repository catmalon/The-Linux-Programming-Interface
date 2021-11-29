#include <sys/types.h>
#include <stdio.h>

pid_t chile_pid[65536];

FILE* mypopen(const char *command, const char *open_mode);
int mypclose(FILE *stream_to_close);

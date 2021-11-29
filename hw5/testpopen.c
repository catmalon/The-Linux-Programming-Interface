#include <stdio.h>
#include <stdlib.h>
#include "mypopen.h"

// int main ()
// {
//     FILE *read_fp = NULL;
//     char buff[1024] = {'\0'};
//     int c_read = 0;

//     read_fp = mypopen("ls -l", "r");

//     if (read_fp) {
//         fread(buff, sizeof(char), sizeof(buff), read_fp);
//         printf("%s\n", buff);
//     }
//     pclose(read_fp);

//     return 0;
// }


int main ()
{
    FILE *read_fp, *write_fp = NULL;
    char buff[1024] = {'\0'};
    int c_read = 0;

    read_fp = mypopen("ls -l", "r");
    write_fp = mypopen("grep rw-r--r--", "w");

    if (read_fp && write_fp) {
        c_read = fread(buff, sizeof(char), sizeof(buff), read_fp);
        while (c_read > 0) {
            buff[c_read] = '\0';

            fwrite(buff, sizeof(char), c_read, write_fp);
            c_read = fread(buff, sizeof(char), sizeof(buff), read_fp);
        }

    }
    pclose(read_fp);
    pclose(write_fp);

    return 0;
}
#include <sys/types.h>
#include <fcntl.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <unistd.h>

#define READ 0
#define WRITE 1

pid_t popen2(const char *command, int *infp, int *outfp);


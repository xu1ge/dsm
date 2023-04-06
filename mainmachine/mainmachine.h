#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <unistd.h>
// #include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#define DSM_SIGRTMIN 34

#define CONFIG_PATH "/home/zx/code/log/dsm.log"
#define DATENOW() ({time_t clock; (long int)time(&clock);})

#define INFO(str, para...) do { \
    char buff[128] = {0};\
	sprintf(buff, "%ld [%s][%s:%d]: "str"\n",DATENOW(),__FUNCTION__,__FILE__,__LINE__,##para);\
    int fd = open(CONFIG_PATH, O_RDWR|O_APPEND|O_CREAT, 0777);\
    write(fd, buff, strlen(buff));\
    close(fd);\
}while(0)

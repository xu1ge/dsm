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
#define FILE_IN_PATH "../mainmachine/input/test.txt"
#define FILE_OUT_PATH "../mainmachine/output/"
#define CONFIG_PATH "/home/zx/code/log/dsm.log"

// #define DATENOW() ({time_t clock; (long int)time(&clock);})
#define DATENOW() ({time_t clock = time(0); char s[32]; strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", localtime(&clock)); s;})

#define _CONS(a, b) (a##b)
#define CONS(a, b) _CONS(a, b)
#define MIN(a,b) (((a) < (b)) ? (a) : (b))


#define INFO(str, para...) do { \
    char _buff[128] = {0};\
	sprintf(_buff, "(%s) [%s][%s:%d]: "str"\n",DATENOW(),__FUNCTION__,__FILE__,__LINE__,##para);\
    int fd = open(CONFIG_PATH, O_RDWR|O_APPEND|O_CREAT, 0777);\
    write(fd, _buff, strlen(_buff));\
    close(fd);\
}while(0)

#define WRITE_FILE(str, len) do { \
    char _buff[128] = {0};\
    sprintf(_buff, "%sdsmoutfile_%s",FILE_OUT_PATH,DATENOW());\
    int fd = open(_buff, O_RDWR|O_APPEND|O_CREAT, 0777);\
    write(fd, str, len);\
    close(fd);\
}while(0)




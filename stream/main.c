#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#define DSM_SIGRTMIN 34

int g_tasklist = -1;

int dataIn(FILE *fp, int len) {
    char buff[255];
    fgets(buff, 255, (FILE*)fp);
    printf("r: %s\n", buff);
    return 0;
}

int dataOut(FILE *fp) {
    return 0;
}

void handleSig(int sig) {
    printf("receive sig %d\n", sig);
    switch (sig)
    {
    case DSM_SIGRTMIN:
        g_tasklist = 0;
        break;
    
    case DSM_SIGRTMIN+1:
        g_tasklist = 1;
        break;
    
    default:
        break;
    }
}

int handleTask(int task) {
    int ret = 0;
    switch (task)
    {
    case 0:
        printf("task in\n");
        break;
    
    case 1:
        printf("task out\n");
        break;
    
    default:
        ret = 1;
        break;
    }
    return ret;
}


void polling() {
    int ret = 0;
    while(1) {
        if(g_tasklist != -1) {
            ret = handleTask(g_tasklist);
            g_tasklist = -1;
        }
        sleep(3);
    }
}
int main(int argc, char **argv) {
    int ret = 0;
    signal(SIGRTMIN, handleSig);
    signal(SIGRTMIN+1, handleSig);
    // 启动守护线程
    daemon(0, 1);
    // 轮询执行
    polling();

    return 0;
}
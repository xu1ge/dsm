#include "stream.h"
#include "window.h"

int g_tasklist = -1;
int g_taskexit = -1;
window *win = NULL;

int getFileSize(int *len) {
    FILE *fp = NULL;
    fp = fopen(FILE_IN_PATH, "r"); 
    if (fp == NULL) {
        printf("path error!: %s\n", strerror(errno));
        return 1;
    }
    //获取文件大小
    fseek(fp, 0L, SEEK_END);
    *len = ftell(fp);
    return 0;
}
// int dataIn() {
//     char buff[255];
//     FILE *fp = NULL;

//     fp = fopen(FILE_IN_PATH, "r"); 
//     if (fp == NULL) {
//         INFO("filepath error!: %s", strerror(errno));
//         return 1;
//     }
    
//     fgets(buff, 255, (FILE*)fp);
//     INFO("readfile: %c%c%c%c%c", buff[0], buff[1], buff[2], buff[3], buff[4]);
//     return 0;
// }

int dataIn() {
    int ret = 0;
    int len = 0;
    char buff[1024];
    FILE *fp = NULL;

    ret = getFileSize(&len);
    if (len >= 1024) {
        INFO("filesize error, size = %d", len);
        return 0;
    }
    fp = fopen(FILE_IN_PATH, "r"); 
    if (fp == NULL) {
        INFO("filepath error!: %s", strerror(errno));
        return 1;
    }
    
    fgets(buff, len, (FILE*)fp);
    INFO("readfile: %c%c%c%c%c...", buff[0], buff[1], buff[2], buff[3], buff[4]);

    ret = windowDataIn(win, buff, len);
    if (ret != 0) {
        INFO("windowDataIn error, ret = %d", ret);
    }
    INFO("dataIn success");

    return 0;
}

int dataOut() {
    int ret = 0;
    int len = 0;
    char buff[1024];
    ret = windowDataOut(win, buff, &len);
    if (ret != 0) {
        INFO("windowDataOut error, ret = %d", ret);
        return 1;
    }
    if(len != 0) {
        WRITE_FILE(buff, len);
        INFO("dataOut success");
    } else {
        INFO("dataOut no file");
    }

    return 0;
}

int dataExit() {
    int ret = 0;
    g_taskexit = 0;
    ret = windowExit(win);
    INFO("exit success");
    return ret;
}

void handleSig(int sig) {
    INFO("receive sig %d", sig);
    switch (sig)
    {
    case DSM_SIGRTMIN:
        g_tasklist = 0;
        break;
    
    case DSM_SIGRTMIN+1:
        g_tasklist = 1;
        break;

    case DSM_SIGRTMIN+2:
        g_tasklist = 2;
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
        INFO("task in");
        ret = dataIn();
        break;
    
    case 1:
        INFO("task out");
        ret = dataOut();
        break;
    
    case 2:
        INFO("task exit");
        ret = dataExit();
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
        if (g_tasklist != -1) {
            ret = handleTask(g_tasklist);
            if (ret != 0) {
                INFO("handleTask error");
                return;
            }
            g_tasklist = -1;
        }
        if (g_taskexit == 0) {
            break;
        }
        sleep(3);
    }
}

int main(int argc, char **argv) {
    int ret = 0;
    // init
    windowInit(&win);
    INFO("win = %p", win);//////////////////
    signal(SIGRTMIN, handleSig);
    signal(SIGRTMIN+1, handleSig);
    signal(SIGRTMIN+2, handleSig);
    // 启动守护线程
    ret = daemon(1, 1);
    if (ret != 0) {
        INFO("daemon error");
        return 1;
    }
    // 轮询执行
    polling();

    return 0;
}
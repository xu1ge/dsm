#include "mainmachine.h"//

int handleParam(int argc, char **argv, char *path, int *len) {
    if (argv[1][0] != '-' || (argc != 3 && argv[1][1] == 'i') || 
    (argc != 2 && argv[1][1] == 'o') || (argc != 2 && argv[1][1] == 'e')) {
        printf("param error\n");
        return 1;
    }

    if (argc == 3) {
        strcpy(path, argv[2]);
        FILE *fp = NULL;
        fp = fopen(path, "r"); 
        if (fp == NULL) {
            printf("path error!: %s\n", strerror(errno));
            return 1;
        }
        //获取文件大小
        fseek(fp, 0L, SEEK_END);
        *len = ftell(fp);

        fclose(fp);
    }

    return 0;
}

int getStreamPid(int *pid) {
    FILE *fp = NULL;
    char buff[32] = { 0 };
    fp = popen("ps -ef | grep stream | grep -v grep | awk '{print $2}'", "r");
    if(fp == NULL) {
        printf("get pid cmd error!: %s\n", strerror(errno));
        return 1;
    }

    fgets(buff, sizeof(buff), fp);

    *pid = atoi(buff);//no safe
    if(*pid == 0) {
        printf("get pid value error!: %s\n", strerror(errno));
        return 1;
    }

    pclose(fp);

    return 0;
}

int getIp(char *ip) {
    FILE *fp = NULL;
    char buff[32] = { 0 };
    fp = popen("ifconfig | grep inet.*broadcast | awk '{print $2}'", "r");
    if(fp == NULL) {
        printf("getIp error!: %s\n", strerror(errno));
        return 1;
    }

    fgets(buff, sizeof(buff), fp);
    memcpy(ip, buff, sizeof(buff));

    pclose(fp);

    return 0;
}

int main(int argc, char **argv) {
    int ret = 0;
    int pid = 0;
    int file_size = 0;
    char file_path[50] = { 0 };
    char ip[32] = { 0 };
    
    INFO("start:%d", ret);

    ret = getIp(ip);
    if (ret != 0) {
        printf("getIp error %d\n", ret);
        return 1;
    }
    printf("ip = %s\n", ip);

    ret = handleParam(argc, argv, file_path, &file_size);
    if (ret != 0) {
        printf("handleParam error %d\n", ret);
        return 1;
    }

    ret = getStreamPid(&pid);
    if (ret != 0) {
        printf("getStreamPid error %d\n", ret);
        return 1;
    } else {
        printf("getStreamPid %d\n", pid);
    }

    switch(argv[1][1]) {
        case 'i':
            printf("task in %s, len is %d\n", file_path, file_size);
            ret = kill(pid, DSM_SIGRTMIN);
            break;
            
        case 'o':
            printf("task out \n");
            ret = kill(pid, DSM_SIGRTMIN+1);
            break;
        case 'e':
            printf("task exit \n");
            ret = kill(pid, DSM_SIGRTMIN+2);
            break;
        default:
            printf("error\n");
            break;
    }
    if (ret != 0) {
        printf("kill error!: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
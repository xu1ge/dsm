#include <stdio.h>
#include <string.h>
#include <errno.h>

int handleParam(int argc, char **argv, char *path, int *len) {
    if (argc != 3 || argv[1][0] != '-') {
    printf("param error\n");
    return 1;
    }
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
    return 0;
}

int dataIn(FILE *fp, int len) {
    char buff[255];
    fgets(buff, 255, (FILE*)fp);
    printf("r: %s\n", buff);
    return 0;
}

int dataOut(FILE *fp) {
    return 0;
}

int main(int argc, char **argv) {
    int ret = 0;
    char file_path[50];
    int file_size = 0;
    printf("start\n");

    ret = handleParam(argc, argv, file_path, &file_size);
    if (ret != 0) {
        printf("handleParam error %d\n", ret);
        return 1;
    }

    FILE *fp = NULL;
    fp = fopen(file_path, "r");

    switch(argv[1][1]) {
        case 'i':
            printf("task in %s, len is %d\n", file_path, file_size);
            ret = dataIn(fp, file_size);
            break;
            
        case 'o':
            printf("task out %s\n", file_path);
            ret = dataOut(fp);
            break;
        default:
            printf("error\n");
            break;
    }
    fclose(fp);

    return 0;
}
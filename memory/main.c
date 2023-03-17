#include <stdio.h>
#include <string.h>

int dataIn() {
    return 0;
}

int dataOut() {
    return 0;
}

int main(int argc, char **argv) {
    int ret = 0;
    char path[50];
    printf("start\n");

    for(int i = 1; i < argc; i++){
        if(argc != 3) {
            printf("param error\n");
            return 1;
        }
        if(argv[i][0] == '-') {
            switch(argv[i][1]) {
                case 'i':
                {
                    strcpy(path, argv[i+1]);
                    printf("task in %s\n", path);
                    ret = dataIn();

                    FILE *fp = NULL;
                    char buff[255];
                    
                    fp = fopen(path, "r");                   
                    fgets(buff, 255, (FILE*)fp);
                    printf("r: %s\n", buff);
                    fclose(fp);

                    break;
                }
                    
                case 'o':
                    printf("task out \n");
                    ret = dataOut();
                    break;
                default:
                    printf("error\n");
                    break;
            }
        }
    }
    return 0;
}
#include "stream.h"
#define PANE_SIZE 1024
#define PANE_NUM 20
#define PANE_FILE 100
#define PREMALLOC 5

typedef struct {
    int num; // 最大100
    int temp_size; // 写到哪个位置
    int temp_read; // 读到哪个文件
    int location[PANE_FILE];
    int size[PANE_FILE];
    char memory[PANE_SIZE]; // 16K
} pane;

typedef struct {
    int length; // 20
    int first;
    int second;
    int third;
    pane *pane[PANE_NUM]; // 20个指针
} window;

int windowInit(window **win);
int windowExit(window *win);
int windowDataIn(window *win, char *data, int len);
int windowDataOut(window *win, char *data, int *len);
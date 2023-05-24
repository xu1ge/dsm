#include "../config.h"
#define PANE_SIZE 1024
#define PANE_NUM 50
// #define PANE_NUM 20
#define PANE_FILE 100
#define PREMALLOC 5

typedef struct {
    int num;                 // 最大100
    int temp_size;           // 写到哪个位置
    int temp_read;           // 读到哪个文件
    int location[PANE_FILE];
    int size[PANE_FILE];
    char memory[PANE_SIZE];  // 16K
} pane;

typedef struct {
    int length;              // 最大20
    int first;               // first指针，记录数据末端pane，即销毁free地址
    int second;              // second指针，记录数据当前写入pane
    int third;               // third指针，记录当前申请的pane
    pane *pane[PANE_NUM];    // 20个指针
} window;

int windowInit(window **win);
int windowReInit(window **win);
int windowExit(window *win);
int windowDataIn(window *win, char *data, int len);
int windowDataOut(window *win, char *data, int *len);
int ssdDataIn(window *win, char *data, int len);
int ssdDataOut(window *win, char *data, int *len);
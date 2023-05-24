// #include "../config.h"
#include "window.h"

int preMalloc(window *win) {
    int ret = 1;
    int start_pane = win->third;
    int mallocNum = MIN_DSM(win->first, (win->third+PREMALLOC)%PANE_NUM);

    for(int i = start_pane; i < start_pane+PREMALLOC; ++i) {
        if (i%PANE_NUM == mallocNum) break;

        // 清空内存
        if (win->pane[i%PANE_NUM] != NULL) {
            free(win->pane[i%PANE_NUM]);
        }
        // 申请内存
        win->pane[i%PANE_NUM] = (pane*)malloc(sizeof(pane));
        if (win->pane[i%PANE_NUM] == NULL) {
            INFO("malloc pane[%d] fail", i%PANE_NUM);
            return 1;
        } else {
            // 动态内存初始化
            memset(win->pane[i%PANE_NUM], 0, sizeof(pane));
            // second指针移动
            win->third = i%PANE_NUM + 1;
            INFO("malloc pane[%d]: size = %lu, third = %d", i%PANE_NUM, sizeof(pane), win->third);
            // 标记可写状态
            ret = 0;
        }
    }
    return ret;
}

int windowInit(window **win) {
    int ret = 0;
    *win = (window*)malloc(sizeof(window));
    if (*win == NULL) {
        INFO("malloc window fail");
        return 1;
    } else {
        INFO("malloc window: size = %lu", sizeof(window));
    }
    
    (*win)->length = PANE_NUM;
    (*win)->first = -1;
    (*win)->second = -1;
    (*win)->third = 0;
    for(int i = 0; i < PANE_NUM; ++i) {
        (*win)->pane[i] = NULL;
    }
    ret = preMalloc(*win);
    (*win)->first = 0;
    (*win)->second = 0;

    return ret;
}

int windowExit(window *win) {
    int ret = 0;
    for(int i = 0; i < PANE_NUM; ++i) {
        free(win->pane[i]);
        win->pane[i] = NULL;
    }
    free(win);
    return ret;
}

int windowMalloc(window **win, int left, int right) {
    int ret = 0;
    for(int i = left; i < right+1; ++i) {
        (*win)->pane[i] = (pane*)malloc(sizeof(pane));
        if ((*win)->pane[i] == NULL) {
            INFO("malloc pane[%d] fail", i%PANE_NUM);
            return 1;
        } else {
            memset((*win)->pane[i], 0, sizeof(pane));
            INFO("windowReInit pane[%d]", i);
        }
    }
    return ret;
}

int windowReInit(window **win) {
    int ret = 0;
    int first = (*win)->first;
    int third = (*win)->third;
    for(int i = 0; i < PANE_NUM; ++i) {
        free((*win)->pane[i]);
        (*win)->pane[i] = NULL;
    }
    if(first < third) {
        ret = windowMalloc(win, first, third-1);
        if(ret != 0) {
            INFO("windowMalloc fail, ret = %d", ret);
            return ret;
        }
    } else {
        ret = windowMalloc(win, 0, third-1);
        if(ret != 0) {
            INFO("windowMalloc fail, ret = %d", ret);
            return ret;
        }

        ret = windowMalloc(win, first, PANE_NUM-1);
        if(ret != 0) {
            INFO("windowMalloc fail, ret = %d", ret);
            return ret;
        }
    }
    return 0;
}

int windowDataIn(window *win, char *data, int len) {
    int ret = 0;
    pane *tpane = win->pane[win->second];

    // 是否更换second写指针
    if (tpane->num >= PANE_FILE || tpane->temp_size + len >= PANE_SIZE) {
        if ((win->third-win->second+PANE_NUM) % PANE_NUM < 2) {
            ret = preMalloc(win);
            if (ret != 0) {
                INFO("preMalloc error:ret = %d", ret);
                return ret;
            }
        }

        win->second = (win->second+1)%PANE_NUM;
        tpane = win->pane[win->second];
    }

    memcpy(&tpane->memory[tpane->temp_size], &data[0], len*sizeof(char));
    tpane->size[tpane->num] = len;
    tpane->location[tpane->num] = tpane->temp_size;
    tpane->temp_size = tpane->temp_size + len;
    tpane->num = tpane->num + 1;
    INFO("windowDataIn success, pane[%d]:num = %d, temp_size = %d, %d %d %d", win->second, tpane->num, tpane->temp_size, win->first, win->second, win->third);

    return ret;
}

int windowDataOut(window *win, char *data, int *len) {
    int ret = 0;
    pane *tpane = win->pane[win->first];

    // 是否更换first读指针
    if (tpane->temp_read >= tpane->num && ((win->second-win->first+PANE_NUM)%PANE_NUM) >= 1) { // 更换指针
        win->first = (win->first+1)%PANE_NUM;
        tpane = win->pane[win->first];
    }
    else if(tpane->temp_read >= tpane->num) { // 读到头了
        INFO("no file to read");
        return 0;
    }

    *len = tpane->size[tpane->temp_read];
    memcpy(&data[0], &tpane->memory[tpane->location[tpane->temp_read]], *len);
    INFO("windowDataOut success, read_pane[%d]:read_num = %d, read_size = %d, %d %d %d", win->first, tpane->temp_read, *len, win->first, win->second, win->third);
    
    tpane->temp_read = tpane->temp_read + 1;

    return ret;
}






int getSSDFileSize(int *len) {
    FILE *fp = NULL;
    fp = fopen(FILE_OUT_PATH_SSD, "r"); 
    if (fp == NULL) {
        printf("path error!: %s\n", strerror(errno));
        return 1;
    }
    //获取文件大小
    fseek(fp, 0L, SEEK_END);
    *len = ftell(fp);
    return 0;
}

int ssdDataIn(window *win, char *data, int len) {
    SSD_WRITE_FILE(data, len);
    return 0;
}

int ssdDataOut(window *win, char *data, int *len) {
    int ret = 0;
    char buff[1024];
    FILE *fp = NULL;
    ret = getSSDFileSize(len);
    if (*len >= 1024) {
        INFO("filesize error, size = %d", *len);
        return 0;
    }
    fp = fopen(FILE_OUT_PATH_SSD, "r"); 
    if (fp == NULL) {
        INFO("filepath error!: %s", strerror(errno));
        return 1;
    }
    
    fgets(buff, *len+1, (FILE*)fp);
    INFO("readfile: %c%c%c%c%c...", buff[0], buff[1], buff[2], buff[3], buff[4]);

    memcpy(data, buff, *len);
    remove(FILE_OUT_PATH_SSD);

    return ret;
}

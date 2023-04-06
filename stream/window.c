#include "stream.h"
#include "window.h"

int preMalloc(window *win) {
    int ret = 1;
    int start_pane = win->third;
    int mallocNum = MIN(win->first, (win->third+PREMALLOC)%PANE_NUM);

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
    tpane->location[tpane->num] = tpane->temp_size + len;
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
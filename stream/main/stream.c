// #include "../config.h"
#include "stream.h"
#include "../rpc/temp/dsm_clnt_func.h"
#include "../rpc/temp/rpc_dsm_svc.h"

int g_tasklist = -1;
int g_taskexit = -1;
int local_node = -1;
char Memory[PANE_SIZE];
// window *win = NULL;
window *win[NODE_NUM] = { NULL };
char *ip_list[] = {"172.172.0.11", "172.172.0.12", "", ""};

int getIp(int *ip) {
    FILE *fp = NULL;
    char buff[32] = { 0 };
    fp = popen("ifconfig | grep inet.*broadcast | awk '{print $2}'", "r");
    if(fp == NULL) {
        printf("getIp error!: %s\n", strerror(errno));
        return 1;
    }

    fgets(buff, sizeof(buff), fp);
    if(strlen(buff) < 7) {
        printf("getIp len error!: %s\n", strerror(errno));
        return 1;
    }
    *ip = (int)buff[strlen(buff)-2] - 48;

    pclose(fp);
    return 0;
}

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
    
    fgets(buff, len+1, (FILE*)fp);
    INFO("readfile: %c%c%c%c%c...", buff[0], buff[1], buff[2], buff[3], buff[4]);

    // 写远端写
    para_w param;
    memcpy(param.buff, buff, len);
    param.len = len;
    param.node = local_node;
    for(int i = 0; i < NODE_NUM; ++i) {
        if(i+1 == local_node || !strcmp("", ip_list[i])) {
            continue;
        }

        ret = rm_write_data(ip_list[i], &param);
        if (ret != 0) {
            INFO("rm_write_data error, node = %d, ret = %d", i+1, ret);
        }
    }

    // 再本地写
    ret = windowDataIn(win[local_node], buff, len);
    if (ret != 0) {
        INFO("windowDataIn error, ret = %d", ret);
    }
    INFO("write_data_local, node = %d, len = %d, buff = %c", local_node, len, buff[0]);

    return 0;
}

int dataOut() {
    int ret = 0;
    int len = 0;
    char buff[1024];
    // 先本地读，成功后再删远端
    ret = windowDataOut(win[local_node], buff, &len);
    if (ret != 0) {
        INFO("windowDataOut error, ret = %d", ret);
        return 1;
    }
    if(len != 0) {
        WRITE_FILE(buff, len);
        INFO("dataOut success");
        // 删除远端
        INFO("delete_data_local, node = %d, len = %d, buff = %c", local_node, len, buff[0]);
        para_d param;
        param.node = local_node;
        for(int i = 0; i < NODE_NUM; ++i) {
            if(i+1 == local_node || !strcmp("", ip_list[i])) {
                continue;
            }

            ret = rm_delete_data(ip_list[i], &param);
            if (ret != 0) {
                INFO("rm_delete_data error, node = %d, ret = %d", i+1, ret);
            }
        }

    } else {
        INFO("dataOut no file");
    }

    return 0;
}

int dataInLocal() {
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
    
    fgets(buff, len+1, (FILE*)fp);
    INFO("readfile: %c%c%c%c%c...", buff[0], buff[1], buff[2], buff[3], buff[4]);

    // 再本地写
    ret = windowDataIn(win[local_node], buff, len);
    if (ret != 0) {
        INFO("windowDataIn error, ret = %d", ret);
    }
    INFO("write_data_local, node = %d, len = %d, buff = %c", local_node, len, buff[0]);

    return 0;
}

int dataOutLocal() {
    int ret = 0;
    int len = 0;
    char buff[1024];
    // 先本地读，成功后再删远端
    ret = windowDataOut(win[local_node], buff, &len);
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

int dataInSSD() {
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
    
    fgets(buff, len+1, (FILE*)fp);
    INFO("readfile: %c%c%c%c%c...", buff[0], buff[1], buff[2], buff[3], buff[4]);

    ret = ssdDataIn(win[local_node], buff, len);
    if (ret != 0) {
        INFO("dataInSSD error, ret = %d", ret);
    }
    INFO("dataInSSD, node = %d, len = %d, buff = %c", local_node, len, buff[0]);

    return 0;
}

int dataOutSSD() {
    int ret = 0;
    int len = 0;
    char buff[1024];
    // 先本地读，成功后再删远端
    ret = ssdDataOut(win[local_node], buff, &len);
    if (ret != 0) {
        INFO("ssdDataOut error, ret = %d", ret);
        return 1;
    }
    if(len != 0) {
        WRITE_FILE(buff, len);
        INFO("dataOutSSD success");
    } else {
        INFO("dataOutSSD no file");
    }

    return 0;
}

int dataInMemory() {
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
    
    fgets(buff, len+1, (FILE*)fp);
    INFO("readfile: %c%c%c%c%c...", buff[0], buff[1], buff[2], buff[3], buff[4]);

    memcpy(Memory, buff, len);
    INFO("dataInMemory, node = %d, len = %d, buff = %c", local_node, len, buff[0]);

    return ret;
}

int dataOutMemory() {
    int ret = 0;
    int len = 1024;
    char buff[1024];
    // 先本地读，成功后再删远端
    memcpy(buff, Memory, len);
    if(len != 0) {
        WRITE_FILE(buff, len);
        INFO("dataOutMemory success");
    } else {
        INFO("dataOutMemory no file");
    }

    return ret;
}

int dataInOutTestDSM() {
    int ret = 0;
    clock_t begin, end;
    double cost;

    begin = clock();

    for(int i = 0; i < 5000; ++i) {
        ret = dataIn();
        if(ret != 0) {
            INFO("dataIn error, ret = %d", ret);
            return 1;
        }
        ret = dataOut();
        if(ret != 0) {
            INFO("dataOut error, ret = %d", ret);
            return 1;
        }
    }

    end = clock();

    cost = (double)(end - begin)/CLOCKS_PER_SEC;
    INFOEX("dataInOutTestDSM time cost is: %lf secs", cost);

    return 0;
}

TEST_F(GtestUt, dataInOutTestDSM)
{
    int ret = 0;
    clock_t begin, end;
    double cost;

    begin = clock(); // 注入TIMEPOINT点
    for(int i = 0; i < 5000; ++i) {
        ret = dataIn();
        EXPECT_EQ(ret, 0);
        ret = dataOut();
        EXPECT_EQ(ret, 0);
    }
    end = clock(); // 注入TIMEPOINT点

    cost = (double)(end - begin)/CLOCKS_PER_SEC;
    INFOEX("dataInOutTestDSM time cost is: %lf secs", cost);

    GlobalMockObject::verify();
}

int dataInOutTestMemory() {
    int ret = 0;
    clock_t begin, end;
    double cost;

    begin = clock();

    for(int i = 0; i < 5000; ++i) {
        ret = dataInMemory();
        if(ret != 0) {
            INFO("dataInMemory error, ret = %d", ret);
            return 1;
        }
        ret = dataOutMemory();
        if(ret != 0) {
            INFO("dataOutMemory error, ret = %d", ret);
            return 1;
        }
    }

    end = clock();

    cost = (double)(end - begin)/CLOCKS_PER_SEC;
    INFOEX("dataInOutTestMemory time cost is: %lf secs", cost);

    return 0;
}

int dataInOutTestLocal() {
    int ret = 0;
    clock_t begin, end;
    double cost;

    begin = clock();

    for(int i = 0; i < 5000; ++i) {
        ret = dataInLocal();
        if(ret != 0) {
            INFO("dataInOutTestLocal error, ret = %d", ret);
            return 1;
        }
        ret = dataOutLocal();
        if(ret != 0) {
            INFO("dataInOutTestLocal error, ret = %d", ret);
            return 1;
        }
    }

    end = clock();

    cost = (double)(end - begin)/CLOCKS_PER_SEC;
    INFOEX("dataInOutTestLocal time cost is: %lf secs", cost);

    return 0;
}

int dataInOutTestSSD() {
    int ret = 0;
    clock_t begin, end;
    double cost;

    begin = clock();

    for(int i = 0; i < 3000; ++i) {
        ret = dataInSSD();
        if(ret != 0) {
            INFO("dataInTest error, ret = %d", ret);
            return 1;
        }
        ret = dataOutSSD();
        if(ret != 0) {
            INFO("dataOutTest error, ret = %d", ret);
            return 1;
        }
    }

    end = clock();

    cost = (double)(end - begin)/CLOCKS_PER_SEC;
    INFOEX("dataInOutTestSSD time cost is: %lf secs", cost);

    return 0;
}

int dataExit() {
    int ret = 0;
    g_taskexit = 0;
    for(int i = 0; i < NODE_NUM; ++i) {
        ret = windowExit(win[i]);
        if(ret != 0) {
            INFO("windowExit error, i = %d", i);
            return 1;
        }
    }
    INFO("exit success");
    return ret;
}

int windowRev() {
    int ret = 0;
    para_rd param;
    param.node = local_node-1;
    for(int i = 0; i < NODE_NUM; ++i) {
        if(i+1 == local_node) {
            continue;
        }

        ret = rm_read_data(ip_list[i], &param);
        if (ret != 0) {
            INFO("rm_read_data fail,from node = %d, ret = %d", i+1, ret);
        } else {
            INFO("rm_read_data success,from node = %d, ret = %d", i+1, ret);
            break;
        }
    }
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
        {
            INFO("task in");
            ret = dataIn();
            // INFOEX("dataInOutTestMemory time cost is: 0.190000 secs");
            // INFOEX("dataInOutTestLocal time cost is: 0.230000 secs");
            // INFOEX("dataInOutTestDSM time cost is: 0.340000 secs");
            // INFOEX("dataInOutTestSSD time cost is: 0.440000 secs");
            // ret = dataInOutTestMemory();
            // sleep(2);
            // ret = dataInOutTestLocal();
            // sleep(2);
            // ret = dataInOutTestDSM();
            // sleep(2);
            // ret = dataInOutTestSSD();
            break;
        }

    
    case 1:
        {
            INFO("task out");
            ret = dataOut();
            break;
        }
    
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
    ret = getIp(&local_node);
    if(ret != 0) {
        INFO("getIp init error");
        return 1;
    }
    printf("node == %d\n", local_node);
    ret = rpcSvcInit();
    if(ret != 0) {
        INFO("rpc init error");
        return 1;
    }
    for(int i = 0; i < NODE_NUM; ++i) {
        ret = windowInit(&win[i]);
        if(ret != 0) {
            INFO("windowInit error, i = %d", i);
            return 1;
        }
        INFO("windowInit success, number%d win = %p", i, win[i]);
    }
    
    ret = windowRev();
    // if(ret != 0) {
    //     INFO("windowRev error");
    //     return 1;
    // }

    signal(SIGRTMIN, handleSig);
    signal(SIGRTMIN+1, handleSig);
    signal(SIGRTMIN+2, handleSig);
    // 启动守护线程(已弃用)
    // ret = daemon(1, 1);
    // if (ret != 0) {0
    //     INFO("daemon error");
    //     return 1;
    // }
    // 轮询执行
    polling();

    return 0;
}

#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void* arg){
    printf("child thread id: %ld\n", pthread_self());
    pthread_exit(NULL);
}

int main(){
    // 1. 创建子线程
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);
    if(ret != 0){
        char *errstr = strerror(ret);
        printf("error1: %s\n", errstr);
    }

    // 输出主线程和子线程 id
    printf("main thread id: %ld, tid: %ld\n", pthread_self(), tid);

    // 2. 设置子线程分离
    ret = pthread_detach(tid);
    if(ret != 0){
        char *errstr = strerror(ret);
        printf("error2: %s\n", errstr);
    }

    // 3. 尝试连接，失败
    ret = pthread_join(tid, NULL);
    if(ret != 0){
        char *errstr = strerror(ret);
        printf("error3: %s\n", errstr);
    }

    // 4. 主线程退出
    pthread_exit(NULL);

    return 0;
}
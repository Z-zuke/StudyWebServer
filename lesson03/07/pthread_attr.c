#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void * arg){
    printf("child thred id: %ld\n", pthread_self());
    return NULL;
}

int main(){
    // 创建线程属性变量
    pthread_attr_t attr;
    // 初始化属性变量
    pthread_attr_init(&attr);

    // 设置属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // 创建子进程
    pthread_t tid;
    int ret = pthread_create(&tid, &attr, callback, NULL);
    if(ret != 0){
        char *errstr = strerror(ret);
        printf("error1: %s\n", errstr);
    }

    // 获取线程的栈大小
    size_t size;
    pthread_attr_getstacksize(&attr, &size);
    printf("thread stack size: %ld\n", size);

    // 输出主线程和子线程的 id
    printf("main thread id: %ld, tid: %ld\n", pthread_self(), tid);

    // 释放线程属性资源
    pthread_attr_destroy(&attr);

    // 退出主线程
    pthread_exit(NULL);

    return 0;
}
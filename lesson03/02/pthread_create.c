#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void * callback(void* arg){
    printf("child[%d] thread...\n", *(int*)arg);
    return NULL;
}

int main(){
    pthread_t tid;

    // 1. 创建一个子进程
    int num = 5;
    for(int i=0; i<num; ++i){
        int ret = pthread_create(&tid, NULL, callback, (void*)&i);
        if(ret != 0){
            char* errstr = strerror(ret);
            printf("error: %s\n", errstr);
        }
        sleep(1);
    }
    
    for(int i=0; i<5; ++i){
        printf("%d\n", i);
    }
    sleep(6);

    return 0;
}
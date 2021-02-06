#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

void* callback(void* arg){
    printf("child[%d] thread id: %ld\n", *(int *)arg, pthread_self());
    sleep(2);
    return NULL;  // pthread_exit(NULL);
}

int main(){
    // 创建子线程
    pthread_t tid;
    int num = 5;
    for(int i=0; i<num; ++i){
        int ret = pthread_create(&tid, NULL, callback, (void*)&i);
        if (ret != 0){
            char* errstr = strerror(ret);
            printf("error: %s\n", errstr);
        }
        sleep(1);
    }

    // 主线程
    // 判断主线程和子线程 id 是否相等
    int ret = pthread_equal(pthread_self(), tid);
    if(ret != 0)
        printf("equal\n");
    else 
        printf("not equal\n");
    printf("main thread id: %ld, tid: %ld\n", pthread_self(), tid);
    
    // 主线程退出，不影响其他线程
    pthread_exit(NULL);
    printf("main thread exit\n");

    return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>


int value = 10;
void* callback(void* arg){
    printf("child thread id: %ld\n", pthread_self());
    sleep(2);
    pthread_exit((void *)&value);
}

int main(){
    // 创建子线程
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, callback, NULL);
    if (ret != 0){
        char* errstr = strerror(ret);
        printf("error: %s\n", errstr);
    }

    // 主线程
    // 判断主线程和子线程 id 是否相等
    ret = pthread_equal(pthread_self(), tid);
    if(ret != 0)
        printf("equal\n");
    else 
        printf("not equal\n");
    printf("main thread id: %ld, tid: %ld\n", pthread_self(), tid);

    // 主线程调用 join 回收子线程资源
    int * thread_retval;
    ret = pthread_join(tid, (void**)&thread_retval);
    if (ret != 0){
        char* errstr = strerror(ret);
        printf("error: %s\n", errstr);
    }
    printf("exit data: %d\n", *thread_retval);
    printf("receive success!\n");
    
    // 主线程退出，不影响其他线程
    pthread_exit(NULL);

    return 0;
}
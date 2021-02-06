/*
案例：8个线程操作同一个全局变量。
    3个线程不定时写这个全局变量，5个线程不定时的读这个全局变量
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


// 创建一个共享数据
int num = 1;
// 创建读写锁
pthread_rwlock_t rwlock;

void * writeNum(void *arg){
    while(1){
        pthread_rwlock_wrlock(&rwlock);
        num++;
        printf("write: ++ , tid: %ld, num: %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(1000);
    }
    return NULL;
}

void * readNum(void *arg){
    while(1){
        pthread_rwlock_rdlock(&rwlock);
        printf("read:  == , tid: %ld, num: %d\n", pthread_self(), num);
        pthread_rwlock_unlock(&rwlock);
        usleep(1000);
    }
    return NULL;
}

int main(){
    pthread_rwlock_init(&rwlock, NULL);

    // 创建 3 个写线程，5个读线程
    pthread_t wtids[3], rtids[5];
    for(int i = 0; i < 3; ++i){
        pthread_create(&wtids[i], NULL, writeNum, NULL);
    }

    for(int i = 0; i < 5; ++i){
        pthread_create(&rtids[i], NULL, readNum, NULL);
    }

    // 设置线程分离
    for(int i = 0; i < 3; ++i){
        pthread_detach(wtids[i]);
    }

    for(int i = 0; i < 5; ++i){
        pthread_detach(rtids[i]);
    }

    pthread_exit(NULL);
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
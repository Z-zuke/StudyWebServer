#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


int ticket = 100;

// 创建互斥量
pthread_mutex_t mutex;

void * sellticket(void *arg){
    // 买票
    while(1){
        // 加锁
        pthread_mutex_lock(&mutex);

        if(ticket>0){
            usleep(6000);
            printf("%ld is selling %dth ticket\n", pthread_self(), ticket);
            ticket--;
        } else {
            pthread_mutex_unlock(&mutex);
            break;
        }

        // 解锁
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}


int main(){
    // 初始化互斥量
    pthread_mutex_init(&mutex, NULL);

    // 创建 3 个子线程
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellticket, NULL);
    pthread_create(&tid2, NULL, sellticket, NULL);
    pthread_create(&tid3, NULL, sellticket, NULL);

    // 或者 设置线程分离
    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    // 退出主线程
    pthread_exit(NULL);

    // 释放互斥量资源
    pthread_mutex_destroy(&mutex);

    return 0;
}
/*
    使用多线程实现买票的案例。
    有3个窗口，一共是100张票。
*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


int ticket = 100;

void * sellticket(void *arg){
    while(ticket>0){
        usleep(6000);
        printf("%ld is selling %dth ticket\n", pthread_self(), ticket);
        ticket--;
    }
    return NULL;
}

int main(){
    // 创建 3 个子线程
    pthread_t tid1, tid2, tid3;
    pthread_create(&tid1, NULL, sellticket, NULL);
    pthread_create(&tid2, NULL, sellticket, NULL);
    pthread_create(&tid3, NULL, sellticket, NULL);

    // 回收子线程资源，阻塞
    // pthread_join(tid1, NULL);
    // pthread_join(tid2, NULL);
    // pthread_join(tid3, NULL);

    // 或者 设置线程分离
    pthread_detach(tid1);
    pthread_detach(tid2);
    pthread_detach(tid3);

    // 退出主线程
    pthread_exit(NULL);

    return 0;
}
/*
    生产者消费者模型
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>


// 创建一个互斥量
pthread_mutex_t mutex;
// 创建两个信号量
sem_t psem; // 生产者
sem_t csem; // 消费者

// 定义链表结构体
struct Node{
    int num;
    struct Node *next;
};
// 头结点
struct Node *head = NULL;

void * producer(void *arg){
    while (1) {
        sem_wait(&psem);
        pthread_mutex_lock(&mutex);
        
        struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->next = head;
        newNode->num = rand() % 100;
        head = newNode;
        printf("producer tid: %ld, addNode num: %d\n", pthread_self(), newNode->num);
        
        pthread_mutex_unlock(&mutex);        
        sem_post(&csem);
    }
    return NULL;
}

void * customer(void *arg){
    while(1){
        sem_wait(&csem);
        pthread_mutex_lock(&mutex);

        // 保存待删除节点的指针
        struct Node *delNode = head;
        head = head->next;
        printf("customer tid: %ld, delNode num: %d\n", pthread_self(), delNode->num);
        free(delNode);

        pthread_mutex_unlock(&mutex);
        sem_post(&psem);
    }
    return NULL;
}


int main(){
    // 初始化
    pthread_mutex_init(&mutex, NULL);
    sem_init(&psem, 0, 4);
    sem_init(&csem, 0, 0);

    // 创建 5 个生产者线程，5 个消费者线程
    pthread_t ptids[5], ctids[5];
    for(int i = 0; i < 5; ++i){
        pthread_create(&ptids[i], NULL, producer, NULL);
        pthread_create(&ctids[i], NULL, customer, NULL);
    }

    // 线程分离
    for(int i = 0; i < 5; ++i){
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while(1){
        sleep(2);
    }
    pthread_mutex_destroy(&mutex);
    sem_destroy(&psem);
    sem_destroy(&csem);

    pthread_exit(NULL);
    // printf("main thread exit...\n");

    return 0;
}
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


// 创建一个互斥量
pthread_mutex_t mutex;
// 创建条件变量
pthread_cond_t cond;

// 创建链表结构体
struct Node{
    int val;
    struct Node *next;
};

// 虚拟头结点
struct Node *head = NULL;


void *producer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->next = head;
        newNode->val = rand() % 100;
        head = newNode;
        printf("producer tid: %ld, addNode val: %d\n", pthread_self(), newNode->val);

        // 只要生产了，就通知消费者消费
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);

        usleep(10000);
    }
    return NULL;
}

void *customer(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 判断是否有数据
        if (head != NULL) {
            // 有数据，直接消费
            // 保存待删除结点的指针
            struct Node *delNode = head;
            head = head->next;
            printf("customer tid: %ld, delNode val: %d\n", pthread_self(), delNode->val);
            free(delNode);
            pthread_mutex_unlock(&mutex);
            usleep(1000);
        } else {
            // 没有数据，阻塞
            // 当条件函数调用阻塞的时候，会对互斥锁进行解锁;
            // 当不阻塞时，继续向下执行，会重新加锁。
            pthread_cond_wait(&cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

int main(){
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // 创建 5 个生产者，5 个消费者线程
    pthread_t ptids[5], ctids[5];
    for(int i = 0; i < 5; ++i) {
        pthread_create(&ptids[i], NULL, producer, NULL);
        pthread_create(&ctids[i], NULL, customer, NULL);
    }
    // 线程分离
    for(int i = 0; i < 5; ++i){
        pthread_detach(ptids[i]);
        pthread_detach(ctids[i]);
    }

    while(1){
        sleep(10);
    }

    // 销毁，释放资源
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    // 退出主线程
    pthread_exit(NULL);

    return 0;
}
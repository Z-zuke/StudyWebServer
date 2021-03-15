#ifndef LST_TIMER_H
#define LST_TIMER_H

#include <stdio.h>
#include <time.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 64
class util_timer;   // 前向声明

// 用户数据结构
struct client_data {
    sockaddr_in address;     // 客户端 scoket 地址
    int sockfd;             // socket 文件描述符
    char buf[BUFFER_SIZE];  // 读缓存
    util_timer* timer;      // 定时器
};

// 定时器类
class util_timer {
public:
    util_timer() : prev(NULL), next(NULL) {}

public:
    time_t expire;         // 任务超时时间，这里使用绝对时间
    void (*cb_func)(client_data*);  // 任务回调函数，由定时器的执行者传递客户数据给回调函数
    client_data* user_data;
    util_timer* prev;       // 指向前一个定时器
    util_timer* next;       // 指向后一个定时器
};

// 定时器类，是一个升序、双向链表，且带有头结点和尾节点
class sort_timer_lst {
public:
    sort_timer_lst() : head(NULL), tail(NULL) {}
    // 链表被销毁时，删除所有定时器
    ~sort_timer_lst() {
        util_timer* tmp = head;
        while (head) {
            head = tmp->next;
            delete tmp;
            tmp = head;
        }
    }

    // 将目标定时器 timer 添加到链表中
    void add_timer(util_timer* timer) {
        if (!timer) {
            return;
        }
        if (!head) {
            head = tail = timer;
            return;
        }
        
        //如果目标定时器的超时时间小于当前链表中所有定时器的超时时间，插入到链表头
        if (timer->expire < head->expire) {
            timer->next = head;
            head->prev = timer;
            head = timer;
            return;
        }
        add_timer(timer, head);
    }

    // 当某个定时任务发生变化，需要调整位置
    void adjust_timer(util_timer* timer) {
        if (!timer) {
            return;
        }

        util_timer* tmp = timer->next;
        // 如果被调整的目标定时器在链表尾部，或者该定时器新的超时时间仍然
        // 小于其下一个定时器的超时时间，则不用调整
        if (!tmp || (timer->expire < tmp->expire)) {
            return;
        }

        // 如果目标定时器是链表的头结点，则将该定时器从链表中取出并重新插入链表
        if (timer == head) {
            head = head->next;
            head->prev = NULL;
            timer->next = NULL;
            add_timer(timer, head);
        } else {
            // 如果不是头节点，则取出重新插入
            timer->prev->next = timer->next;
            timer->next->next = timer->prev;
            add_timer(timer, timer->next);
        }
    }

    // 将目标定时器 timer 从链表中删除
    void del_timer(util_timer* timer) {
        if (!timer) {
            return;
        }
        // 下面这个条件成立表示链表中只有一个定时器，即目标定时器
        if ((timer == head) && (timer == tail)) {
            delete timer;
            head = tail = NULL;
            return;
        }
        // 如果至少有两个定时器，且目标定时器为头结点
        if (timer == head) {
            head = head->next;
            head->prev = NULL;
            delete timer;
            return;
        }
        // 如果至少有两个定时器，且目标定时器是链表的尾节点
        if (timer == tail) {
            tail = tail->prev;
            tail->next = NULL;
            delete timer;
            return;
        }
        // 如果目标定时器位于链表中间
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        delete timer;
    }

    // SIGALARM 信号每次被触发就在其信号处理函数中执行一次 tick() 函数
    // 用来处理链表上到期的任务
    void tick() {
        if (!head) {
            return;
        }
        printf("timer tick\n");
        time_t cur = time(NULL);    // 获取当前时间
        util_timer* tmp = head;
        // 从头节点开始依次处理每个定时器，直到遇到一个尚未到期的定时器
        while (tmp) {
            // 因为每个定时器都使用绝对时间作为超时值，
            // 所以可以把定时器的超时值和系统当前时间比较，判断是否到期
            if (cur < tmp->expire) {
                break;
            }
            // 调用定时器的回调函数，以执行定时任务
            tmp->cb_func(tmp->user_data);
            // 执行完定时器中的定时任务后，就从链表中删除，并重置链表头结点
            head = tmp->next;
            if (head) {
                head->prev = NULL;
            }
            delete tmp;
            tmp = head;
        }
    }


private:
    // 插入定时器函数，被公有的 add_timer 函数和 adjust_timer 函数调用
    // 该函数表示将目标定时器 timer 添加到节点 lst_head 之后的部分链表中
    void add_timer(util_timer* timer, util_timer* lst_head) {
        util_timer* prev = lst_head;
        util_timer* tmp = prev->next;
        while (tmp) {
            if (timer->expire < tmp->expire) {
                prev->next = timer;
                timer->next = tmp;
                tmp->next = timer;
                timer->prev = prev;
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }
        // 如果大于所有节点，则插入链表尾部
        if (!tmp) {
            prev->next = timer;
            timer->prev = prev;
            timer->next = NULL;
            tail = timer;
        }
    }

private:
    util_timer* head;       // 头结点
    util_timer* tail;       // 尾节点
};



#endif
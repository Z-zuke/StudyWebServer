#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "locker.h"
#include "threadpool.h"
#include "http_conn.h"

#define MAX_FD 65535            // 最大的文件描述符个数
#define MAX_EVENT_NUMBER 10000  // 监听的最多的事件数量


// 向 epoll 中添加文件描述符
extern void addfd(int epollfd, int fd, bool one_shot);


// 注册信号捕捉
void addsig(int sig, void(handler)(int)) {
    struct sigaction sa;
    bzero(&sa, sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);  // 将信号集中的所有的标志位置为1，全部阻塞
    assert(sigaction(sig, &sa, NULL) != -1);
}


int main(int argc, char* argv[]) {
    if (argc <= 1) {
        printf("usage: %s port_number\n", basename(argv[0]));
        return 1;
    }

    int port = atoi(argv[1]);

    addsig(SIGPIPE, SIG_IGN);

    threadpool<http_conn> *pool = NULL;
    try {
        pool = new threadpool<http_conn>;
    } catch(...) {
        return 1;
    }

    http_conn* users = new http_conn[MAX_FD];

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);

    // 绑定服务器地址信息
    struct sockaddr_in saddr;
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 设置端口复用
    int reuse = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    // 绑定服务器
    int ret = bind(listenfd, (struct sockaddr*)&saddr, sizeof(saddr));

    // 监听
    ret = listen(listenfd, 5);

    // 创建 epoll 实例，事件数组
    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    // 将监听事件添加到 epoll 实例中
    addfd(epollfd, listenfd, false);
    http_conn::m_epollfd = epollfd;

    while (true) {
        int number = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0) {
            if (errno != EINTR) {
                printf("epoll fail\n");
                break;
            }
        }

        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            
            if (sockfd == listenfd) {
                struct sockaddr_in client_addr;
                socklen_t client_addrLength = sizeof(client_addr);

                int connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addrLength);
                if (connfd < 0) {
                    printf("errno is: %d\n", errno);
                    continue;
                }

                if (http_conn::m_user_count >= MAX_FD) {
                    // TODO: 回复客户端：服务器正忙，稍后连接
                    close(connfd);
                    continue;
                }
                users[connfd].init(connfd, client_addr);

            } else if (events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                // 处理错误事件
                users[sockfd].close_conn();

            } else if (events[i].events & EPOLLIN) {
                // 处理接受数据事件
                if (users[sockfd].read()) {
                    pool->append(users + sockfd);
                } else {
                    users[sockfd].close_conn();
                }

            } else if (events[i].events & EPOLLOUT) {
                // 处理写数据事件
                if (!users[sockfd].write()) {
                    users[sockfd].close_conn();
                }
            }
        }
    }
    close(epollfd);
    close(listenfd);
    delete [] users;
    delete pool;

    return 0;
}
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>


int main() {
    // 创建 socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    // 创建 epoll 实例
    int epfd = epoll_create(100);

    // 将监听文件描述符相关信息添加到实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    // 用于接受检测后的数据
    struct epoll_event epev_list[1024];
    while (1) {
        // epoll 检测
        int ret = epoll_wait(epfd, epev_list, 1024, -1);
        if (ret == -1) {
            perror("epoll_wait");
            exit(-1);
        }
        printf("event num = %d\n", ret);

        // 遍历事件
        for (int i = 0; i < ret; ++i) {
            int curfd = epev_list[i].data.fd;
            if (curfd == lfd) {
                // 监听文件描述符有数据到达，说明有客户端连接
                struct sockaddr_in clientAddr;
                int len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
                // 打印客户端信息
                char clientIP[16];
                inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
                unsigned short clientPort = ntohs(clientAddr.sin_port);
                printf("client ip: %s, port: %d\n", clientIP, clientPort);

                // 添加到 epfd 监测实例中
                epev.events = EPOLLIN;  // 默认水平触发 LT 模式
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            } else {
                // 有数据到达，需要通信
                if (epev_list[i].events & EPOLLIN) {
                    char buf[5] = {0};
                    int len = read(curfd, buf, sizeof(buf));  // read data
                    if (len == -1) {
                        perror("read");
                        exit(-1);
                    } else if (len == 0) {
                        printf("client[%d] closed...\n", curfd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                        close(curfd);
                    } else if (len > 0) {
                        // printf("Server received: %s\n", buf);  // 格式有点乱？
                        write(STDOUT_FILENO, buf, len);
                        write(curfd, buf, strlen(buf) + 1);  // write data
                    }
                }
            }
        }
    }
    close(epfd);
    close(lfd);

    return 0;
}
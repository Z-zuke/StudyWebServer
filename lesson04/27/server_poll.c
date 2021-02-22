#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>


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

    // 初始化检测的文件描述符数组
    struct pollfd fds[1024];
    for (int i = 0; i < 1024; ++i) {
        fds[i].fd = -1;
        fds[i].events = POLLIN;  // 监测读事件
    }
    fds[0].fd = lfd;
    int nfds = 0;

    while (1) {
        // 调用 poll，检测哪些文件描述符有数据 (第一次遍历)
        int ret = poll(fds, nfds+1, -1);
        if (ret == -1) {
            perror("poll");
            exit(-1);
        } else if (ret == 0) {
            // 仅当设置超时时间，且在时间内没有检测到描述符修改
            continue;
        } else if (ret > 0) {
            // 说明检测到了有文件描述符对应的缓冲区数据发生了改变
            if (fds[0].revents & POLLIN) {
                // 有新的客户端连接进来
                struct sockaddr_in clientAddr;
                int len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
                // 打印客户端信息
                char clientIP[16];
                inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientAddr));
                unsigned short clientPort = ntohs(clientAddr.sin_port);
                printf("client ip: %s, port: %d\n", clientIP, clientPort);

                // 将新的文件描述符添加到集合中
                for (int i = 1; i < 1024; ++i) {
                    if (fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;
                        break;
                    }
                }

                // 更新最大的文件描述符的索引
                nfds = nfds > cfd ? nfds : cfd;

            }

            // 接收和发送数据  (第二次遍历)
            for (int i = 1; i <= nfds; ++i) {
                if (fds[i].revents & POLLIN) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    char buf[1024] = {0};
                    int len = recv(fds[i].fd, buf, sizeof(buf), 0);  // receive
                    if (len == -1) {
                        perror("recv");
                        exit(-1);
                    } else if (len == 0) {
                        // 客户端关闭
                        printf("client [%d]  closed...\n", fds[i].fd);
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    } else if (len > 0) {
                        printf("Server received: %s\n", buf);
                        send(fds[i].fd, buf, strlen(buf) + 1, 0);  // send
                    }
                }
            }
        }
    }
    close(lfd);

    return 0;
}
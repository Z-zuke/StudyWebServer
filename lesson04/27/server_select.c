#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>


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

    // 创建 fd_set 集合，存放需要检测的文件描述符
    fd_set rfdSet, rtmpSet;
    FD_ZERO(&rfdSet);
    FD_SET(lfd, &rfdSet);
    int maxfd = lfd;

    while (1) {
        rtmpSet = rfdSet;  // rtmpSet 用于提供给内核修改

        // 调用 select, 让内核检测哪些文件描述符有数据 (第一次遍历)
        int ret = select(maxfd+1, &rtmpSet, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select");
            exit(-1);
        } else if (ret == 0) {
            // 仅当设置超时时间，且在时间内没有检测到描述符修改
            continue;
        } else if (ret > 0) {
            // 说明检测到了有文件描述符对应的缓冲区数据发生了改变
            if (FD_ISSET(lfd, &rtmpSet)) {
                // 表述有新的客户端连接进来
                struct sockaddr_in clientAddr;
                int len = sizeof(clientAddr);
                int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
                // 打印客户端信息
                char clientIP[16];
                inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientAddr));
                unsigned short clientPort = ntohs(clientAddr.sin_port);
                printf("client ip: %s, port: %d\n", clientIP, clientPort);

                // 将新的文件描述符加入到集合中
                FD_SET(cfd, &rfdSet);

                // 更新最大的文件描述符
                maxfd = maxfd > cfd ? maxfd : cfd;
            }

            // 接收和发送数据  (第二次遍历)
            for (int i = lfd + 1; i <= maxfd; ++i) {
                if (FD_ISSET(i, &rtmpSet)) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    char buf[1024] = {0};
                    int len = recv(i, buf, sizeof(buf), 0); // receive
                    if (len == -1) {
                        perror("recv");
                        exit(-1);
                    } else if (len == 0) {
                        // 客户端关闭
                        printf("client [%d]  closed...\n", i);
                        close(i);
                        FD_CLR(i, &rfdSet);
                    } else if (len > 0) {
                        printf("Server received: %s\n", buf);
                        send(i, buf, strlen(buf) + 1, 0);  // send
                    }
                }
            }
        }
    }
    close(lfd);

    return 0;
}
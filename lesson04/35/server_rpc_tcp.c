#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>

/* 使用数据报 TCP 方式的本地套接字, AF_LOCAL, SOCK_STREAM */
int main() {
    // 删除套接字文件，防止占用
    unlink("server.sock");

    // 创建监听套接字
    int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // 绑定本地套接字文件
    struct sockaddr_un saddr;
    saddr.sun_family = AF_LOCAL;
    strcpy(saddr.sun_path, "server.sock");
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 监听
    listen(lfd, 100);

    // 等待客户端连接
    struct sockaddr_un clientAddr;
    int len = sizeof(clientAddr);
    int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
    if (cfd == -1) {
        perror("accept");
        exit(-1);
    }
    printf("client sockt filename: %s\n", clientAddr.sun_path);

    // 通信
    while (1) {
        char buf[128];
        int len = recv(cfd, buf, sizeof(buf), 0);
        if (len == -1) {
            perror("recv");
            exit(-1);
        } else if (len == 0) {
            printf("client closed...\n");
            break;
        } else if (len > 0) {
            printf("Server received: %s\n", buf);
            send(cfd, buf, len, 0);
        }
    }
    close(cfd);
    close(lfd);

    return 0;
}
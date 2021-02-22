#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


int main() {
    // 创建 socket
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;
    // 绑定
    bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 通信
    while (1) {
        struct sockaddr_in clientAddr;
        int len = sizeof(clientAddr);
        // 打印客户端信息
        char clientIP[16];
        inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
        unsigned short clientPort = ntohs(clientAddr.sin_port);
        printf("client IP: %s, Port: %d\n", clientIP, clientPort);

        // 接收数据
        char buf[128] = {0};
        recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &len);
        printf("Server received: %s\n", buf);
        // 发送数据 (回射)
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&clientAddr, len);
    }
    close(fd);

    return 0;
}
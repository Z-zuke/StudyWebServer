#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


int main() {
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = PF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_port = htons(9999);

    // 设置端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    ret = listen(lfd, 8);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
    if (cfd == -1) {
        perror("accept");
        exit(-1);
    }

    char clientIP[16];
    inet_ntop(PF_INET, &clientAddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(clientAddr.sin_port);
    printf("client ip: %s, port: %d\n", clientIP, clientPort);

    char recvBuf[1024] = {0};
    while (1) {
        int len = recv(cfd, recvBuf, sizeof(recvBuf), 0);
        if (len == -1) {
            perror("recv");
            exit(-1);
        } else if (len == 0) {
            printf("client closed...\n");
        } else if (len > 0) {
            printf("read buf = %s\n", recvBuf);
        }

        for (int i = 0; i < len; ++i) {
            recvBuf[i] = toupper(recvBuf[i]);
        }
        printf("convert buf = %s\n", recvBuf);

        // 大写字符串发送给客户端
        ret = send(cfd, recvBuf, strlen(recvBuf) + 1, 0);
        
        if (ret == -1) {
            perror("send");
            exit(-1);
        }
    }
    close(cfd);
    close(lfd);

    return 0;
}
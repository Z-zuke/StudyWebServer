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

    // 设置多播属性，设置组播接口
    struct in_addr imr_multiaddr;
    // 初始化多播地址
    inet_pton(AF_INET, "239.0.0.10", &imr_multiaddr.s_addr);
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &imr_multiaddr, sizeof(imr_multiaddr));


    // 初始化组播地址信息
    struct sockaddr_in multiAddr;
    multiAddr.sin_family = AF_INET;
    multiAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "239.0.0.10", &multiAddr.sin_addr.s_addr);

    // 组播通信
    int num = 0;
    while (1) {
        char buf[128] = {0};
        sprintf(buf, "hello, send num = %d\n", num++);
        // 发送数据
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&multiAddr, sizeof(multiAddr));
        printf("Multi-Broadcast data: %s\n", buf);

        sleep(1);
    }
    close(fd);

    return 0;
}
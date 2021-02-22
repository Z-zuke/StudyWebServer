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

    // 客户端绑定本地 IP 和接收组播信息端口
    struct sockaddr_in bAddr;
    bAddr.sin_family = AF_INET;
    bAddr.sin_port = htons(9999);
    bAddr.sin_addr.s_addr = INADDR_ANY;
    // 绑定
    bind(fd, (struct sockaddr *)&bAddr, sizeof(bAddr));

    // 加入多播组，初始化属性
    struct ip_mreq op;
    inet_pton(AF_INET, "239.0.0.10", &op.imr_multiaddr.s_addr);
    op.imr_interface.s_addr = INADDR_ANY;
    setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &op, sizeof(op));

    // 接收广播信息
    while (1) {
        char buf[128] = {0};
        // 接收信息
        recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("Receive from Server: %s\n", buf);
    }
    close(fd);

    return 0;
}
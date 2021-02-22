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

    // 设置广播属性
    int optval = 1;  // 表示允许广播
    setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));

    // 创建广播地址
    struct sockaddr_in broadAddr;
    broadAddr.sin_family = AF_INET;
    broadAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "192.168.254.255", &broadAddr.sin_addr.s_addr);

    // 广播通信
    int num = 0;
    while (1) {
        char buf[128] = {0};
        sprintf(buf, "hello, send num = %d\n", num++);
        // 发送数据
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&broadAddr, sizeof(broadAddr));
        printf("Broadcast data: %s\n", buf);

        sleep(1);
    }
    close(fd);

    return 0;
}
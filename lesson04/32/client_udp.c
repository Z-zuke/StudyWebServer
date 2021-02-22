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

    // 服务器地址信息
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);

    int num = 0;
    while (1) {
        // 发送数据
        char buf[128] = {0};
        sprintf(buf, "hello, send num = %d\n", num++);
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        // 接收数据
        recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        printf("Client received: %s\n", buf);

        sleep(1);
    }
    close(fd);

    return 0;
}
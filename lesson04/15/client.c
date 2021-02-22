/* TCP 通信的客户端  */

#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main() {
    // 1. 创建套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    // 2. 连接服务器
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.254.129", &serverAddr.sin_addr.s_addr);
    serverAddr.sin_port = htons(9999);
    int ret = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret == -1) {
        perror("connect");
        exit(-1);
    }

    // 3. 通信
    char recvBuf[1024] = {0};
    while (1) {
        char *data = "hello, i am client";
        // 给服务器端发送数据
        write(fd, data, strlen(data));
        sleep(1);

        // 接收服务器端数据
        int len = read(fd, recvBuf, sizeof(recvBuf));
        if (len == -1) {
            perror("read");
            exit(-1);
        } else if (len > 0) {
            printf("received server data: %s\n", recvBuf);
        } else if (len == 0) {
            // 表示服务器端断开连接
            printf("server closed...\n");
            break;
        }
    }

    // 4. 关闭连接
    close(fd);
}
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main() {
    // 创建 socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    int num = 0;
    while (1) {
        char buf[1024] = {0};
        sprintf(buf, "send data %d", num++);
        send(fd, buf, strlen(buf), 0);  // send

        int len = recv(fd, buf, sizeof(buf), 0); // receive
        if (len == -1) {
            perror("receive");
            exit(-1);
        } else if (len == 0) {
            printf("server closed...\n");
            break;
        } else if (len > 0) {
            printf("Client received: %s\n", buf);
        }
        // sleep(1);
        usleep(2000);
    }
    close(fd);

    return 0;
}
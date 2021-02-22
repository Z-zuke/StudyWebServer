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

    while (1) {
        char buf[1024] = {0};
        fgets(buf, sizeof(buf), stdin);
        write(fd, buf, strlen(buf) + 1);  // send

        int len = read(fd, buf, sizeof(buf));  // receive
        if (len == -1) {
            perror("read");
            exit(-1);
        } else if (len == 0) {
            printf("server closed...\n");
            break;
        } else if (len > 0) {
            printf("Client received: %s\n", buf);
        }
    }
    close(fd);

    return 0;
}
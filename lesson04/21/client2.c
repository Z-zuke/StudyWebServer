#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, "192.168.254.129", &serverAddr.sin_addr.s_addr);
    serverAddr.sin_port = htons(9999);
    int ret = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret == -1) {
        perror("connect");
        exit(-1);
    }

    char recvBuf[1024];
    int i = 0;
    while (1) {
        sprintf(recvBuf, "data: %d\n", ++i);
        write(fd, recvBuf, sizeof(recvBuf));

        int len = read(fd, recvBuf, sizeof(recvBuf));
        if (len == -1) {
            perror("read");
            exit(-1);
        } else if (len > 0) {
            printf("received server: %s\n", recvBuf);
        } else if (len == 0) {
            printf("server closed...\n");
            break;
        }

        sleep(1);
    }
    close(fd);

    return 0;
}
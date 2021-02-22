#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int main() {
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serverAddr;
    inet_pton(PF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(9999);

    int ret = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }

    while (1) {
        char sendBuf[1024] = {0};
        fgets(sendBuf, sizeof(sendBuf), stdin);
        write(fd, sendBuf, strlen(sendBuf) + 1);

        int len = read(fd, sendBuf, sizeof(sendBuf));
        if (len == -1) {
            perror("read");
            return -1;
        } else if (len > 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("server closed...\n");
            break;
        }
    }
    close(fd);

    return 0;
}
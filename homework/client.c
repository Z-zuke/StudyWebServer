#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#define MAXLINE 1024


int main() {
    // 创建 socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
    }

    // 服务器地址信息
    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);

    // 连接服务器
    int connect_rt = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (connect_rt < 0) {
        perror("connect");
    }

    // 创建 fd_set 集合，使用 select 同时处理标准输入和套接字
    fd_set readmask, allreads;
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);  // 处理标准输入
    FD_SET(fd, &allreads); // 处理 socket 输入

    char recvBuf[MAXLINE], sendBuf[MAXLINE];
    int n;
    while (1) {
        readmask = allreads;
        int ret = select(fd + 1, &readmask, NULL, NULL, NULL);
        if (ret <= 0) {
            perror("select");
            exit(-1);
        }
        // 处理套接字输入
        if (FD_ISSET(fd, &readmask)) {
            n = read(fd, recvBuf, MAXLINE);
            if (n < 0) {
                perror("read");
                exit(-1);
            } else if (n == 0) {
                printf("server closed...\n");
                break;
            }
            // fputs 把字符串写入到指定的流 stream 中，但不包括空字符
            recvBuf[n] = 0;
            fputs(recvBuf, stdout);
            fputs("\n", stdout);
        }

        // 处理标准输入
        if (FD_ISSET(STDIN_FILENO, &readmask)) {
            if (fgets(sendBuf, MAXLINE, stdin) != NULL) {
                int i = strlen(sendBuf);
                if (sendBuf[i-1] == '\n') {
                    sendBuf[i-1] = 0;
                }
                // 处理输入为 quit，关闭套接字
                if (strncmp(sendBuf, "quit", strlen(sendBuf)) == 0) {
                    if (shutdown(fd, SHUT_WR)) {
                        perror("shutdown");
                        exit(-1);
                    }
                }
                // 发送数据
                int w_rt = write(fd, sendBuf, strlen(sendBuf));
                if (w_rt < 0) {
                    perror("write");
                    exit(-1);
                }
            }
        }
    }
    close(fd);

    return 0;
}
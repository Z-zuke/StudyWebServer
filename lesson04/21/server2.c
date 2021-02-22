/* 多进程服务器 */
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>


void recycleChild(int arg) {
    while (1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if (ret == -1) {
            // 所有子进程都回收了
            printf("recycled all child process...\n");
            break;
        } else if (ret == 0) {
            // 还有子进程存活
            break;
        } else if (ret > 0) {
            // 回收子进程
            printf("child process %d recycled\n", ret);
        }
    }
}

int main() {
    // 利用信号捕捉回收子进程资源
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = recycleChild;
    sigemptyset(&act.sa_mask);
    // 注册信号捕捉
    sigaction(SIGCHLD, &act, NULL);

    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;  // 0.0.0.0
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    ret = listen(lfd, 128);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }

    while (1) {
        struct sockaddr_in clientAddr;
        int len = sizeof(clientAddr);
        int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
        if (cfd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            exit(-1);
        }

        pid_t pid = fork();
        if (pid == 0) {
            // 子进程
            char clientIP[16];
            inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP ,sizeof(clientAddr));
            unsigned short clientPort = ntohs(clientAddr.sin_port);
            printf("client ip: %s, port: %d\n", clientIP, clientPort);

            char recvBuf[1024];
            while (1) {
                int len = read(cfd, &recvBuf, sizeof(recvBuf));
                if (len == -1) {
                    perror("read");
                    exit(-1);
                } else if (len > 0) {
                    printf("received client: %s\n", recvBuf);
                } else if (len == 0) {
                    printf("client closed...\n");
                    break;
                }

                write(cfd, recvBuf, strlen(recvBuf) + 1);
            }
            close(cfd);
            exit(0);  // 退出当前子进程
        }
    }
    close(lfd);

    return 0;
}
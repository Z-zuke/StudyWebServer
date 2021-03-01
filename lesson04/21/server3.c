/* 多线程服务器 */
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


struct sockInfo {
    int fd;  // 通信的文件描述符
    pthread_t tid;  // 线程号
    struct sockaddr_in addr;
};

struct sockInfo sockinfos[128];  // 线程池

void* working(void* arg) {
    // 子线程和客户端通信，获取客户端信息
    struct sockInfo *pinfo = (struct sockInfo *)arg;

    char clientIP[16];
    inet_ntop(PF_INET, &pinfo->addr.sin_addr.s_addr, clientIP, sizeof(clientIP));
    unsigned short clientPort = ntohs(pinfo->addr.sin_port);
    printf("client ip: %s, port: %d\n", clientIP, clientPort);

    // 接收客户端发来的数据
    char recvBuf[1024];
    while (1) {
        int len = read(pinfo->fd, &recvBuf, sizeof(recvBuf));
        if (len == -1) {
            perror("read");
            exit(-1);
        } else if (len > 0) {
            printf("received client: %s\n", recvBuf);
        } else if (len == 0) {
            printf("client closed...\n");
            break;
        }

        write(pinfo->fd, recvBuf, strlen(recvBuf) + 1);
    }
    close(pinfo->fd);
    return NULL;
}

int main() {
    // 创建 socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);  // AF_INET=PF_INET
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 监听
    ret = listen(lfd, 128);
    if (ret == -1) {
        perror("listen");
        exit(-1);
    }


    // 初始化结构体
    int max = sizeof(sockinfos) / sizeof(sockinfos[0]);
    for (int i = 0; i < max; ++i) {
        bzero(&sockinfos[i], sizeof(sockinfos[i]));
        sockinfos[i].fd = -1;
        sockinfos[i].tid = -1;
    }

    while (1) {
        struct sockaddr_in clientAddr;
        int len = sizeof(clientAddr);
        // 接受连接
        int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);

        struct sockInfo *pinfo;

        for (int i = 0; i < max; ++i) {
            // 从这个数组中找到一个可以用的 sockInfo 
            if (sockinfos[i].fd == -1) {
                pinfo = &sockinfos[i];
                break;
            }
            if (i == max - 1) {
                sleep(1);
                --i;
            }
        }

        pinfo->fd = cfd;
        memcpy(&pinfo->addr, &clientAddr, len);

        // 创建子线程
        pthread_create(&pinfo->tid, NULL, working, pinfo);
        pthread_detach(pinfo->tid);
    }
    close(lfd);

    return 0;
}
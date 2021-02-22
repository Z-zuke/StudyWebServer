#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/un.h>


int main() {
    // 删除套接字文件，防止占用
    unlink("client.sock");
    
    // 创建套接字
    int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    // ! 绑定本地套接字文件  ---> 需要
    // 需要指定一个本地路径，以便在服务器端回包时，可以正确地找到地址
    struct sockaddr_un saddr;
    saddr.sun_family = AF_LOCAL;
    strcpy(saddr.sun_path, "client.sock");
    int ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 连接服务器
    struct sockaddr_un serverAddr;
    serverAddr.sun_family = AF_LOCAL;
    strcpy(serverAddr.sun_path, "server.sock");

    // int ret = connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    // if (ret == -1) {
    //     perror("connect");
    //     exit(-1);
    // }

    // 通信
    int num = 0;
    while (1) {
        char buf[128] = {0};
        //发送数据
        sprintf(buf, "send num = %d\n", num++);
        sendto(fd, buf, strlen(buf) + 1, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

        // 接收数据
        int len = recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL);
        if (len == -1) {
            perror("recv");
            exit(-1);
        } else if (len == 0) {
            printf("server closed...\n");
            break;
        } else if (len > 0) {
            printf("Client received: %s\n", buf);
        }

        sleep(1);
    }
    close(fd);

    return 0;
}
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define  LISTENQ  1024
static int count;  // 连接数


char *run_cmd(char *cmd) {
    char *data = malloc(16384);
    bzero(data, sizeof(data));
    FILE *fdp;
    const int max_buffer = 256;
    char buffer[max_buffer];
    fdp = popen(cmd, "r");
    char *dataIndex = data;
    if (fdp) {
        while (!feof(fdp)) {
            if (fgets(buffer, max_buffer, fdp) != NULL) {
                int len = strlen(buffer);
                memcpy(dataIndex, buffer, len);
                dataIndex += len;
            }
        }
        pclose(fdp);
    }
    return data;

}


int main() {
    // 创建监听套接字
    int lfd = socket(AF_INET, SOCK_STREAM, 0);

    // 设置服务器信息
    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9999);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 设置端口复用
    int op = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op));
    // 绑定服务器
    int ret = bind(lfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (ret < 0) {
        perror("bind");
        exit(-1);
    }

    // 监听
    ret = listen(lfd, LISTENQ);
    if (ret < 0) {
        perror("listen");
        exit(-1);
    }

    // 信号处理
    signal(SIGPIPE, SIG_IGN);

    // 客户端信息
    struct sockaddr_in clientAddr;
    int len = sizeof(clientAddr);

    char buf[256];
    while (1) {
        int cfd = accept(lfd, (struct sockaddr *)&clientAddr, &len);
        if (cfd < 0) {
            perror("accept");
            exit(-1);
        }
        // 打印连接客户端信息
        char clientIP[16];
        inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIP, len);
        unsigned short clientPort = ntohs(clientAddr.sin_port);
        printf("client ip: %s, port: %d\n", clientIP, clientPort);

        while (1) {
            bzero(buf, sizeof(buf));
            int n = read(cfd, buf, sizeof(buf));
            if (n < 0) {
                perror("read");
            } else if (n == 0) {
                printf("client[%d] closed...\n", cfd);
                close(cfd);
                break;
            }
            count++;
            buf[n] = 0;
            printf("Server received: %s\n", buf);
            // 处理命令
            if (strncmp(buf, "ls", n) == 0) {
                char *result = run_cmd("ls");
                if (send(cfd, result, strlen(result), 0) < 0) {
                    return 1;
                }
                free(result);
            } else if (strncmp(buf, "pwd", n) == 0) {
                char *result = getcwd(buf, sizeof(buf));
                if (send(cfd, result, strlen(result), 0) < 0) {
                    return 1;
                }
                free(result);
            } else if (strncmp(buf, "cd", 3) == 0) {
                char target[256];
                bzero(target, sizeof(target));
                memcpy(target, buf + 3, strlen(buf) - 3);
                if (chdir(target) == -1) {
                    printf("change dir failed, %s\n", target);
                }
            } else {
                char *error = "error: unknow input type";
                if (send(cfd, error, strlen(error), 0) < 0) {
                    return 1;
                }
            }
        }
    }
    close(lfd);

    return 0;
}
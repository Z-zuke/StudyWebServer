#include<sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    // 在 fork 之前创建管道
    int pipefd[2];
    int ret = pipe(pipefd);
    if(ret==-1){
        perror("pipe");
        exit(0);
    }
    // 创建子进程
    pid_t pid = fork();
    if(pid>0){
        // 父进程
        printf("parent process, pid: %d\n",getpid());

        // 关闭写端
        close(pipefd[1]);

        char buf[1024] = {0};
        while(1){
            // 从管道读数据
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("parent Receive: %s, pid: %d\n", buf, getpid());
            // 向管道写数据
            char *str = "parent --> child";
            write(pipefd[1], str, strlen(str));
            sleep(1);
        }
    }
    else if(pid==0){
        // 子进程
        printf("child process, pid: %d\n", getpid());

        // 关闭读端
        close(pipefd[0]);

        char buf[1024] = {0};
        while(1){
            // 向管道写数据
            char *str = "child --> parent";
            write(pipefd[1], str, strlen(str));
            sleep(1);
            // 从管道读数据
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("child Receive: %s, pid: %d\n", buf, getpid());
            // 清除管道数据
            bzero(buf, sizeof(buf));
        }
    }

    return 0;
}
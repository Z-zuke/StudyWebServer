/*
    实现 cat ipc2.c | wc -l 父子进程间通信
    
    子进程： cat ipc2.c, 子进程结束后，将数据发送给父进程
    父进程：获取到数据，统计
    pipe()
    execlp()
    子进程将标准输出 stdout_fileno 重定向到管道的写端。  dup2
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

int main()
{
    // 保存终端输出的文件描述符，用于恢复
    int sfd;
    sfd = dup(STDOUT_FILENO);

    // 创建管道
    int fd[2];
    // 保存写端描述符
    int fd1 = dup(fd[1]);
    // 保存读端描述符
    int fd0 = dup(fd[0]);

    int ret = pipe(fd);
    if (ret == -1)
    {
        perror("pipe");
        exit(0);
    }

    // 创建子进程
    pid_t pid = fork();
    if (pid > 0)
    {
        // 父进程
        // 关闭写端
        close(fd[1]);
        
        // 从管道中读取
        char buf[1024] = {0};
        int len = 0;
        while ((len = read(fd[0], buf, sizeof(buf) - 1)) > 0)
        {
            // 统计输出失败，wc 只能自己实现
            // execlp("wc", "wc", "-l", buf, NULL);
            // perror("execlp wc");
            printf("%s\n", buf);
            bzero(buf, sizeof(buf));
        }
        wait(NULL);
    }
    else if (pid == 0)
    {
        // 子进程
        // 关闭读端
        close(fd[0]);
        // 文件描述符重定向 stdout_fileno -> fd[1]
        dup2(fd[1], STDOUT_FILENO);
        // 执行 cat ipc2.c
        execlp("cat", "cat", "ipc2.c", NULL);
        perror("execlp cat");
    }
    else
    {
        perror("fork");
        exit(0);
    }

    return 0;
}
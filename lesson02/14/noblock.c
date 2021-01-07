/*
    设置管道非阻塞
    int flags = fcntl(fd[0], F_GETFL);  // 获取原来的flag
    flags |= O_NONBLOCK;            // 修改flag的值
    fcntl(fd[0], F_SETFL, flags);   // 设置新的flag
*/

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main()
{
    int pipefd[2];
    int ret = pipe(pipefd);
    if (ret == -1)
    {
        perror("pipe");
        exit(0);
    }

    pid_t pid = fork();
    if (pid > 0)
    {
        printf("parent process, pid: %d\n", getpid());
        close(pipefd[1]); // 关闭写端
        char buf[1024] = {0};

        int flags = fcntl(pipefd[0], F_GETFL);
        flags |= O_NONBLOCK;              // 修改 flags 值,添加非阻塞
        fcntl(pipefd[0], F_SETFL, flags); // 设置新的 flags 值

        while (1)
        {
            int len = read(pipefd[0], buf, sizeof(buf));
            printf("len: %d\n", len);
            printf("parent receive: %s, pid: %d\n", buf, getpid());
            bzero(buf, sizeof(buf));
            sleep(1);
        }
    }
    else if (pid == 0)
    {
        printf("child process, pid: %d\n", getpid());
        close(pipefd[0]); // 关闭读端
        char buf[1024] = {0};
        while (1)
        {
            char *str = "child --> parent";
            write(pipefd[1], str, strlen(str));
            sleep(5);
        }
    }
    else
    {
        perror("fork");
        exit(0);
    }

    return 0;
}
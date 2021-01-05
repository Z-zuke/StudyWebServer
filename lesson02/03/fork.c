#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void isParent(pid_t pid, int num)
{
    if (pid > 0)
    {
        printf("parent process, pid: %d, ppid: %d\n", getpid(), getppid());
        printf("parent num: %d\n", num); // share on read
        num += 10;
        printf("parent num+=10 : %d\n", num); // copy on write
    }
    else if (pid == 0)
    {
        printf("child process, pid: %d, ppid: %d\n", getpid(), getppid());
        printf("child num: %d\n", num);
        num += 100;
        printf("child num+=100 : %d\n", num);
    }
}

int main()
{
    int num = 10;
    // 1.创建子进程
    pid_t pid = fork();

    // 2.判断父/子进程
    isParent(pid, num);

    for (int i = 0; i < 3; ++i)
    {
        printf("i: %d, pid: %d, ppid: %d\n", i, getpid(), getppid());
        sleep(1);
    }

    return 0;
}
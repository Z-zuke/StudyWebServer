/*
    写一个守护进程，每隔2s获取一下系统时间，将这个时间写入到磁盘文件中。
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

void work(int num){
    // 捕捉到信号之后，获取系统时间，写入磁盘文件
    time_t tm = time(NULL);
    struct tm *loc = localtime(&tm);

    char *str = asctime(loc);
    int fd = open("time.txt", O_CREAT|O_RDWR|O_APPEND, 0664);
    write(fd, str, strlen(str));
    close(fd);
}

int main(){
    // 1.创建子进程
    pid_t pid = fork();
    if(pid>0){
        exit(0);
    }

    // 2.将子进程重新创建一个会话
    setsid();

    // 3.设置掩码
    umask(022);

    // 4.更改工作目录
    chdir("/home/ronnyz/Documents/Project/lesson02/31/");

    // 5.关闭、重定向文件描述符
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    // 6.业务逻辑
    // 6-1 注册信号捕捉
    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = work;
    sigemptyset(&act.sa_mask);
    sigaction(SIGALRM, &act, NULL);

    struct itimerval val;
    val.it_value.tv_sec = 2;
    val.it_value.tv_usec = 0;
    val.it_interval.tv_sec = 3;
    val.it_interval.tv_usec = 0;

    // 6-2 创建定时器
    setitimer(ITIMER_REAL, &val, NULL);


    // 不让进程结束
    while (1)
    {
        sleep(10);
    }
    

    return 0;
}
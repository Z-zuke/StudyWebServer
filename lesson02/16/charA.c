#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

void action(pid_t pid, int fdw, int fdr);

int main(){
    // 1. 判断有名管道文件（fifo1, fifo2）是否存在，不存在则创建
    int ret = access("fifo1", F_OK);
    if (ret==-1)
    {
        printf("fifo1 not exits, create fifo1...\n");
        ret = mkfifo("fifo1", 0664);
        if (ret==-1)
        {
            perror("mkfifo");
            exit(0);
        }        
    }

    ret = access("fifo2", F_OK);
    if (ret==-1)
    {
        printf("fifo2 not exits, create fifo2...\n");
        ret = mkfifo("fifo2", 0664);
        if (ret==-1)
        {
            perror("mkfifo");
            exit(0);
        }        
    }
    
    // 2. 以只写的方式打开 fifo1
    int fd1_w = open("fifo1", O_WRONLY);
    if (fd1_w==-1)
    {
        perror("open");
        exit(0);
    }
    printf("open fifo1 success, wait write in...\n");
    
    // 3. 以只读的方式打开 fifo2
    int fd2_r = open("fifo2", O_RDONLY);
    if (fd2_r==-1)
    {
        perror("open");
        exit(0);
    }
    printf("open fifo2 success, wait read out...\n");
    
    // 4. fork 子进程，父进程负责写，子进程负责读
    pid_t pid = fork();
    action(pid, fd1_w, fd2_r);
    // 5. 关闭文件描述符
    close(fd1_w);
    close(fd2_r);

    return 0;
}


void action(pid_t pid, int fdw, int fdr){
    if (pid>0)
    {
        // 父进程负责写数据
        char buf[128];
        int ret;
        while(1){
            memset(buf, 0, 128);
            // 获取标准输入的数据
            fgets(buf, 128, stdin);
            // 写入数据
            ret = write(fdw, buf, strlen(buf));
            if (ret==-1)
            {
                perror("write");
                exit(0);
            }
        }
    }
    else if (pid==0)
    {
        // 子进程负责读数据
        char buf[128];
        int ret;
        while(1){
            memset(buf, 0, 128);
            ret = read(fdr, buf, 128);
            if (ret==-1)
            {
                perror("read");
                exit(0);
            }
            else if (ret == 0)
            {
                printf("write process out of connect...\n");
                break;
            }
            printf("B --> A: %s\n", buf);
        }        
    }
    
}
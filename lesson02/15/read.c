#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    // 1. 打开管道文件
    int fd = open("test", O_RDONLY);
    if (fd==-1)
    {
        perror("open");
        exit(0);
    }
    // 2. 读数据
    while (1)
    {
        char buf[1024]={0};
        int len = read(fd, buf, sizeof(buf));
        if(len==0){
            printf("write process out of connect...\n");
            break;
        }
        printf("receive buf: %s", buf);
    }
    close(fd);
    
    
    return 0;
}
#include<sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    int pipefd[2];
    int ret = pipe(pipefd);

    // 获取管道大小
    long size = fpathconf(pipefd[0], _PC_PIPE_BUF);
    printf("pipe size: %ld\n", size);

    return 0;
}
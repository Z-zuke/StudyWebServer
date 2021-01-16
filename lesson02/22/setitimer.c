#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>


// 过 3s 以后，每隔 2s 定时一次
int main(){
    struct itimerval new_value;
    // 设置时间间隔
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 0;
    // 设置延迟时间，3s 后开始定时
    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 0;

    int ret = setitimer(ITIMER_REAL, &new_value, NULL);  // 非阻塞
    printf("clock start...\n");
    if(ret == -1){
        perror("setitimer");
        exit(0);
    }
    return 0;
}
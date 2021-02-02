#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myAlarm(int num){
    printf("catch signal ID: %d\n", num);
    printf("XXXXXXXXXXXXXXXXXXXXXX\n");
}

int main(){

    struct sigaction act;
    act.sa_flags = 0;
    act.sa_handler = myAlarm;
    sigemptyset(&act.sa_mask);

    // 注册信号捕捉
    // void (*sighandler_t)(int); 函数指针，int类型为捕捉的信号的值
    sigaction(SIGALRM, &act, NULL); // SIG_IGN  SIG_DFL

    struct itimerval new_value;
    // 设置间隔时间
    new_value.it_interval.tv_sec = 2;
    new_value.it_interval.tv_usec = 0;
    // 设置延迟时间，3s 后开始第一次定时
    new_value.it_value.tv_sec = 3;
    new_value.it_value.tv_usec = 0;

    int ret = setitimer(ITIMER_REAL, &new_value, NULL);
    printf("timer start...\n");
    if(ret==-1){
        perror("setitimer");
        exit(0);
    }
    // getchar();
    while(1);
    return 0;
}
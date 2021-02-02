#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// 把所有的常规信号（1-31）的未决状态打印到屏幕
// 设置某些信号是阻塞的，通过键盘产生这些信号

int main(){
    // 设置 2、3 号信号阻塞
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    
    // 修改内核中的阻塞信号集
    sigprocmask(SIG_BLOCK, &set, NULL);

    int num=0;
    while(1){
        // 获取当前未决信号集的数据
        sigset_t pendingset;
        sigemptyset(&pendingset);
        sigpending(&pendingset);

        // 遍历前 32 位
        for(int i=1; i<=31; ++i){
            if(sigismember(&pendingset, i)==1){
                printf("1");
            }else if(sigismember(&pendingset, i)==0){
                printf("0");
            }else{
                perror("sigismember");
                exit(0);
            }
        }
        printf("\n\n");
        num += 1;
        sleep(1);
        if(num==10){
            // 解除阻塞
            sigprocmask(SIG_UNBLOCK, &set, NULL);
        }
    }

    return 0;
}
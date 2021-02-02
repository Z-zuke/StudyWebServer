#include <signal.h>
#include <stdio.h>

int main(){
    // 创建信号集
    sigset_t set;

    // 清空信号集内容
    sigemptyset(&set);

    // 判断 SIGINT 是否在 set 中
    int ret = sigismember(&set, SIGINT);
    if (ret==0)
    {
        printf("SIGINT non-blocking\n");
    }else if(ret==1){
        printf("SIGINT blocking\n");
    }

    // 向信号集添加信号
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    // 判断是否添加成功
    ret = sigismember(&set, SIGINT);
    if (ret==0)
    {
        printf("SIGINT non-blocking\n");
    }else if(ret==1){
        printf("SIGINT blocking\n");
    }
    ret = sigismember(&set, SIGQUIT);
    if (ret==0)
    {
        printf("SIGQUIT non-blocking\n");
    }else if(ret==1){
        printf("SIGQUIT blocking\n");
    }

    // 从信号集删除信号
    sigdelset(&set, SIGQUIT);
    // 判断是否删除成功
    ret = sigismember(&set, SIGQUIT);
    if (ret==0)
    {
        printf("SIGQUIT non-blocking\n");
    }else if(ret==1){
        printf("SIGQUIT blocking\n");
    }    

    return 0;
}
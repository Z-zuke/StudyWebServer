#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    // 父进程
    pid_t pid;

    // 创建 5 个子进程
    for(int i=0;i<5;++i){
        pid = fork();
        if(pid==0){
            break;
        }
    }

    if(pid>0){
        // 父进程行为
        while(1){
            printf("parent, pid = %d\n", getpid());
            // int ret = wait(NULL);
            int statusCode;
            int ret = wait(&statusCode);
            if(ret==-1){
                break;
            }

            if(WIFEXITED(statusCode)){
                // 正常退出
                printf("exit status code: %d\n", WEXITSTATUS(statusCode));
            }
            if(WIFSIGNALED(statusCode)){
                // 异常终止，被哪个信号终止
                printf("kill signal: %d\n", WTERMSIG(statusCode));
            }

            printf("child die, pid = %d\n", ret);
            sleep(1);
        }
    } else if(pid==0){
        // 子进程行为
        // while(1){
            printf("child, pid = %d\n",getpid());
            sleep(2);
        // }
        exit(0); // 0 即为传出状态码
    }

    return 0;
}
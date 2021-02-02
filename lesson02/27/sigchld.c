#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

void myFun(int num){
    printf("catched signal ID: %d\n", num);
    // 回收子进程 PCB 资源
    while(1){
        int ret = waitpid(-1, NULL, WNOHANG);
        if(ret>0){
            printf("child die, pid=%d\n", ret);
        } else if(ret==0){
            // 还有子进程活着
            break;
        } else if(ret==-1){
            // 没有子进程
            break;
        }
    }
}

int main(){
    // 提前设置好阻塞信号集，阻塞 SIGCHLD 
    // 子进程有可能很快结束，而父进程还没有注册好信号捕捉
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL);

    // 创建子进程
    pid_t pid;
    for(int i=0; i<10; ++i){
        pid = fork();
        if(pid==0){
            break;
        }
    }

    if(pid>0){
        // 父进程
        // 捕捉子进程结束时发送的 SIGCHLD 信号
        struct sigaction act;
        act.sa_flags = 0;
        act.sa_handler = myFun;
        sigemptyset(&act.sa_mask);
        sigaction(SIGCHLD, &act, NULL);
        
        // 注册完成信号捕捉，解除 SIGCHLD 阻塞
        sigprocmask(SIG_UNBLOCK, &set, NULL);

        while(1){
            printf("parent process pid: %d\n", getpid());
            sleep(2);
        }
    } else if(pid==0){
        // 子进程
        printf("child process pid: %d\n", getpid());
    }

    return 0;
}
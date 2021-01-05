#include <unistd.h>
#include <stdio.h>

int main(){
    pid_t pid = fork();

    if (pid>0){
        printf("parent process, pid: %d\n", getpid());
        sleep(1);
    }
    else if(pid==0){
        execl("hello", "hello", NULL);
        perror("execl");
        printf("child process, pid: %d\n", getpid());
    }

    for (int i=0; i<3;++i){
        printf("i=%d, pid=%d\n", i, getpid());
    }

    return 0;
}
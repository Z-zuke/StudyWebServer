#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>


int main(){
    int len = 4096;
    void *ptr = mmap(NULL, len, PROT_READ|PROT_WRITE, 
                     MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(ptr==MAP_FAILED){
        perror("mmap");
        exit(0);
    }

    pid_t pid = fork();
    if(pid>0){
        strcpy((char *) ptr, "hello, world!");
        wait(NULL);
    } else if(pid==0){
        sleep(1);
        printf("%s\n", (char *)ptr);
    }

    munmap(ptr, len);
    return 0;
}
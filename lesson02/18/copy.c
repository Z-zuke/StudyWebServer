#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>


int main(){
    int fd = open("english.txt", O_RDWR);
    if(fd == -1){
        perror("open");
        exit(0);
    }
    int len = lseek(fd, 0, SEEK_END);

    int fd1 = open("cpy.txt", O_RDWR | O_CREAT, 0664);
    if(fd1==-1){
        perror("open");
        exit(0);
    }
    truncate("cpy.txt", len);
    write(fd1, " ", 1);

    void *ptr1 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    void *ptr2 = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    if( ptr1 == MAP_FAILED){
        perror("mmap ptr1");
        exit(0);
    }
    if( ptr2 == MAP_FAILED){
        perror("mmap ptr2");
        exit(0);
    }

    memcpy(ptr2, ptr1, len);
    munmap(ptr2, len);
    munmap(ptr1, len);

    close(fd1);
    close(fd);
    return 0;
}
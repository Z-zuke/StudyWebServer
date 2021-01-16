#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

// 向文件读出内容
int main(){
    // 1. 打开文件
    int fd = open("test.txt", O_RDWR);
    int size = lseek(fd, 0, SEEK_END); // 获取文件大小
    printf("file size: %d\n", size);
    // 2. 创建内存映射区
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        perror("mmap");
        exit(0);
    }
    // 3. 使用内存映射区
    char buf[128];
    strcpy(buf, (char *)ptr);
    printf("read from file: %s\n", buf);
    // 4. 关闭映射区
    close(fd);
    munmap(ptr, size);
    return 0;
}
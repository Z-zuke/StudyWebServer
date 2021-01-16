#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    // 判断文件是否存在
    int ret = access("fifo1", F_OK);
    if (ret==-1)
    {
        printf("fifo not exits, create fifo\n");
        ret = mkfifo("fifo1", 0664);
        if (ret==-1)
        {
            perror("mkfifo");
            exit(0);
        }
        
    }    

    return 0;
}
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


int main(){
    // 1.创建一个共享内存
    key_t key = ftok("./", 'a');
    int shmid = shmget(key, 0, IPC_CREAT);
    printf("shmid: %d\n", shmid);

    // 2.和当前进程关联
    void *ptr = shmat(shmid, NULL, 0);

    // 3.读取数据
    printf("%s\n", (char *)ptr);

    printf("press to end process...\n");
    getchar();

    // 4.解除关联
    int ret = shmdt(ptr);
    if(ret==0){
        printf("marked delete by read...\n");
    } else if(ret==-1){
        printf("delete already!\n");
    }

    // 5.删除共享内存
    // 由最后一个解除关联的进程删除
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
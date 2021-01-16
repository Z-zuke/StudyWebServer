#include <stdio.h>
#include <unistd.h>


int main(){
    int secs = alarm(10);
    printf("seconds = %d\n", secs);
    sleep(2);
    secs = alarm(3); // 不阻塞
    printf("seconds = %d\n", secs);
    while(1){}
    
    return 0;
}
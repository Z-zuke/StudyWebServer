#include <stdio.h>
#include <arpa/inet.h>


int main() {
    // htons 转换端口
    unsigned short a = 0x0102;
    printf("a: %x\n", a);
    unsigned short b = htons(a);
    printf("b: %x\n", b);

    printf("=========================\n");

    // ntohs 转换端口
    unsigned short a1 = 0x0403;
    printf("a1: %x\n", a1);
    unsigned short b1 = ntohs(a1);
    printf("b1: %x\n", b1);

    printf("=========================\n");

    // htonl 转换 ip
    char buf[4] = {192, 168, 1, 100};
    int num = *(int *)buf;
    int res = htonl(num);
    unsigned char *p = (unsigned char *)&res;
    printf("%d.%d.%d.%d\n", *p, *(p+1), *(p+2), *(p+3));

    printf("=========================\n");

    // ntohl 转换 ip
    unsigned char buf1[4] = {101, 1, 168, 192};
    int num1 = *(int *)buf1;
    int res1 = ntohl(num1);
    unsigned char *p1 = (unsigned char *)&res1;
    printf("%d.%d.%d.%d\n", *p1, *(p1+1), *(p1+2), *(p1+3));

    return 0;
}
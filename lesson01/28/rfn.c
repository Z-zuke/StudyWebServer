#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

int getFileNum(const char *path);

// 读取指定目录下所有的普通文件个数
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("%s path\n", argv[0]);
        return -1;
    }
    int num = getFileNum(argv[1]);
    printf("普通文件个数：%d\n", num);
    return 0;
}

// 用于获取目录下所有的普通文件个数
int getFileNum(const char *path)
{
    // 1. 打开目录
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        exit(0);
    }

    // 2. 读取目录下文件
    struct dirent *ptr;
    int total = 0;
    while ((ptr = readdir(dir)) != NULL)
    {
        // 获取名称
        char *dname = ptr->d_name;
        // 忽略掉 . 和 .. 目录
        if (strcmp(dname, ".") == 0 || strcmp(dname, "..") == 0)
        {
            continue;
        }
        // 判断是普通文件还是目录
        if (ptr->d_type == DT_DIR)
        {
            // 目录，需要递归读取目录下内容
            char newpath[256];
            sprintf(newpath, "%s/%s", path, dname);
            total += getFileNum(newpath);
        }
        if (ptr->d_type == DT_REG)
        {
            // 普通文件
            ++total;
        }
    }

    // 3. 关闭目录
    closedir(dir);
    return total;
}
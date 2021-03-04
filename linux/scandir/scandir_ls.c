//perror
#include <stdio.h>
#include <errno.h>
//opendir
#include <sys/types.h>
#include <dirent.h>
//exit
#include <unistd.h>
#include <stdlib.h>
int main(int argc ,char* argv[])
{
    //打开目录
    DIR *dp;
    dp = opendir(argv[1]);
    if(dp == NULL){
        perror("opendir error.");
        exit(1);
    }
    //扫描目录
    struct dirent *dir;
    while( (dir=readdir(dp))!=NULL ){
        printf("%s\n" ,dir->d_name);
    }
    //关闭目录
    closedir(dp);
    return 0;
}


#include <stdio.h>
//open
#include <fcntl.h>
//perror
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
//thread
#include <sys/types.h>
#include <sys/stat.h>
//strlen
#include <string.h>
int main()
{
    int pid = fork();
    if(pid==-1){
        perror("fork error.");
        exit(1);
    }else if(pid>0){
        exit(0); //父进程直接退出
    }
    int ret = chdir("/home/"); //改变工作位置
    if(ret==-1){
        perror("chdir error.");
        exit(1);
    }
    umask(0022); //掩码文件755,文件夹422
    close(STDIN_FILENO);
    int fd = open("/dev/null",O_RDWR); //关闭STDIN_FILENO并使文件描述符0指向dev/null
    if(fd==-1){
        perror("open error.");
        exit(1);
    }
    dup2(fd ,STDOUT_FILENO);
    dup2(fd ,STDERR_FILENO); //重定向stdout和stderr
    
    int fdd = open("/home/peng/Desktop/linux_emmx/deamon/d.txt",O_RDWR|O_TRUNC);
    if(fdd==-1){
        perror("open error.");
        exit(1);
    }
    while(1){
        //守护进程程序模型
        static int num;
        char buf[32];
        sprintf(buf ,"num: %d\n" ,num++%1024);
        int ret=write(fdd ,buf ,strlen(buf));
        if(ret==-1){
            perror("write error.");
            exit(1);
        }
        sleep(1);
    }
    return 0;
}


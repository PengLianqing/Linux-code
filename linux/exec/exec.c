#include <stdio.h>
//exit
#include <unistd.h>
#include <stdlib.h>
//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//dup2将标准输出重定向至out.txt
int main()
{
    int fd1=open("./out.txt", O_RDWR | O_CREAT);
    dup2(fd1 ,STDOUT_FILENO);
    execlp("ls","ls","-l","-h",NULL);
    perror("exec error."); //exec出错会继续执行，否则执行完直接退出
    exit(1);

    return 0;
}


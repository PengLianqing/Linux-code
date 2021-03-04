#include <stdio.h>
//thread
#include <sys/types.h>
#include <unistd.h>
//exit
#include <unistd.h>
#include <stdlib.h>

//子进程执行ls,父进程执行wc -l避免出现孤儿进程
int main()
{
    int fd[2];
    int ret;
    int pid;
    ret = pipe(fd); //创建匿名管道
    if(ret==-1){
        perror("pipe error.");
        exit(1);
    }
    pid = fork();
    if(pid==-1){
        perror("fork error.");
        exit(1);
    }else if(pid==0){
        //子进程
        printf("child thread.pid is %d ,parent thread pid is %d\n",getpid() ,getppid());
        //子进程执行ls
        close(fd[0]); //关闭读端
        dup2(fd[1] ,STDOUT_FILENO); //将标准输出重定向到写端
        execlp("ls","ls","-l","-h",NULL);
        perror("exec error");
        exit(1);
    }else if(pid>0){
        //父进程
        printf("parent thread.pid is %d ,child thread pid is %d\n",getpid() ,pid);
        //父进程执行wc -l
        close(fd[1]); //关闭写端
        dup2(fd[0] ,STDIN_FILENO); //将标准输入重定向到读端
        execlp("wc","wc","-l",NULL);
        perror("exec error");
        exit(1);
    }
    return 0;
}


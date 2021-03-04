#include <stdio.h>
//thread
#include <sys/types.h>
#include <unistd.h>
//exit
#include <unistd.h>
#include <stdlib.h>
//僵尸进程,子进程终止但父进程并未对其回收,[thread_create] <defunct>
//孤儿进程,父进程先于子进程终止,子进程ppid变为1,由init进程领养

/*
int main()
{
    pid_t pid=fork();
    if(pid==-1){
        perror("fork error.");
        exit(1);
    }else if(pid==0){
        //子进程
        printf("child thread.pid is %d ,parent thread pid is %d\n",getpid() ,getppid());
        sleep(1);
        printf("child done.");
    }else if(pid>0){
        //父进程
        printf("parent thread.pid is %d ,child thread pid is %d\n",getpid() ,pid);
        sleep(5);
        printf("parent done.");
    }
    printf("Hello world\n");
    return 0;
}
*/

int main()
{
    int i=0;
    for(i=0 ;i<5 ;i++){
        pid_t pid=fork();
        if(pid==-1){
            perror("fork error.");
            exit(1);
        }else if(pid==0){
            //子进程
            printf("child %d thread.pid is %d ,parent thread pid is %d\n",i ,getpid() ,getppid());
            break; //循环创建子进程时子进程即时退出循环
        }else if(pid>0){
            //父进程
            printf("parent thread.pid is %d ,child thread pid is %d\n",getpid() ,pid);
        }
    }
    //不同进程执行对应任务.
    if(i==5){
        printf("#parent.\n");
        sleep(1);
    }else{
        printf("#child %d.\n" ,i);
    }
    //不同进程执行相同任务.
    sleep(5);
    printf("-all done\n");
    return 0;
}

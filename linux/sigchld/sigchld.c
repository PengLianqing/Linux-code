#include <stdio.h>
//fork
#include <sys/types.h>
#include <unistd.h>
//signal
#include <signal.h>
//waitpid
#include <sys/wait.h>
//rand
#include <stdlib.h>

//信号回调函数
void catch_child(int signo);

int main()
{
    pid_t pid;
    int i=0;
    for( ;i<10 ;i++){
        if((pid=fork())==0) //子进程直接退出循环
            break;
    }
    if(10==i){ //父进程
        printf("parent thread.pid is %d ,child thread pid is %d\n",getpid() ,pid);
        struct sigaction act;
        act.sa_handler = catch_child;   //设置回调函数
        sigemptyset(&act.sa_mask);      //设置捕捉函数执行期间屏蔽字
        act.sa_flags = 0;               //设置默认属性, 本信号自动屏蔽
        sigaction(SIGCHLD, &act, NULL); //注册信号捕捉函数
        //解除阻塞
        while(1){
            
        }
    }else{ //子进程
        printf("child %d thread.pid is %d ,parent thread pid is %d\n",i ,getpid() ,getppid());
        sleep(i);
        return i;
    }
    return 0;
}

void catch_child(int signo){
    pid_t wpid;
    int status;
    while((wpid=waitpid(wpid, &status, 0))!=-1){
        //循环回收
        if (WIFEXITED(status)) //子进程正常终止
                printf("-----catch child id %d, ret=%d\n", wpid, WEXITSTATUS(status)); //打印pid与返回值
    }
}
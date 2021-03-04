#include <stdio.h>
#include <stdlib.h>
//pthread
#include <pthread.h>

typedef struct{
    int a;
    int b;
}exit_t;

void *tfn0(void *arg){
    exit_t *ret;
    ret=malloc(sizeof(exit_t));
    ret->a=100;
    ret->b=20;
    pthread_exit((void*)ret);
}

void *tfn1(void *arg){
    int inc=(int)arg;
    sleep(inc*2);
    printf("pthread num %d\n" ,inc);
    return NULL;//pthread_exit((void*)0);
}

/*
peng@peng-virtual-machine:~/Desktop/linux_emmx/pthread$ ps -ef|grep pthread
peng        5950    2100  0 11:52 pts/1    00:00:00 ./pthread
peng        5958    5712  0 11:52 pts/4    00:00:00 grep --color=auto pthread
peng@peng-virtual-machine:~/Desktop/linux_emmx/pthread$ ps -Lf 5950
UID          PID    PPID     LWP  C NLWP STIME TTY      STAT   TIME CMD
peng        5950    2100    5950  0    4 11:52 pts/1    Sl+    0:00 ./pthread
peng        5950    2100    5954  0    4 11:52 pts/1    Sl+    0:00 ./pthread
peng        5950    2100    5955  0    4 11:52 pts/1    Sl+    0:00 ./pthread
peng        5950    2100    5956  0    4 11:52 pts/1    Sl+    0:00 ./pthread
*/
int main()
{
    pthread_t tid;
    exit_t *ret;
    //线程创建与回收
    pthread_create(&tid ,NULL ,tfn0 ,NULL); //创建线程，无传入参数
    pthread_join(tid ,(void**)&ret); //回收指定线程，得到返回值结构体exit_t
    printf("pthread return %d ,%d\n" ,ret->a ,ret->b);


    //循环创建线程
    for(int i=0;i<5;i++){
        pthread_create(&tid ,NULL ,tfn1 ,(void*)i); //传入参数
        pthread_detach(tid);        //设置线程分离,线程终止后会自动清理pcb,无需回收
    }
    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self()); //pid与线程id
    pthread_exit((void*)0); //主线程退出其他线程未退出，使用pthread_exit代替return
}

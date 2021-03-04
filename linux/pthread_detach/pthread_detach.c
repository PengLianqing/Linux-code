#include <stdio.h>
#include <stdlib.h>
//pthread
#include <pthread.h>
//strerror
#include <string.h>
//getpid
#include <sys/types.h>
#include <unistd.h>

void *tfn(void *arg){
    sleep(5);
    printf("thread: pid = %d, tid = %lu\n", getpid(), pthread_self());
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;
    pthread_attr_t attr; //创建一个线程属性结构体
    int ret = pthread_attr_init(&attr); //初始化线程属性
    if (ret != 0) {
        fprintf(stderr, "attr_init error:%s\n", strerror(ret));
        exit(1);
    }
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //设置线程属性
    if (ret != 0) {
        fprintf(stderr, "attr_setdetachstate error:%s\n", strerror(ret));
        exit(1);
    }
    ret = pthread_create(&tid ,NULL ,tfn ,NULL); //创建分离态的新线程
    if (ret != 0) {
        perror("pthread_create error");
    }
    ret = pthread_attr_destroy(&attr); //销毁线程属性
    if (ret != 0) {
        fprintf(stderr, "attr_destroy error:%s\n", strerror(ret));
        exit(1);
    }
    printf("main: pid = %d, tid = %lu\n", getpid(), pthread_self()); //pid与线程id
    pthread_exit((void *)0);
}


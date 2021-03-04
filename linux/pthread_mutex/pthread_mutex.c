#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//pthread
#include <pthread.h>

pthread_mutex_t mutex;
/*
通过加锁可以实现每个线程的完整输出而不被另一个线程打断：
peng@peng-virtual-machine:~/Desktop/linux_emmx/pthread_mutex$ ./pthread 
++Whats
++Wrong
--Hello
--World
++Whats
++Wrong
++Whats
++Wrong
--Hello
--World
--Hello
--World
++Whats
++Wrong
--Hello
--World
++Whats
++Wrong
--Hello
--World

不加锁的运行情况：
++Whats
--Hello
--World
--Hello
--World
--Hello
++Wrong
++Whats
++Wrong
++Whats
++Wrong
++Whats
--World
++Wrong
--Hello
++Whats
--World
++Wrong
--Hello
*/
void *tfn(void *arg){
    srand(time(NULL));
    while(1){
        pthread_mutex_lock(&mutex); //加锁
        printf("--Hello\n");
        sleep(rand()%3);
        printf("--World\n");
        pthread_mutex_unlock(&mutex); //解锁
        sleep(rand()%3);
    }
}

int main()
{
    int flag=5;
    pthread_t tid;
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL); //初始化锁
    pthread_create(&tid ,NULL ,tfn ,NULL); //创建线程，无传入参数
    while(flag--){
        pthread_mutex_lock(&mutex); //加锁
        printf("++Whats\n");
        sleep(rand()%3);
        printf("++Wrong\n");
        pthread_mutex_unlock(&mutex); //解锁
        sleep(rand()%3);
    }
    pthread_cancel(tid); //将子线程杀死,子线程中自带取消点
    pthread_join(tid ,NULL); //回收指定线程，得到返回值结构体exit_t
    pthread_mutex_destroy(&mutex); //销毁锁
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//pthread
#include <pthread.h>

/* 
3个线程不定时"写"全局资源,5个线程不定时"读"同一全局资源
*/
pthread_rwlock_t rwlock;
int counter; //全局资源

void *tfnwrite(void *arg){
    int t;
    int i=(int)arg;
    while(1){
        t=counter;
        usleep(1000);
        pthread_rwlock_wrlock(&rwlock); //加写锁
        printf("=======write %d: %lu: counter=%d ++counter=%d\n", i, pthread_self(), t, ++counter);
        pthread_rwlock_unlock(&rwlock); //解除锁
        usleep(9000); //为r锁提供机会
    }
    return NULL;
}

void *tfnread(void *arg){
    int i=(int)arg;
    while(1){
        pthread_rwlock_rdlock(&rwlock); //加读锁
        printf("----------------------------read %d: %lu: %d\n", i, pthread_self(), counter);
        pthread_rwlock_unlock(&rwlock); //解除锁
        usleep(2000); //为w锁提供机会
    }
    return NULL;
}

int main()
{
    pthread_t tid[8];
    pthread_rwlock_init(&rwlock, NULL); //初始化读写锁
    for(int i=0 ;i<3 ;i++){
        pthread_create(&tid[i], NULL, tfnwrite, (void*)i);
    }
    for(int i=3 ;i<8 ;i++){
        pthread_create(&tid[i], NULL, tfnread, (void*)i);
    }
    for(int i=0 ;i<8 ;i++){
        pthread_join(tid[i], NULL);
    }
    pthread_rwlock_destroy(&rwlock); //销毁锁
    return 0;
}

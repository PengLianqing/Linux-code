#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//pthread
#include <pthread.h>
//sem
#include <semaphore.h>

/*借助信号量模拟生产者-消费者问题*/
#define NUM 5
int queue[NUM];
sem_t blanknum,productnum;

void *producter(void *p){
    int i=0;
    for(; ;){
        sem_wait(&blanknum); //生产者将空格子数--，为0则阻塞等待
        queue[i]=rand()%1024+1; //模拟生产产品
        printf("produce %d.\n" ,queue[i]);
        sem_post(&productnum); //产品数++
        i=(i+1)%NUM; //借助下标实现环形队列
        sleep(rand()%3);
    }
    return NULL;
}

void *consumer(void *p){
    int i=0;
    for(; ;){
        sem_wait(&productnum); //消费者将产品数--，为0则阻塞等待
        printf("consume %d.\n" ,queue[i]);
        queue[i]=0; //模拟消费产品
        sem_post(&blanknum); //空格子数++
        i=(i+1)%NUM; 
        sleep(rand()%3);
    }
    return NULL;
}

int main()
{
    pthread_t pid,cid;
    srand(time(NULL));
    sem_init(&blanknum ,0 ,NUM);
    sem_init(&productnum ,0 ,0); //初始化信号量
    pthread_create(&pid, NULL, producter, NULL);
    pthread_create(&cid, NULL, consumer, NULL);
    pthread_join(pid, NULL);
    pthread_join(cid, NULL);
    sem_destroy(&blanknum);
    sem_destroy(&productnum); //销毁信号量
    return 0;
}

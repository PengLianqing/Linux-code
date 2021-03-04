#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//pthread
#include <pthread.h>

/*借助条件变量模拟生产者-消费者问题*/
/*链表作为共享数据,需被互斥量保护*/
struct msg{
    struct msg* next;
    int num;
};
struct msg* head;
/* 静态初始化一个条件变量和一个互斥量*/
pthread_cond_t has_product = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *producter(void *p){
    struct msg* mp;
    for(; ;){
        mp=malloc(sizeof(struct msg));
        mp->num=rand()%1024; //模拟生产一个产品
        printf("produce %d.\n" ,mp->num);

        pthread_mutex_lock(&lock); //加锁
        mp->next=head;
        head=mp; //将产品添加到链表
        pthread_mutex_unlock(&lock); //解锁
        pthread_cond_signal(&has_product); //将等待在该条件变量上的线程唤醒
        sleep(rand()%3);
    }
    return NULL;
}

void *consumer(void *p){
    struct msg* mp;
    for(; ;){
        pthread_mutex_lock(&lock); //加锁
        while(head==NULL){ //头指针为空时说明没有产品
            //阻塞等待条件变量cond满足并解锁信号量；当条件满足，函数返回时，重新加锁信号量
            pthread_cond_wait(&has_product ,&lock); 
        }
        mp=head;
        head=mp->next; //模拟消费一个产品
        pthread_mutex_unlock(&lock); //解锁
        printf("consume %d.\n",mp->num);
        free(mp); //释放内存
        sleep(rand()%3);
    }
    return NULL;
}

int main()
{
    pthread_t pid,cid;
    srand(time(NULL));
    pthread_create(&pid, NULL, producter, NULL);
    pthread_create(&cid, NULL, consumer, NULL);
    pthread_join(pid, NULL);
    pthread_join(cid, NULL);
    return 0;
}

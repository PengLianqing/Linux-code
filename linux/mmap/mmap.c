#include <stdio.h>
//mmap
#include <sys/mman.h>
//thread
#include <sys/types.h>
#include <unistd.h>
//exit
#include <unistd.h>
#include <stdlib.h>
//open
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//memcpy
#include <string.h>

struct mmap_date_t{
    int num;
    int idate[3];
    float fdate[3];
    char cdate[16];
};

int main()
{
    //匿名映射
    struct mmap_date_t *p = (struct mmap_date_t *)mmap(NULL, sizeof(struct mmap_date_t), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(p==MAP_FAILED){
        perror("mmap error.");
        exit(1);
    }

    int pid=fork();
    if(pid==-1){
        perror("fork error.");
        exit(1);
    }else if(pid==0){
        //子进程
        while(1){
            //*p方式
            //*p=rand()%1024;
            
            //memcpy方式
            static int i;
            struct mmap_date_t mmap_date;
            mmap_date.num=i++%1024;
            sprintf(mmap_date.cdate,"hello");
            mmap_date.fdate[0]=rand()%1024*0.01f;
            mmap_date.fdate[1]=rand()%1024*0.01f;
            mmap_date.fdate[2]=rand()%1024*0.01f;
            mmap_date.idate[0]=rand()%1024;
            mmap_date.idate[1]=rand()%1024;
            mmap_date.idate[2]=rand()%1024;
            memcpy(p,&mmap_date,sizeof(mmap_date));
            sleep(1);
        }
    }else if(pid>0){
        //父进程
        while(1){
            //printf("%d\n",(int)*p);
            printf("read: %d ,%d ,%d ,%d ,%4.2f ,%4.2f ,%4.2f ,%s\n" ,
                   p->num ,
                   p->idate[0] ,
                   p->idate[1] ,
                   p->idate[2] ,
                   p->fdate[0] ,
                   p->fdate[1] ,
                   p->fdate[2] ,
                   p->cdate  );
            sleep(1);
        }
    }
    //释放mmap
    munmap(p ,sizeof(struct mmap_date_t));
    return 0;
}


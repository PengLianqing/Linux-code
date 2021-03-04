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

#define MMAP_FILE "./m.txt"

int main()
{
    int ret = truncate(MMAP_FILE, 256);
    if(ret==-1){
        perror("truncate error.");
        exit(1);
    }
    //打开mmap文件
    int fd = open(MMAP_FILE,O_RDWR);
    if(fd==-1){
        perror("open error.");
        exit(1);
    }
    //mmap shared
    struct mmap_date_t *p = (struct mmap_date_t *)mmap(NULL, sizeof(struct mmap_date_t), PROT_WRITE, MAP_SHARED, fd, 0); //读
    if(p==MAP_FAILED){
        perror("mmap error.");
        exit(1);
    }
    close(fd); //关闭mmap文件
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
    //释放mmap
    munmap(p ,sizeof(struct mmap_date_t));
    return 0;
}


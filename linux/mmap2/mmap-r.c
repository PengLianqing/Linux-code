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
    struct mmap_date_t *p = (struct mmap_date_t *)mmap(NULL, sizeof(struct mmap_date_t), PROT_READ, MAP_SHARED, fd, 0); //读
    if(p==MAP_FAILED){
        perror("mmap error.");
        exit(1);
    }
    close(fd); //关闭mmap文件
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
    //释放mmap
    munmap(p ,sizeof(struct mmap_date_t));
    return 0;
}


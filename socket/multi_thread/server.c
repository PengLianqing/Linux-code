#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
//pthread
#include <pthread.h>
#include <errno.h>
//socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
//read
#include <unistd.h>
//wrap.h
#include "wrap.h"

void *tfn(void *arg);

struct s_info{
    struct sockaddr_in clinaddr;
    int connfd;
    int num;
};

char buf[4096]; //用于通信
int ret = -1; 

int main()
{
    int lfd = 0,cfd = 0;
    char clin_ip[32];
    
    pthread_t tid;
    //配置服务器地址结构
    struct sockaddr_in serv_addr , clin_addr;
    //memset(&serv_addr ,0 ,sizeof(serv_addr));
    bzero(&serv_addr ,sizeof(serv_addr)); //将地址结构清零 <strings.h>
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202); //本地套接字转网络短整型
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //本地套接字转长整型
    
    //创建socket文件描述符
    lfd = Socket(AF_INET,SOCK_STREAM,0);
    //为socket文件描述符绑定服务器端地址结构
    ret = Bind(lfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr));
    //设置与服务器建立的最大连接数
    Listen(lfd ,128);

    socklen_t clin_addr_len = sizeof(clin_addr);
    
    struct s_info ts[256];
    
    int num = 0;
    while(1)
    {
        num ++; //建立的第num个连接

        cfd = Accept(lfd ,(struct sockaddr *)&clin_addr ,&clin_addr_len);
        
        ts[num].num = num;
        ts[num].connfd = cfd;
        ts[num].clinaddr = clin_addr;
        //创建线程，传入参数num
        int ret = pthread_create(&tid ,NULL ,tfn ,(void *)&ts[num]);
        if(ret != 0)
        {
            fprintf(stderr ,"pthread create error :%s\n" ,strerror(ret));
            exit(1);
        }
        pthread_detach(tid);
                        
        //打印新建客户端连接地址结构
        printf("Server: new connect. IP address: %s,Port: %d \n",
                inet_ntop(AF_INET,&clin_addr.sin_addr.s_addr,clin_ip,sizeof(clin_ip)),
                htons(clin_addr.sin_port)  );
    } 
    printf("All done\n");
    return 0;
}

void *tfn(void *arg)
{
    struct s_info *ts = arg; 

    printf("thread: pid = %d ,tid = %lu \n",getpid() ,pthread_self());
            
    ssize_t res;
    printf("thread %d on Work.",ts->num);
    
    while(1)
    {   
        //服务器线程读取数据
        char child_process_buf[32];
        res = Read(ts->connfd ,buf ,sizeof(buf));
        sprintf(child_process_buf ,"Child %d read %d : ",ts->num ,(int)res);
        Write(STDOUT_FILENO ,child_process_buf ,sizeof("Child %d read: "));
               
        if((int)(res) == 0)
        {
            sprintf(child_process_buf,"disconnect %d \n" ,ts->num);
            write(STDOUT_FILENO ,child_process_buf ,sizeof("------disconnect %d \n"));
            sleep(1);
            close(ts->connfd);
            return (void *)0; //读到0字节，说明连接终止了
        }
                
        Write(STDOUT_FILENO,buf,res);
                
        //服务器线程发送数据
        sprintf(buf ,"pthread %d write :",ts->num);
        Write(STDOUT_FILENO ,buf ,sizeof("pthread %d write :")); //fprintf会出莫名其妙的问题
        #define serv_write_size sizeof("Server %d write: %d ,%5.2f ,%5.2f \n")
        sprintf(buf ,"Server %d write: %d ,%5.2f ,%5.2f \n" ,ts->num ,rand()%1024,
                (float)(rand()%1024*0.1f) ,(float)(rand()%1024*0.1f));
        Write(ts->connfd ,buf ,serv_write_size);
        Write(STDOUT_FILENO,buf,serv_write_size);
    }
}

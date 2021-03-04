#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
//signal
#include <signal.h>
//wait
#include <sys/wait.h>
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
//thread pool
#include "threadpool.h"

void *process(void *arg);

int ret = -1; 

int main()
{
    int lfd = 0;
    
    threadpool_t *thp = threadpool_create(3,100,100);/*创建线程池，池里最小3个线程，最大100，队列最大100*/
    printf("pool inited");

    //配置服务器地址结构
    struct sockaddr_in serv_addr , clin_addr;
    //memset(&serv_addr ,0 ,sizeof(serv_addr));
    bzero(&serv_addr ,sizeof(serv_addr)); //将地址结构清零 <strings.h>
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202); //本地套接字转网络短整型
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //本地套接字转长整型
    
    //创建socket文件描述符
    lfd = Socket(AF_INET,SOCK_STREAM,0);

    // 端口复用
    int flag = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    
    //为socket文件描述符绑定服务器端地址结构
    ret = Bind(lfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr));
    //设置与服务器建立的最大连接数
    Listen(lfd ,128);

    socklen_t clin_addr_len = sizeof(clin_addr);
    
    int num = 0; //记录建立的连接数
    int *cfd;
    while(1)
    {
        cfd = malloc( sizeof(int) );
        *cfd = Accept(lfd ,(struct sockaddr *)&clin_addr ,&clin_addr_len);
        //客户端地址结构
        char clin_ip[32];
        printf("new connection %d ,cfd %d ,client: ip %s ,port: %d", ++num ,*cfd ,
           inet_ntop(AF_INET,&clin_addr.sin_addr.s_addr,clin_ip,sizeof(clin_ip)),
           ntohs(clin_addr.sin_port) );
        threadpool_add(thp, process, (void*)cfd);     /* 向线程池中添加任务 */
    }    
    printf("All done\n");
    threadpool_destroy(thp);
    return 0;
}

void *process(void *arg){
    int cfd = *(int *)arg;
    ssize_t res;
    char buf[4096];
    while(1){
        res = Read(cfd ,buf ,sizeof(buf));
        if((int)res <= 0) //连接关闭或错误
        {
            close(cfd);
            return NULL;
        }
        printf("task %d read %d : %s",cfd ,(int)res ,buf);
        
        //服务器子进程发送数据
        memset(buf ,0 ,sizeof(buf));
        sprintf(buf ,"%4d ,%5.2f ,%5.2f\n" ,rand()%1024,
                (float)(rand()%1024*0.1f) ,(float)(rand()%1024*0.1f));
        res = Write(cfd ,buf ,strlen(buf));
        printf("Server %d write: %d\n" ,cfd ,(int)res);
    }
    return NULL;
}



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
    
    int num = 0; //记录建立的连接数
    int *cfd;
    while(1)
    {
        usleep(10000);
        cfd = malloc( sizeof(int) );

        struct sockaddr_in serv_addr; //服务器地址结构
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(6202); 
        inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
        *cfd = Socket(AF_INET,SOCK_STREAM,0); //创建socket
        ret = Connect(*cfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr) ); //连接服务器端
        
        //服务器地址结构
        char serv_ip[32];
        printf("Server: ip %s ,port: %d",
            inet_ntop(AF_INET,&serv_addr.sin_addr.s_addr,serv_ip,sizeof(serv_ip)),
            ntohs(serv_addr.sin_port) );

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
    //服务器子进程发送数据
    memset(buf ,0 ,sizeof(buf));
    sprintf(buf ,"GET / HTTP/1.1\r\n\r\n");
    res = Write(cfd ,buf ,strlen(buf));
    printf("Server %d write: %d\n" ,cfd ,(int)res);
    close(cfd);
    return NULL;
}



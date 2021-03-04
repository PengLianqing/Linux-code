#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//fcntl
#include <unistd.h>
#include <fcntl.h>
//stat
#include <sys/types.h>
#include <sys/stat.h>
//socket
#include <sys/types.h>
#include <sys/socket.h>
#include "wrap.h"
//epoll
#include <sys/epoll.h>

void close_connection(int cfd ,int epfd);

//user var
char buf[4096],write_buf[128];
char clin_ip[32];
int ret = -1;
int main()
{
    int lfd = 0,cfd = 0;
    char clin_ip[32];
            
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

    //创建监听红黑树并将lfd挂到红黑树上
    int epfd = epoll_create(1024);
    if(epfd == -1)
    {
        perror("epoll creat error");
        exit(1);
    }
    struct epoll_event tep ,ep[1024];
    tep.events = EPOLLIN;
    tep.data.fd = lfd;
    ret = epoll_ctl(epfd ,EPOLL_CTL_ADD ,lfd ,&tep);
    if(ret==-1)
    {
        perror("epoll_ctr add lfd error");
        exit(1);
    }

    while(1)
    {
        //监听
        ret = epoll_wait(epfd ,ep ,1024 ,-1);
        if(ret==-1)
        {
            perror("epoll_wait error");
            exit(1);
        }

        for(int i=0 ;i<ret ;i++)
        {
            //只处理读事件, 其他事件默认不处理
            struct epoll_event *pev = &ep[i];
            if(!(pev->events & EPOLLIN)) {
                // 不是读事件
                continue;
            }
            if(pev->data.fd==lfd)
            {
                //新客户端建立连接
                socklen_t clin_addr_len = sizeof(clin_addr);
                cfd = Accept(lfd ,(struct sockaddr *)&clin_addr ,&clin_addr_len);

                //打印客户端地址结构
                printf("new connection: client ip %s ,port: %d\n",
                    inet_ntop(AF_INET,&(clin_addr.sin_addr.s_addr),clin_ip,sizeof(clin_ip)),
                    ntohs(clin_addr.sin_port));
                
                //设置cfd为非阻塞
                int flag = fcntl(cfd ,F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd ,F_SETFL ,flag);

                //将cfd挂到红黑树上
                //设置边沿非阻塞模式，否则会循环按行读取http请求
                tep.events = EPOLLIN | EPOLLET;
                tep.data.fd = cfd;
                epoll_ctl(epfd ,EPOLL_CTL_ADD ,cfd ,&tep);
            }
            else
            {
                char sprint_buf[128];
                memset(buf ,0 ,sizeof(buf));
                ret = Read(pev->data.fd ,buf ,sizeof(buf));
                if(ret==0){
                    close_connection(pev->data.fd ,epfd); //读到0说明客户端断开连接
                }
                sprintf(sprint_buf ,"server %d read %d byte:" ,pev->data.fd ,ret);
                Write(STDOUT_FILENO ,sprint_buf ,strlen(sprint_buf));
                Write(STDOUT_FILENO ,buf ,strlen(buf));

                memset(buf ,0 ,sizeof(buf));
                sprintf(buf ,"Server %d write: %d ,%5.2f ,%5.2f \n" ,pev->data.fd ,rand()%1024,
                        (float)(rand()%1024*0.1f) ,(float)(rand()%1024*0.1f));
                Write(pev->data.fd ,buf ,strlen(buf));
                Write(STDOUT_FILENO,buf ,strlen(buf));
            }
        }
    }
    printf("all done.\n");
    return 0;
}

void close_connection(int cfd ,int epfd)
{
    int ret = epoll_ctl(epfd ,EPOLL_CTL_DEL ,cfd ,NULL);
    if(ret==-1)
    {
        perror("epoll_ctl error");
        exit(1);
    }
    close(cfd);
}

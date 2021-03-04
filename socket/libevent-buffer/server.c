#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//socket
#include <sys/types.h>
#include <sys/socket.h>

//inet_ntop
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//libevent 
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

#define SERVER_PORT 6202
void listener_cb(
                 struct evconnlistener *listener ,
                 evutil_socket_t fd ,
                 struct sockaddr *addr ,
                 int len ,void *ptr);

void read_cb(struct bufferevent *bev ,void *arg);
void write_cb(struct bufferevent *bev ,void *arg);
void event_cb(struct bufferevent *bev , short events ,void *arg);

//服务器端程序
int main()
{
    //初始化服务器地址结构
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);

    //创建eventbase
    struct event_base *base;
    base = event_base_new();

    //创建listener
    struct evconnlistener *listener;
    listener = evconnlistener_new_bind(base ,listener_cb ,base ,
                                       LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE ,
                                       36 ,(struct sockaddr *)&serv ,sizeof(serv));
    printf("srart task.\n");
    //启动循环
    event_base_dispatch(base);

    //释放listener与base
    evconnlistener_free(listener);
    event_base_free(base);

    printf("All done.\n");
    return 0;
}

//evconnlistener_new_bind回调函数
//打印连接客户端情况
//创建bufferevent事件对象，并设置回调函数
void listener_cb(
                 struct evconnlistener *listener ,
                 evutil_socket_t fd ,
                 struct sockaddr *addr ,
                 int len ,void *ptr)
{
    //打印客户端地址结构
    char serv_ip[32];
    struct sockaddr_in *clin = (struct sockaddr_in *)addr;
    printf("new connection: client ip %s ,port: %d\n",
            inet_ntop(AF_INET,&(clin->sin_addr.s_addr),serv_ip,sizeof(serv_ip)),
            ntohs(clin->sin_port));

    //创建bufferevent事件
    struct event_base *event = (struct event_base *)ptr;
    struct bufferevent *bev = bufferevent_socket_new(event ,fd ,BEV_OPT_CLOSE_ON_FREE);
    //为缓冲区设置回调函数与权限
    bufferevent_setcb(bev ,read_cb ,write_cb ,event_cb ,NULL);
    bufferevent_enable(bev ,EV_READ);
}

//bufferevent回调函数
void read_cb(struct bufferevent *bev ,void *arg)
{
    //bufferevent_read
    char buf[1024] = {0};
    ssize_t ret = bufferevent_read(bev ,buf ,sizeof(buf));
    printf("client write: %ld, %s",ret ,buf);
    //bufferevent_write
    #define WRITE_FORMAT "server write: %4d ,%5.2f \n"
    memset(buf ,0 ,sizeof(buf));
    sprintf(buf ,WRITE_FORMAT ,rand()%1024 ,(float)(rand()%1024*0.10f));
    bufferevent_write(bev ,(const void*)buf ,sizeof(WRITE_FORMAT));
    printf("%s" ,buf);
}

void write_cb(struct bufferevent *bev ,void *arg)
{
    printf("write ok.\n");
}

void event_cb(struct bufferevent *bev , short events ,void *arg)
{
    if (events & BEV_EVENT_EOF)
    {
        printf("connection closed\n");  
                    
    }
    else if(events & BEV_EVENT_ERROR)   
    {
        printf("some other error\n");
                      
    }
    else if(events & BEV_EVENT_CONNECTED)
    {
        printf("connect success.\n");
        return;                              
    }
                
    // 释放资源
    bufferevent_free(bev);
}







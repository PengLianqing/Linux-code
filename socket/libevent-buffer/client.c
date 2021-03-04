#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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

void read_cb(struct bufferevent *bev,void *arg);
void write_cb(struct bufferevent *bev ,void *arg);
void event_cb(struct bufferevent *bev ,short events ,void *arg);
void read_terminal(evutil_socket_t fd , short what ,void *arg);

//客户端程序
int main()
{
    //创建eventbase
    struct event_base *base = NULL;
    base = event_base_new();
    
    //创建用于通信的socket文件
    int fd = socket(AF_INET ,SOCK_STREAM ,0);

    //将socket加入到bufferevent
    struct bufferevent *bev = NULL;
    bev = bufferevent_socket_new(base ,fd ,BEV_OPT_CLOSE_ON_FREE);

    //初始化服务器端地址结构
    struct sockaddr_in serv;
    memset(&serv ,0 ,sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT);
    //serv.sin_addr.s_addr = htonl(INADDR_ANY); //服务器端addr系统分配，客户端需要指定服务器addr
    inet_pton(AF_INET ,"127.0.0.1" ,&serv.sin_addr.s_addr);

    //连接服务器
    bufferevent_socket_connect(bev ,(struct sockaddr *)&serv ,sizeof(serv));

    //设置回调函数和权限
    bufferevent_setcb(bev ,read_cb ,write_cb ,event_cb ,NULL);
    bufferevent_enable(bev ,EV_READ);

    //创建事件
    struct event* event_user = event_new(base ,STDIN_FILENO ,EV_READ |EV_PERSIST ,
                                         read_terminal ,bev);
    event_add(event_user ,NULL);
    
    printf("task start.\n");
    //启动循环
    event_base_dispatch(base);

    //释放base与用户event
    event_free(event_user);
    event_base_free(base);

    printf("All done.\n");
    return 0;
}

void read_terminal(evutil_socket_t fd , short what ,void *arg)
{
    char buf[1024] = {0};
    int ret = read(fd ,buf ,sizeof(buf));
    struct bufferevent *bev = (struct bufferevent *)arg;
    bufferevent_write(bev ,buf ,ret+1);
}

//bufferevent回调函数
void read_cb(struct bufferevent *bev ,void *arg)
{
    char buf[1024] = {0};
    bufferevent_read(bev ,buf ,sizeof(buf));
    printf("server write: %s",buf);
    /*
    #define WRITE_FORMAT "client write: %4d ,%5.2f \n"
    memset(buf ,0 ,sizeof(buf));
    sprintf(buf ,WRITE_FORMAT,rand()%1024 ,(float)(rand()%1024*0.10f));
    bufferevent_write(bev ,(const void*)buf ,sizeof(WRITE_FORMAT));
    */
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





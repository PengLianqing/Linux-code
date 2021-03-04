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
//poll
#include <poll.h>

//user var
char buf[4096],write_buf[128];
char clin_ip[32];
int ret = -1;

#define OPEN_MAX 1024
int main()
{
    int i, maxi, lfd, cfd, sockfd;
    int nready; //poll返回值, 记录满足监听事件的fd个数
    ssize_t n;

    char buf[4096], str[INET_ADDRSTRLEN];
    socklen_t clilen;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in cliaddr;
            
    //配置服务器地址结构
    struct sockaddr_in serv_addr;
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

    client[0].fd = lfd; //lfd,存入client[0]
    client[0].events = POLLIN;            

    for (i = 1; i < OPEN_MAX; i++)
        client[i].fd = -1; //用-1初始化client[]里剩下元素

    maxi = 0; //client[]数组有效元素中最大元素下标

    while(1)
    {
        nready = poll(client, maxi+1, -1); //阻塞监听事件

        if (client[0].revents & POLLIN) { //lfd

            clilen = sizeof(cliaddr);
            cfd = Accept(lfd, (struct sockaddr *)&cliaddr, &clilen);
            printf("received from %s at PORT %d\n",
                    inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                    ntohs(cliaddr.sin_port));

            for (i = 1; i < OPEN_MAX; i++)
                if (client[i].fd < 0) {
                    client[i].fd = cfd; //找到client[]中空闲的位置,存放accept返回的cfd
                    break;
                }

            if (i == OPEN_MAX) //达到了最大客户端数
                perr_exit("too many clients");

            client[i].events = POLLIN; //设置刚刚返回的cfd,监控读事件 */
            if (i > maxi)
                maxi = i; //更新client[]中最大元素下标
            if (--nready <= 0)
                continue; //没有更多就绪事件时,继续回到poll阻塞
        }
        for (i = 1; i <= maxi; i++) { //cfd
            if ((sockfd = client[i].fd) < 0)
                continue;

            if (client[i].revents & POLLIN) {
                memset(buf ,0 ,sizeof(buf));
                if ((n = Read(sockfd, buf, sizeof(buf))) < 0) {
                    /* connection reset by client */
                    if (errno == ECONNRESET) {  /* 收到RST标志 */
                        printf("client[%d] aborted connection\n", i);
                        Close(sockfd);
                        client[i].fd = -1;      /* poll中不监控该文件描述符,直接置为-1即可,不用像select中那样移除 */
                    } else
                        perr_exit("read error");
                } else if (n == 0) {            /* 说明客户端先关闭链接 */
                    printf("client[%d] closed connection\n", i);
                    Close(sockfd);
                    client[i].fd = -1;
                } else {
                    char sprint_buf[128];
                    sprintf(sprint_buf ,"server %d read:" ,i);
                    Write(STDOUT_FILENO ,sprint_buf ,strlen(sprint_buf));
                    Write(STDOUT_FILENO ,buf ,strlen(buf));

                    memset(buf ,0 ,sizeof(buf));
                    sprintf(buf ,"Server %d write: %d ,%5.2f ,%5.2f \n" ,i ,rand()%1024,
                            (float)(rand()%1024*0.1f) ,(float)(rand()%1024*0.1f));
                    Write(sockfd ,buf ,strlen(buf));
                    Write(STDOUT_FILENO,buf ,strlen(buf));
                }
                if (--nready <= 0)
                    break;
            }
        }
    }
    printf("all done.\n");
    return 0;
}
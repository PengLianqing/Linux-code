#include <stdio.h>
#include <strings.h>
#include <string.h>
//select
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
//wrap
#include "wrap.h"

char buf[4096]; //用于通信
char sprintf_buf[128]; //用于显示
int ret = -1; 
int main()
{
    int lfd = 0,cfd = 0;
    char clin_ip[32];
    int i=0;
    //配置服务器地址结构
    struct sockaddr_in serv_addr , clin_addr;
    //memset(&serv_addr ,0 ,sizeof(serv_addr));
    bzero(&serv_addr ,sizeof(serv_addr)); //将地址结构清零 <strings.h>
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202); //本地套接字转网络短整型
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //本地套接字转长整型
    
    //创建socket文件描述符
    lfd = Socket(AF_INET,SOCK_STREAM,0);
    
    //端口复用
    int opt =1;
    setsockopt(lfd ,SOL_SOCKET ,SO_REUSEADDR ,&opt ,sizeof(opt));
    
    //为socket文件描述符绑定服务器端地址结构
    ret = Bind(lfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr));
    //设置与服务器建立的最大连接数
    Listen(lfd ,128);

    int maxfd = lfd; //初始化maxfd为lfd，此时没有连接建立
    int maxi = -1; //初始化建立的连接数
    int client[FD_SETSIZE];
    for(i=0 ;i<FD_SETSIZE ;i++)
    {
        client[i] = -1;
    }
    
    fd_set rset ,allset; //rset是读事件描述符集合；allset是所有事件文件描述符集合
    FD_ZERO(&allset);
    FD_SET(lfd ,&allset); //初始化allset并添加lfd监听
    int nready = 0; //nready为满足事件的个数

    while(1)
    {
        rset = allset; //rset为传入传出参数，故需要以allset作为所有事件文件描述符集合
        
        printf("wait for read maxfd:%d;maxi %d.\n",maxfd ,maxi);
        nready = select(maxfd+1 ,&rset ,NULL ,NULL ,NULL);
        if(nready <0)
        {
            perr_exit("select error.");
        }
        if(FD_ISSET(lfd ,&rset)) //lfd有新的客户端连接请求
        {
            socklen_t clin_addr_len = sizeof(clin_addr);
            cfd = Accept(lfd ,(struct sockaddr *)&clin_addr ,&clin_addr_len);
            //打印新建客户端连接地址结构
            printf("Server: new connect. IP address: %s,Port: %d \n",
                     inet_ntop(AF_INET,&clin_addr.sin_addr.s_addr,clin_ip,sizeof(clin_ip)),
                     htons(clin_addr.sin_port)  );
            for(i=0 ;i<FD_SETSIZE ;i++)
            {
                //添加到client[]数组
                if(client[i] < 0)
                {
                    client[i] = cfd;
                    break;
                }
            }

            //连接数不能超过文件描述符最大值1024
            if(i == FD_SETSIZE)
            {
                fputs("too many connections.\n" ,stderr);
                exit(1);
            }
            FD_SET(cfd ,&allset); //添加新描述符到allset
            //存在前面文件描述符关闭等情况，cfd可能被分配在前面
            //判断cfd那件描述符是否超过maxfd
            if(cfd > maxfd)
            {
                maxfd = cfd;
            }
            //判断i大小是否超过maxi
            if(i > maxi)
            {
                maxi = i;
            }

            //--nread == 0时说明只有lfd的新客户端连接请求；否则可能还有其他监控到read
            if(--nready == 0)
            {
                continue;
            }
        }
        
        //lfd没有新连接请求时，说明只有监控到其他read
        int sockfd ,n;
        for(i=0 ;i<=maxi ;i++) //使用maxi循环lient数组
        {
            if( (sockfd = client[i]) < 0 )
            {
                continue;
            }
            if( FD_ISSET(sockfd ,&rset) )
            {
                n = Read(sockfd ,buf ,sizeof(buf));
                //如果读到0字节，说明连接已关闭
                if( n==0 )
                {
                    close(sockfd);
                    printf("close client %d.\n",n);
                    FD_CLR(sockfd ,&allset);
                    client[i] = -1;
                }else if( n>0 )
                {   
                    sprintf(sprintf_buf ,"---Client %d read %d: ",i ,n);
                    Write(STDOUT_FILENO ,sprintf_buf ,sizeof("Child %d read: "));
                    Write(STDOUT_FILENO ,buf ,n);

                    //-----------------------process();---------------------------
                    memset(sprintf_buf ,0 ,128);
                    sprintf(sprintf_buf ,"Server %d write: %d ,%5.2f ,%5.2f \n" ,i ,rand()%1024,
                           (float)(rand()%1024*0.1f) ,(float)(rand()%1024*0.1f));
                    Write(STDOUT_FILENO ,sprintf_buf ,sizeof("Server %d write: %d ,%5.2f ,%5.2f \n"));
                    Write(sockfd ,sprintf_buf ,sizeof("Server %d write: %d ,%5.2f ,%5.2f \n"));
                }
                if( --nready == 0 )
                {
                    break;
                }
            }
        }
    }

    printf("All done\n");
    return 0;
}


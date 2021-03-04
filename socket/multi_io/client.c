#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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

int ret = -1;

int main()
{
    char buf[4096]; 
    int cfd = 0;
    struct sockaddr_in serv_addr; //服务器地址结构
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202); 
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);
    cfd = Socket(AF_INET,SOCK_STREAM,0); //创建socket
    ret = Connect(cfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr) ); //连接服务器端
    
    //服务器地址结构
    char serv_ip[32];
    printf("Server: ip %s ,port: %d",
           inet_ntop(AF_INET,&serv_addr.sin_addr.s_addr,serv_ip,sizeof(serv_ip)),
           ntohs(serv_addr.sin_port) );

    int i = 10 ;
    while(i--)
    {   
        memset(buf ,0 ,sizeof(buf));
        sprintf(buf,"server write: %d, %5.2f ,%5.2f \n",rand()%1024,(float)(rand()%1024*0.1f),(float)(rand()%1024*0.1f));
        Write(cfd , buf ,sizeof("server write: %d, %5.2f ,%5.2f \n"));
        printf("client write:%d ,%s",ret ,buf);
        
        memset(buf ,0 ,sizeof(buf));
        printf("Wait for Read.\n");
        Read(cfd ,buf ,sizeof(buf));
        write(STDOUT_FILENO,buf,ret);

        sleep(1);
    }
    close(cfd);
    printf("All Done\n");
    return 0;
}


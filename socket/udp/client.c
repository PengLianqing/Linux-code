#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
int ret = -1;

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main()
{
    char buf[4096]; //BUFSIZ 4096字节
    int sockfd = 0;
    struct sockaddr_in serv_addr; //服务器地址结构
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202); //Aliyun 6202
    //47.110.234.39
    inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr.s_addr);

    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd == -1)
    {
        sys_err("socket error");
    }

    int i = 10 ;
    while(i--)
    {   
        memset(buf ,0 ,sizeof(buf));
        sprintf(buf,"server write: %d, %5.2f ,%5.2f \n",rand()%1024,(float)(rand()%1024*0.1f),(float)(rand()%1024*0.1f));

        //如果不指定发送长度会出乱码
        ret = sendto(sockfd , buf ,strlen(buf) ,0 ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr));
        if(ret == -1)
        {
            sys_err("write error");
        }
        write(STDOUT_FILENO,buf ,ret);
        
        printf("Wait for Read.\n");
        memset(buf ,0 ,sizeof(buf));
        ret = recvfrom(sockfd , buf ,sizeof(buf) ,0 ,NULL ,0);
        if(ret == -1)
        {
            sys_err("read error");
        }
        write(STDOUT_FILENO,buf,ret);

        sleep(1);
    }
    close(sockfd);
    printf("All Done\n");
    return 0;
}


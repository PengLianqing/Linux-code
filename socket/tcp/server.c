#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//socket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
//read
#include <unistd.h>

int main()
{
    char buf[4096];
    int lfd ,cfd;
    int ret;
    struct sockaddr_in serv_addr ,clin_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    lfd = socket(AF_INET ,SOCK_STREAM ,0);
    if(lfd==-1){
        perror("socket error.");
        exit(1);
    }
    ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret==-1){
        perror("bind error.");
        exit(1);
    }
    listen(lfd ,128);
    socklen_t clin_addr_len=sizeof(clin_addr);
    cfd=accept(lfd, (struct sockaddr*)&clin_addr, &clin_addr_len);
    if(cfd==-1){
        perror("accept error.");
        exit(1);
    } 
    //地址结构
    char clin_ip[32];
    printf("Client: ip %s,port %d\n",
        inet_ntop(AF_INET, &clin_addr.sin_addr.s_addr, clin_ip, sizeof(clin_ip)),
        ntohs(clin_addr.sin_port));
    int i=0;
    while(1){
        memset(buf ,0 ,sizeof(buf));
        printf("wait for read.\n");
        ret = read(cfd ,buf ,sizeof(buf));
        if(ret==-1){
            perror("read error.");
            exit(1);
        }
        write(STDOUT_FILENO ,buf ,ret); 
        
        memset(buf ,0 ,sizeof(buf));
        sprintf(buf ,"Server write: %d ,%d ,%4.2f\n"  ,++i ,rand()%1024 ,rand()%1024*0.01f);
        ret = write(cfd ,buf ,strlen(buf));
        if(ret==-1){
            perror("read error.");
            exit(1);
        }
    }
    close(lfd);
    close(cfd);
    printf("all done.\n");
    return 0;
}


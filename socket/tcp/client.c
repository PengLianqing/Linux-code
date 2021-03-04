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
    int cfd;
    int ret;
    struct sockaddr_in serv_addr; //sockaddr_in
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202);
    inet_pton(AF_INET ,"127.0.0.1",&serv_addr.sin_addr.s_addr); //server ip地址
    cfd = socket(AF_INET ,SOCK_STREAM ,0); //创建socket
    if(cfd==-1){
        perror("socket error.");
        exit(1);
    }
    ret = connect(cfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); //连接服务器端
    if(ret==-1){
        perror("bind error.");
        exit(1);
    }
    //地址结构
    char serv_ip[32];
    printf("Client: ip %s,port %d\n",
        inet_ntop(AF_INET, &serv_addr.sin_addr.s_addr, serv_ip, sizeof(serv_ip)),
        ntohs(serv_addr.sin_port));
    int i=0;
    while(1){
        sleep(2);
        memset(buf ,0 ,sizeof(buf));
        sprintf(buf ,"Client write: %d ,%d ,%4.2f\n"  ,++i ,rand()%1024 ,rand()%1024*0.01f);
        ret = write(cfd ,buf ,strlen(buf));
        if(ret==-1){
            perror("read error.");
            exit(1);
        }

        memset(buf ,0 ,sizeof(buf));
        printf("wait for read.\n");
        ret = read(cfd ,buf ,sizeof(buf));
        if(ret==-1){
            perror("read error.");
            exit(1);
        }
        write(STDOUT_FILENO ,buf ,ret); 
    }
    close(cfd);
    printf("all done.\n");
    return 0;
}


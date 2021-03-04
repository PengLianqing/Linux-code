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

void catch_child(int signa);
char buf[4096]; //用于通信
int ret = -1; 

int main()
{
    int lfd = 0,cfd = 0;
    char clin_ip[32];
    
    pid_t pid;

    //配置服务器地址结构
    struct sockaddr_in serv_addr , clin_addr;
    //memset(&serv_addr ,0 ,sizeof(serv_addr));
    bzero(&serv_addr ,sizeof(serv_addr)); //将地址结构清零 <strings.h>
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(6202); //本地套接字转网络短整型
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //本地套接字转长整型
    
    //创建socket文件描述符
    lfd = Socket(AF_INET,SOCK_STREAM,0);
    //为socket文件描述符绑定服务器端地址结构
    ret = Bind(lfd ,(struct sockaddr *)&serv_addr ,sizeof(serv_addr));
    //设置与服务器建立的最大连接数
    Listen(lfd ,128);

    socklen_t clin_addr_len = sizeof(clin_addr);
    
    int num = 0;
    while(1)
    {
        num ++; //建立的第num个连接

        cfd = Accept(lfd ,(struct sockaddr *)&clin_addr ,&clin_addr_len);
        
        pid = fork();
        if(pid < 0)
        {
            perror("fork error.");
            exit(1);
        }
        else if(pid ==0) //子进程
        {
            sleep(1);
            ssize_t res;
            printf("Child %d on Work.\n",num);
            Close(lfd);
            while(1)
            {   
                //服务器子进程读取数据
                char child_process_buf[32];
                res = Read(cfd ,buf ,sizeof(buf));
                sprintf(child_process_buf ,"Child %d read %d : ",num ,(int)res);
                Write(STDOUT_FILENO ,child_process_buf ,sizeof("Child %d read: "));
                
                if((int)(res) == 0)
                {
                    close(cfd);
                    write(STDOUT_FILENO ,"Chile %d gg.\n" ,num);
                    exit(1); //读到0字节，说明连接终止了
                }
                
                Write(STDOUT_FILENO,buf,res);
                
                //服务器子进程发送数据
                printf("Child %d write: ",num);
                #define serv_write_size sizeof("Server %d write: %d ,%5.2f ,%5.2f \n")
                sprintf(buf ,"Server %d write: %d ,%5.2f ,%5.2f \n" ,num ,rand()%1024,
                        (float)(rand()%1024*0.1f) ,(float)(rand()%1024*0.1f));
                Write(cfd ,buf ,serv_write_size);
                Write(STDOUT_FILENO,buf,serv_write_size);
            }
            
        }
        else if(pid >0) //主进程
        {

            //主进程不做通信，关闭用于通信的套接字cfd
            Close(cfd);
            //打印新建客户端连接地址结构
            printf("Server: new connect. IP address: %s,Port: %d \n",
                    inet_ntop(AF_INET,&clin_addr.sin_addr.s_addr,clin_ip,sizeof(clin_ip)),
                    htons(clin_addr.sin_port)  );
            
            //回收僵尸进程
            static int SIGCHLD_has_created = 0;
            if( (++SIGCHLD_has_created) == 1 )
            {
                //初始化捕捉信号SIGCHLD
                struct sigaction act;
                act.sa_handler = catch_child; //回调函数
                sigemptyset(&act.sa_mask); //设置捕捉函数执行期间屏蔽字
                act.sa_flags = 0; //设置默认属性，本信号自动屏蔽
                sigaction(SIGCHLD ,&act ,NULL); //注册信号捕捉函数
            }
        }
    }
    
    printf("All done\n");
    return 0;
}

void catch_child(int signa)
{
    pid_t wpid;
    //循环非阻塞方式回收任意子进程
    while( (wpid = waitpid(0 ,NULL ,WNOHANG))>0 )
    {
        printf("---------------catch child id %d \n",wpid );
    }
    return;
}

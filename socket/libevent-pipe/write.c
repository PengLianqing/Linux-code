#include <event2/util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//libevent
#include <event2/event.h>

void write_cb(evutil_socket_t fd ,short what ,void *arg){
    char buf[1024];

    static int num=0;
    sprintf(buf ,"%d ,%4.2f\n",num++ ,rand()%1024*0.01f);
    write(fd ,buf ,strlen(buf));

    sleep(1);
}

int main(){
    //打开fifo文件
    int fd=open("fifo",O_WRONLY);
    if(fd==-1){
        perror("open error");
        exit(1);
    }
    //创建base
    struct event_base* base=NULL;
    base=event_base_new();
    //创建event
    struct event* ev=NULL;
    ev=event_new(base ,fd ,EV_WRITE|EV_PERSIST ,write_cb ,NULL);
    //添加事件
    event_add(ev, NULL);
    //启动循环
    event_base_dispatch(base);
    //释放资源
    event_free(ev);
    event_base_free(base);
    //关闭管道
    close(fd);
    printf("All done.\n");
    return 0;
}
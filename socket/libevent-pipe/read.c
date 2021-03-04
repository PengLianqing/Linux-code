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

void read_cb(evutil_socket_t fd ,short what ,void *arg){
    char buf[1024];

    int len=read(fd ,buf ,sizeof(buf));
    printf("read event %s\n",what&EV_READ?"Yes":"No");
    printf("read %d :%s\n",len ,buf);
    sleep(1);
}

int main(){
    //创建命名管道
    unlink("fifo");
    mkfifo("fifo",0664);
    //打开fifo文件
    int fd=open("fifo",O_RDONLY);
    if(fd==-1){
        perror("open error");
        exit(1);
    }
    //创建base
    struct event_base* base=NULL;
    base=event_base_new();
    //创建event
    struct event* ev=NULL;
    ev=event_new(base ,fd ,EV_READ|EV_PERSIST ,read_cb ,NULL);
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
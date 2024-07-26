/*
 * @Author       : mark
 * @Date         : 2020-06-15
 * @copyleft Apache 2.0
 */ 
#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h> //epoll_ctl()
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h> // close()
#include <vector>
#include <errno.h>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);//最大能够检测的事件的数量

    ~Epoller();

    bool AddFd(int fd, uint32_t events);//往rpoll里面添加   文件描述符

    bool ModFd(int fd, uint32_t events);//修改事件

    bool DelFd(int fd);         //删除事件

    int Wait(int timeoutMs = -1);   

    int GetEventFd(size_t i) const;     //获取事件的fd

    uint32_t GetEvents(size_t i) const; //获取events
        
    //使用模板
private:
    int epollFd_;           //epoll_create() 创建一个epoll对象，返回值就是epollfd
    std::vector<struct epoll_event> events_;    //检测到的事件的集合
};

#endif //EPOLLER_H
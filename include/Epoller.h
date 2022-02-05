#ifndef EPOLLER_H
#define EPOLLER_H

#include<cstring>
#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<map>
#include<set>
#include<sys/epoll.h>
#include<unistd.h>

const int MAX_FD = 4096;

class Epoller {
public:
    Epoller();
    Epoller(const Epoller &) = delete;
    Epoller &operator=(const Epoller &) = delete;
    Epoller(Epoller &&) = default;
    Epoller &operator=(Epoller &&) = default;
    ~Epoller();
    
    void Add(int fd_, uint32_t events_);
    void Mod(int fd_, uint32_t events_);
    void Del(int fd_);
    
    epoll_event &Get(int id_);
    int Wait(int timeout_);
    
private:
    int epollfd;
    epoll_event revents[MAX_FD];
};



#endif
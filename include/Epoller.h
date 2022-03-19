#pragma once

#include<cstring>
#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<map>
#include<set>
#include<memory>
#include<sys/epoll.h>
#include<unistd.h>

#include"Config.h"

class Epoller {
public:
    using ptr = std::shared_ptr<Epoller>;
    Epoller();
    Epoller(const Epoller &) = delete;
    Epoller &operator=(const Epoller &) = delete;
    Epoller(Epoller &&) = default;
    Epoller &operator=(Epoller &&) = default;
    ~Epoller() { ::close(epollfd); }
    
    void add(int fd_, uint32_t events_);
    void mod(int fd_, uint32_t events_);
    void del(int fd_);
    int wait(int timeout_);

    epoll_event &get(int id_) { return revents[id_]; }    
private:
    int epollfd;
    epoll_event revents[MAX_FD];
};
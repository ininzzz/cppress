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

#define MAX_FD 512

class Epoller {
public:
    Epoller();
    void Add(int fd_, uint32_t events_);
    void Mod(int fd_, uint32_t events_);
    void Del(int fd_);
    int Wait(int timeout);
    epoll_event &Get(int p);
    uint32_t GetEvents(int fd);
    ~Epoller();
private:
    static const int maxn = MAX_FD;
    int epollfd;
    epoll_event revents[maxn];
    uint32_t events[maxn];
    std::set<int> fd_set;
};



#endif
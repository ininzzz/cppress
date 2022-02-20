#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include<map>
#include<memory>

#include"ThreadPool.h"
#include"Epoller.h"
#include"Timer.h"


class EventLoop {
public:
    using ptr = std::shared_ptr<EventLoop>;
    
    EventLoop();
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    EventLoop(EventLoop &&) = default;
    EventLoop &operator=(EventLoop &&) = default;
    ~EventLoop() = default;

    // 注册事件
    void addEvent(int fd_, uint32_t event_) { epoll->add(fd_, event_); }
    void modEvent(int fd_, uint32_t event_) { epoll->mod(fd_, event_); }
    void delEvent(int fd_) { epoll->del(fd_); }

    // 设置回调函数
    void setAcceptCallBack(int fd_, const std::function<void()> &func_);
    void setReadCallBack(const std::function<void(int)> &func_);
    void setWriteCallBack(const std::function<void(int)> &func_);
    void setCloseCallBack(const std::function<void(int)> &func_);
    void setTimeoutCallBack(const std::function<void(int)> &func_);

    // 事件循环
    void loop();
    
    // 设置定时器
    void setTimeout(int timeout_);
    void addTimeoutTask(int fd_);
    void eraseFromTimer(int fd_);
private:
    // 线程池
    ThreadPool::ptr pool;
    // epoll
    Epoller::ptr epoll;
    // 定时器
    Timer::ptr timer;
    int timeout;
    // 回调函数
    int listenfd;
    std::function<void()> accept_callback;
    std::function<void(int)> read_callback;
    std::function<void(int)> write_callback;
    std::function<void(int)> close_callback;
    
};


#endif
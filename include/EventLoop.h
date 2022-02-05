#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include<map>

#include"ThreadPool.h"
#include"Epoller.h"
#include"Timer.h"


class EventLoop {
public:
    EventLoop();
    EventLoop(const EventLoop &) = delete;
    EventLoop &operator=(const EventLoop &) = delete;
    EventLoop(EventLoop &&) = default;
    EventLoop &operator=(EventLoop &&) = default;
    ~EventLoop() = default;

    void AddEvent(int fd_, uint32_t event_);
    void ModEvent(int fd_, uint32_t event_);
    void DeleteEvent(int fd_);

    void Set_AcceptCallBack(int fd_, const std::function<void()> &func_);
    void Set_ReadCallBack(const std::function<void(int)> &func_);
    void Set_WriteCallBack(const std::function<void(int)> &func_);
    void Set_CloseCallBack(const std::function<void(int)> &func_);
    void Set_TimeoutCallBack(const std::function<void(int)> &func_);

    // 事件循环
    void Loop();
    
    // 设置超时时间
    void SetTimeout(int timeout_);
    void AddTimeoutTask(int fd_);
private:
    // 线程池
    ThreadPool pool;
    // epoll
    Epoller epoll;
    // 定时器
    Timer timer;
    int timeout;

    int listenfd;
    std::function<void()> accept_callback;
    std::function<void(int)> read_callback;
    std::function<void(int)> write_callback;
    std::function<void(int)> close_callback;
    
};


#endif
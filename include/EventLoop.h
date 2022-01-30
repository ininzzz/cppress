#ifndef EVENTLOOP_H
#define EVENTLOOP_H
#include<map>

#include"ThreadPool.h"
#include"Epoller.h"
#include"Timer.h"


class EventLoop {
public:
    EventLoop();
    // 重新激活事件(epolloneshot)
    void RestartEvent(int fd_);
    // 激活写事件
    void EnableWrite(int fd_);
    // 激活读事件
    void EnableRead(int fd_);
    // 关闭读事件
    void ShutdownWrite(int fd_);
    // 注册读事件
    void AddReadEvent(int fd_, const std::function<void()> &r_);
    // 注册写事件
    void AddWriteEvent(int fd_, const std::function<void()> &w_);
    // 删除事件(一般是客户端close后删除，否则add和mod会报错)
    void DeleteEvent(int fd_);
    // 事件循环
    void Loop();
    // 设置超时时间
    void SetTimeout(int timeout_);
    void AddTimeout(int fd_, const std::function<void()> &callback);
    void UpdateTimeout(int fd_);
private:
    // 线程池
    ThreadPool pool;
    // epoll
    Epoller epoll;
    // 定时器
    Timer timer;
    int timeout;
    // 可读事件回调函数
    std::map<int, std::function<void()>> read_callback;
    // 可写事件回调函数
    std::map<int, std::function<void()>> write_callback;
};


#endif
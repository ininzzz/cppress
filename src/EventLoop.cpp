#include"EventLoop.h"

EventLoop::EventLoop() {}

void EventLoop::RestartEvent(int fd_) {
    uint32_t events = 0;
    // epolloneshot保证每个socket只被一个线程处理
    events |= EPOLLONESHOT;
    events |= EPOLLIN;
    events |= EPOLLRDHUP;
    epoll.Add(fd_, events);
}

void EventLoop::AddReadEvent(int fd_, const std::function<void()> &r_) {
    uint32_t events = epoll.GetEvents(fd_);
    events |= EPOLLONESHOT;
    events |= EPOLLIN;
    events |= EPOLLRDHUP;
    read_callback[fd_] = r_;
    epoll.Add(fd_, events);
}

void EventLoop::AddWriteEvent(int fd_, const std::function<void()> &w_) {
    uint32_t events = epoll.GetEvents(fd_);
    events |= EPOLLOUT;
    write_callback[fd_] = w_;
    epoll.Add(fd_, events);
}

void EventLoop::DeleteEvent(int fd_) {
    epoll.Del(fd_);
    read_callback.erase(fd_);
    write_callback.erase(fd_);
}

void EventLoop::SetTimeout(int timeout_) {
    timer.SetTimeout(timeout_);
    timeout = timeout_;
}

void EventLoop::AddTimeout(int fd_, const std::function<void()> &callback) {
    timer.Add(fd_, callback);
}

void EventLoop::UpdateTimeout(int fd_) {
    timer.Update(fd_);
}

void EventLoop::Loop() {
    pool.run();
    while (true) {
        timer.Handle();
        int len = epoll.Wait(timeout);
        for (int i = 0;i < len;i++) {
            epoll_event now = epoll.Get(i);
            int fd = now.data.fd;
            // 对端关闭连接,做相应处理
            if (now.events & EPOLLRDHUP) {
                DeleteEvent(fd);
                if (close(fd) < 0) continue;
                printf("%d is closed\n", fd);
                continue;
            }
            if (now.events & EPOLLIN) {
                pool.append(read_callback[fd]);
            }
            if (now.events & EPOLLOUT) {
                pool.append(write_callback[fd]);
            }
        }
    }
}
#include"EventLoop.h"

EventLoop::EventLoop() {}

void EventLoop::AddEvent(int fd_, uint32_t event_) {
    epoll.Add(fd_, event_);
}

void EventLoop::ModEvent(int fd_, uint32_t event_) {
    epoll.Mod(fd_, event_);
}

void EventLoop::DeleteEvent(int fd_) {
    epoll.Del(fd_);
    timer.Erase(fd_);
}

void EventLoop::SetTimeout(int timeout_) {
    timer.SetTimeout(timeout_);
    timeout = timeout_;
}

void EventLoop::AddTimeoutTask(int fd_) {
    timer.Insert(fd_);
}


void EventLoop::Set_AcceptCallBack(int fd_, const std::function<void()> &func_) {
    listenfd = fd_;
    accept_callback = std::move(func_);
}

void EventLoop::Set_ReadCallBack(const std::function<void(int)> &func_) {
    read_callback = std::move(func_);
}

void EventLoop::Set_WriteCallBack(const std::function<void(int)> &func_) {
   write_callback = std::move(func_);
}

void EventLoop::Set_CloseCallBack(const std::function<void(int)> &func_) {
    close_callback = std::move(func_);
}

void EventLoop::Set_TimeoutCallBack(const std::function<void(int)> &func_) {
    timer.SetCallback(func_);
}

void EventLoop::Loop() {
    while (true) {
        timer.Handle();
        int len = epoll.Wait(timeout);
        for (int i = 0;i < len;i++) {
            epoll_event now = epoll.Get(i);
            int fd = now.data.fd;
            if (now.events & EPOLLRDHUP) {
                this->close_callback(fd);
            }
            else if (now.events & EPOLLIN) {
                if (fd == this->listenfd) {
                    pool.append([this, fd] {
                        this->accept_callback();
                    });
                }
                else {
                    pool.append([this, fd] {
                        this->read_callback(fd);
                    });
                }
            }
            else if (now.events & EPOLLOUT) {
                pool.append([this, fd] {
                    this->write_callback(fd);
                });
            }
        }
    }
}
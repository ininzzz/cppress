#include"EventLoop.h"

EventLoop::EventLoop()
    :pool(new ThreadPool), epoll(new Epoller), timer(new Timer) {
}

// 设置定时器

void EventLoop::setTimeout(int timeout_) {
    timer->setTimeout(timeout_);
    timeout = timeout_;
}

void EventLoop::addTimeoutTask(int fd_) {
    timer->insert(fd_);
}

void EventLoop::eraseFromTimer(int fd_) {
    timer->erase(fd_);
}


// 设置回调函数
void EventLoop::setAcceptCallBack(int fd_, const std::function<void()> &func_) {
    listenfd = fd_;
    accept_callback = std::move(func_);
}

void EventLoop::setReadCallBack(const std::function<void(int)> &func_) {
    read_callback = std::move(func_);
}

void EventLoop::setWriteCallBack(const std::function<void(int)> &func_) {
   write_callback = std::move(func_);
}

void EventLoop::setCloseCallBack(const std::function<void(int)> &func_) {
    close_callback = std::move(func_);
}

void EventLoop::setTimeoutCallBack(const std::function<void(int)> &func_) {
    timer->setCallback(func_);
}

void EventLoop::loop() {
    while (true) {
        timer->handle();
        int len = epoll->wait(timeout);
        for (int i = 0;i < len;i++) {
            epoll_event now = epoll->get(i);
            int fd = now.data.fd;
            if (now.events & EPOLLRDHUP) {
                this->close_callback(fd);
            }
            else if (now.events & EPOLLIN) {
                if (fd == this->listenfd) {
                    pool->append([this, fd] {
                        this->accept_callback();
                    });
                }
                else {
                    pool->append([this, fd] {
                        this->read_callback(fd);
                    });
                }
            }
            else if (now.events & EPOLLOUT) {
                pool->append([this, fd] {
                    this->write_callback(fd);
                });
            }
        }
    }
}
#include"Epoller.h"

Epoller::Epoller() {
    epollfd = epoll_create1(EPOLL_CLOEXEC);
    bzero(events, sizeof(events));
    if (epollfd == -1) throw std::runtime_error("epoll create error\n");
}

Epoller::~Epoller() {
    close(epollfd);
}

void Epoller::Add(int fd_, uint32_t events_) {
    if (fd_set.count(fd_)) return Mod(fd_, events_);
    epoll_event event;
    event.data.fd = fd_;
    event.events = events_;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_, &event);
    if (ret == -1) throw std::runtime_error("epoll add error\n");
    fd_set.insert(fd_);
    events[fd_] = events_;
}

void Epoller::Mod(int fd_, uint32_t events_) {
    epoll_event event;
    event.data.fd = fd_;
    event.events = events_;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd_, &event);
    if (ret == -1) throw std::runtime_error("epoll mod error\n");
    events[fd_] = events_;
}

void Epoller::Del(int fd_) {
    if (fd_set.count(fd_) == 0) return;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd_, NULL);
    if (ret == -1) throw std::runtime_error("epoll del error\n");
    fd_set.erase(fd_);
    events[fd_] = 0;
}

int Epoller::Wait(int timeout) {
    return epoll_wait(epollfd, revents, maxn, timeout);
}

epoll_event &Epoller::Get(int p) {
    return revents[p];
}

uint32_t Epoller::GetEvents(int fd) {
    return events[fd];
}
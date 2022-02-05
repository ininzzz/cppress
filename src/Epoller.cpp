#include"Epoller.h"

Epoller::Epoller() {
    epollfd = epoll_create1(EPOLL_CLOEXEC);
    if (epollfd == -1) throw std::runtime_error("epoll create error\n");
}

Epoller::~Epoller() {
    close(epollfd);
}

void Epoller::Add(int fd_, uint32_t events_) {
    epoll_event event;
    event.data.fd = fd_;
    event.events = events_;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd_, &event);
    if (ret == -1) throw std::runtime_error("epoll add error\n");
}

void Epoller::Mod(int fd_, uint32_t events_) {
    epoll_event event;
    event.data.fd = fd_;
    event.events = events_;
    int ret = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd_, &event);
    if (ret == -1) throw std::runtime_error("epoll mod error\n");
}

void Epoller::Del(int fd_) {
    int ret = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd_, NULL);
    if (ret == -1) throw std::runtime_error("epoll del error\n");
}

int Epoller::Wait(int timeout_) {
    return epoll_wait(epollfd, revents, MAX_FD, timeout_);
}

epoll_event &Epoller::Get(int id_){
    return revents[id_];
}
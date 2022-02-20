#include"Timer.h"

Timer::Timer():save(8192) {}

void Timer::insert(int fd_) {
    if (std::chrono::high_resolution_clock::to_time_t(save[fd_]) > 0) erase(fd_);

    TimerNode node;
    node.fd = fd_;
    save[fd_] = node.tm = std::chrono::high_resolution_clock::now() + interval;
    que.push(node);
}

void Timer::update(int fd_) {
    if (std::chrono::high_resolution_clock::to_time_t(save[fd_]) == 0) return;
    save[fd_] = std::chrono::high_resolution_clock::now() + interval;
}

void Timer::erase(int fd_) {
    save[fd_] = std::chrono::high_resolution_clock::from_time_t(0);
}

void Timer::handle() {
    while (!que.empty()) {
        TimerNode node = que.top();
        if (node.tm > std::chrono::high_resolution_clock::now()) break;
        if (save[node.fd] > std::chrono::high_resolution_clock::now()) {
            que.pop();
            node.tm = save[node.fd];
            que.push(node);
            continue;
        }
        que.pop();
        if (std::chrono::high_resolution_clock::to_time_t(save[node.fd]) == 0) continue;
        save[node.fd] = std::chrono::high_resolution_clock::from_time_t(0);
        callback(node.fd);
    }
}

void Timer::setTimeout(int timeout_) {
    interval = std::chrono::milliseconds(timeout_);
}

void Timer::setCallback(const std::function<void(int)> &func_) {
    callback = std::move(func_);
}
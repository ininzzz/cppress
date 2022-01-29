#include"Timer.h"

Timer::Timer(int timeout) :interval(timeout) {}

void Timer::Add(int fd, const std::function<void()> &callback) {
    while (fd >= save.size()) save.emplace_back();
    
    TimerNode node;
    node.fd = fd;
    node.callback = callback;
    save[fd] = node.tm = std::chrono::high_resolution_clock::now() + interval;
    que.push(node);
}

void Timer::Update(int fd) {
    save[fd] = std::chrono::high_resolution_clock::now() + interval;
}

void Timer::Handle() {
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
        save[node.fd] = std::chrono::high_resolution_clock::from_time_t(0);
        node.callback();
    }
}
void Timer::SetTimeout(int timeout) {
    interval = std::chrono::milliseconds(timeout);
}
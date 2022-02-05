#ifndef TIMER_H
#define TIMER_H

#include<cassert>
#include<chrono>
#include<queue>
#include<vector>
#include<iostream>
#include<functional>

struct TimerNode {
    int fd;
    std::chrono::time_point<std::chrono::high_resolution_clock> tm;
    bool operator>(const TimerNode &tn) const{
        return tm > tn.tm;
    }
};

class Timer {
public:
    Timer();
    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;
    Timer(Timer &&) = default;
    Timer &operator=(Timer &&) = default;
    ~Timer() = default;
    
    void Insert(int fd_);
    void Erase(int fd_);
    void Update(int fd_);
    void Handle();
    void SetTimeout(int timeout_);
    void SetCallback(const std::function<void(int)> &func_);
private:
    std::priority_queue<TimerNode, std::vector<TimerNode>, std::greater<TimerNode>> que;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> save;
    std::chrono::milliseconds interval;
    std::function<void(int)> callback;
};

#endif
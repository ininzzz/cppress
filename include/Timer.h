#pragma once

#include<cassert>
#include<chrono>
#include<queue>
#include<vector>
#include<iostream>
#include<functional>
#include<memory>

#include"Config.h"

struct TimerNode {
    int fd;
    std::chrono::time_point<std::chrono::high_resolution_clock> tm;
    bool operator>(const TimerNode &tn) const{
        return tm > tn.tm;
    }
};

class Timer {
public:
    using ptr = std::shared_ptr<Timer>;
    
    Timer();
    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;
    Timer(Timer &&) = default;
    Timer &operator=(Timer &&) = default;
    ~Timer() = default;
    
    void insert(int fd_);
    void erase(int fd_);
    void update(int fd_);
    void handle();
    void setTimeout(int timeout_);
    void setCallback(const std::function<void(int)> &func_);
private:
    std::priority_queue<TimerNode, std::vector<TimerNode>, std::greater<TimerNode>> que;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> save;
    std::chrono::milliseconds interval;
    std::function<void(int)> callback;
};
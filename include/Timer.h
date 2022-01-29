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
    std::function<void()> callback;
    std::chrono::time_point<std::chrono::high_resolution_clock> tm;
    bool operator>(const TimerNode &tn) const{
        return tm > tn.tm;
    }
};

class Timer {
public:
    Timer(int timeout = 1000);
    void Add(int fd, const std::function<void()> &callback);
    void Update(int fd);
    void Handle();
    void SetTimeout(int timeout);
private:
    std::priority_queue<TimerNode, std::vector<TimerNode>, std::greater<TimerNode>> que;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> save;
    std::chrono::milliseconds interval;
};

#endif
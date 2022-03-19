#pragma once

#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<memory>
#include<condition_variable>
#include<functional>

const int maxn = 8;

class ThreadPool {
public:
    using ptr = std::shared_ptr<ThreadPool>;
    
    ThreadPool(int num = maxn);
    ~ThreadPool();
    
    void append(std::function<void()> task);
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex mut;
    std::condition_variable cond;
    bool stop;
};
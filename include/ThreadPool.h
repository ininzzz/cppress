#ifndef THREADPOOL_H
#define THREADPOOL_H

#include<vector>
#include<queue>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>

const int maxn = std::thread::hardware_concurrency() - 1;

class ThreadPool {
public:
    ThreadPool(int num = maxn);
    void append(std::function<void()> task);
    void run();
    void clear();
    ~ThreadPool();
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    std::mutex mut;
    std::condition_variable cond;
    bool stop;
};



#endif
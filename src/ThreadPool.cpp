#include"ThreadPool.h"

ThreadPool::ThreadPool(int num) : stop(false) {
    for (int i = 0;i < num;i++) {
        threads.emplace_back([this] {
            while(true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mut);
                    cond.wait(lock, [this] {
                        return stop || !tasks.empty();
                    });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    } 
}

void ThreadPool::append(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mut);
        if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task]() {task();});
    }
    cond.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mut);
        stop = true;
    }
    cond.notify_all();
    for (auto &&t : threads) {
        t.join();
    }
}
#include"ThreadPool.h"

ThreadPool::ThreadPool(int num) : stop(false) {
    for (int i = 0;i < num;i++) threads.emplace_back();
}

// template<typename F, typename... Args>
// void ThreadPool::append(F &&func, Args&&... args) {
//     using return_type = typename std::result_of<F(Args...)>::type;
//     auto task = std::bind(std::forward<F>(func), std::forward<Args>(args)...);
//     {
//         std::unique_lock<std::mutex> lock(mut);
//         if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
//         tasks.emplace([task]() {task();});
//     }
//     cond.notify_one();
// }

void ThreadPool::append(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(mut);
        if (stop) throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task]() {task();});
    }
    cond.notify_one();
}

void ThreadPool::run() {
    for (auto &&t : threads) {
        t = std::move(std::thread([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->mut);
                    cond.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        }));
    }
}

void ThreadPool::clear() {
    {
        std::unique_lock<std::mutex> lock(mut);
        while (!tasks.empty()) tasks.pop();
        stop = true;
    }
    cond.notify_all();
    for (auto &&t : threads) {
        if (t.joinable()) t.join();
    }
    stop = false;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(mut);
        stop = true;
    }
    cond.notify_all();
    for (auto &&t : threads) {
        if (t.joinable()) t.join();
    }
}
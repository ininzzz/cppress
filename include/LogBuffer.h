#pragma once


#include<sys/uio.h>
#include<vector>
#include<string>
#include<functional>
#include<list>
#include<memory>
#include<thread>
#include<condition_variable>
#include<cstring>
#include<unistd.h>

#include"Config.h"

class LogBuffer {
public:
    using ptr = std::shared_ptr<LogBuffer>;

    LogBuffer(int fd);

    void append(const std::string &str);

    int size() const { return m_size; }
    bool empty() const { return m_size == 0; }
    void clear() { m_buffer.clear(); }
    

private:
    void dump();
    static const int page_size = 4096;
    struct page {
        page() :rpos(0), wpos(0) {}
        char buf[page_size];
        int rpos, wpos;
    };
    std::list<page> m_buffer;
    std::list<page> m_buffer_temp;
    int m_size;
    int m_fd;

    std::thread m_producer;
    std::mutex m_mtx;
    std::condition_variable m_cond;
};

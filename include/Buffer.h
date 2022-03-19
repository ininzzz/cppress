#pragma once


#include<sys/uio.h>
#include<vector>
#include<string>
#include<functional>
#include<list>
#include<memory>
#include<cstring>
#include<unistd.h>

class Buffer {
public:
    using ptr = std::shared_ptr<Buffer>;
    
    Buffer() :m_size(0) {}
    
    void readFrom(int fd);
    void writeTo(int fd);

    void append(const std::string &str);
    
    char front();
    void pop();
    
    int size() { return m_size; }
    bool empty() { return m_size == 0; }
    void clear() { m_buffer.clear(); }
private:
    static const int page_size = 4096;
    struct page {
        page() :rpos(0), wpos(0) {}
        char buf[page_size];
        int rpos, wpos;
    };
    std::list<page> m_buffer;
    int m_size;
};

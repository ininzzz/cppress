#include"LogBuffer.h"

LogBuffer::LogBuffer(int fd) :
    m_size(0),
    m_fd(fd),
    m_producer([this] {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mtx);
        m_cond.wait(lock, [this] {return !empty() && size() > LOG_THRESHOLD;});
        m_buffer.swap(m_buffer_temp);
        lock.unlock();
        dump();
    }
}) {}

void LogBuffer::append(const std::string &str) {
    if (str.size() == 0) return;
    std::unique_lock<std::mutex> lock(m_mtx);
    for (auto &c : str) {
        if (!m_buffer.size() || m_buffer.back().wpos == page_size - 1) {
            m_buffer.emplace_back();
        }
        m_buffer.back().buf[m_buffer.back().wpos++] = c;
        m_size++;
    }
    m_cond.notify_one();
}

// #define m_buffer_temp m_buffer
void LogBuffer::dump() {
    char buf[page_size];
    while (!m_buffer_temp.empty()) {
        int buf_cnt = 0;
        for (int i = 0;i < m_buffer_temp.front().wpos;i++) {
            buf[buf_cnt++] = m_buffer_temp.front().buf[m_buffer_temp.front().rpos++];
            m_size--;
        }
        buf[buf_cnt] = '\0';
        ::write(m_fd, buf, buf_cnt);
        m_buffer_temp.pop_front();
    }
}





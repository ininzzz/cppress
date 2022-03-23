#include"Buffer.h"

void Buffer::append(const std::string &str) {
    for (auto &c : str) {
        if (!m_buffer.size() || m_buffer.back().wpos == page_size - 1) {
            m_buffer.emplace_back();
        }
        m_buffer.back().buf[m_buffer.back().wpos++] = c;
        m_size++;
    }
}

void Buffer::append(const char *buf, int len) {
    for (int i = 0;i < len;i++) {
        char c = buf[i];
        if (!m_buffer.size() || m_buffer.back().wpos == page_size - 1) {
            m_buffer.emplace_back();
        }
        m_buffer.back().buf[m_buffer.back().wpos++] = c;
        m_size++;
    }
}

void Buffer::readFrom(int fd) {
    char buf[page_size];
    int len = 0;
    while ((len = ::read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[len] = '\0';
        this->append(buf, len);
    }
    
}

void Buffer::writeTo(int fd) {
    char buf[page_size];
    while (!m_buffer.empty()) {
        int buf_cnt = 0;
        for (int i = 0;i < m_buffer.front().wpos;i++) {
            buf[buf_cnt++] = m_buffer.front().buf[m_buffer.front().rpos++];
            m_size--;
        }
        buf[buf_cnt] = '\0';
        ::write(fd, buf, buf_cnt);
        m_buffer.pop_front();
    }
}

char Buffer::front() {
    return m_buffer.front().buf[m_buffer.front().rpos];
}

void Buffer::pop() {
    if (!this->size()) throw std::runtime_error("Buffer pop error");
    m_buffer.front().rpos++;
    m_size--;
    if (m_buffer.front().rpos == m_buffer.front().wpos) {
        m_buffer.pop_front();
    }
}






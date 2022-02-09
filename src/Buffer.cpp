#include"Buffer.h"

Buffer::Buffer() :rpos(0), wpos(0) {}

int Buffer::Readable() {
    return wpos - rpos;
}

int Buffer::Writeable() {
    return buf.size() - wpos;
}

void Buffer::Append(const std::string &str) {
    int len = str.size();
    if (len > Writeable()) EnsureSpace(len);
    std::copy(str.begin(), str.end(), buf.begin() + wpos);
    wpos += len;
}

void Buffer::EnsureSpace(int len) {
    if (rpos + Writeable() >= len) {
        int readable_temp = Readable();
        std::copy(buf.begin() + rpos, buf.begin() + wpos, buf.begin());
        rpos = 0, wpos = rpos + readable_temp;
    }
    else {
        buf.resize(wpos + len);
    }
}

int Buffer::ReadFrom(int fd) {
    char buffer[65536];
    iovec iov[2];
    iov[0].iov_base = &buf[0] + wpos;
    iov[0].iov_len = Writeable();
    iov[1].iov_base = buffer;
    iov[1].iov_len = sizeof(buffer);
    int len = readv(fd, iov, 2);
    if (len < 0) throw std::runtime_error("Buffer Read error\n");
    if (len > Writeable()) {
        wpos += Writeable();
        buffer[len - Writeable()] = '\0';
        Append(buffer);
    }
    else {
        wpos += len;
    }
    return len;
}

int Buffer::WriteTo(int fd) {
    int readable_len = Readable();
    int len = write(fd, &buf[0] + rpos, readable_len);
    if (len < 0) throw std::runtime_error("Buffer Write error\n");
    rpos += len;
    return len;
}

std::string Buffer::GetBuf() {
    std::string str(&buf[0] + rpos, Readable());
    rpos = 0, wpos = 0;
    return str;
}

void Buffer::Clear() {
    rpos = 0;
    wpos = 0;
}
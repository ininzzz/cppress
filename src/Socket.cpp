#include"Socket.h"

void ServerSocket::listen(uint16_t port) {
    fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (fd == -1) throw std::runtime_error("server init error");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    sock_addr.sin_port = htons(port);

    int opt = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) == -1)
        throw std::runtime_error("server linger error");

    if (::bind(fd, (sockaddr *)&sock_addr, sizeof(sock_addr)) == -1)
        throw std::runtime_error("server bind error");
    
    if (::listen(fd, 4096) == -1)
        throw std::runtime_error("server listen error");
}

Socket ServerSocket::accept() {
    Socket conn;
    socklen_t len = sizeof(conn.sock_addr);
    conn.fd = ::accept(this->fd, (sockaddr *)&conn.sock_addr, &len);
    fcntl(conn.fd, F_SETFL, fcntl(conn.fd, F_GETFD, 0) | O_NONBLOCK);
    return conn;
}

ServerSocket::~ServerSocket() { close(fd); }

int ServerSocket::get_fd() { return fd; }


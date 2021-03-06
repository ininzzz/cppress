#pragma once

#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string>
#include<memory>
#include<functional>

#include"Config.h"

struct Socket {
    int fd;
    sockaddr_in sock_addr;
};

class ServerSocket {
public:
    using ptr = std::shared_ptr<ServerSocket>;
    ServerSocket() = default;
    ServerSocket(const ServerSocket &) = delete;
    ServerSocket &operator=(const ServerSocket &) = delete;
    ServerSocket(ServerSocket &&) = default;
    ServerSocket &operator=(ServerSocket &&) = default;
    ~ServerSocket();

    void listen(uint16_t port);
    Socket accept();
    int get_fd();
private:
    int fd;
    sockaddr_in sock_addr;
};
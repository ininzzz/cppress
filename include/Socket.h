#ifndef SOCKET_H
#define SOCKET_H

#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string>
#include<functional>

struct Socket {
    int fd;
    sockaddr_in sock_addr;
};

class ServerSocket {
public:
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

#endif
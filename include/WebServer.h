#ifndef WEBSERVER_H
#define WEBSERVER_H

#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include<unordered_map>
#include<list>
#include<functional>

#include"EventLoop.h"
#include"Router.h"
#include"TCPConnection.h"
#include"Socket.h"

class WebServer {
public:
    WebServer();
    void Listen(uint16_t port);
    void Get(const std::string &url, const std::function<void(Request&, Response&)> &func = nullptr);
    void Post(const std::string &url, const std::function<void(Request&, Response&)> &func = nullptr);
    ~WebServer();
private:
    // 回调函数
    void AcceptTask();
    void ReadTask(int fd_);
    void WriteTask(int fd_);
    void CloseTask(int fd_);
    void TimeoutTask(int fd_);
    // 关闭连接
    void CloseTCPConnection(int fd_);
    ServerSocket sock;
    uint32_t event;
    EventLoop loop;
    std::unordered_map<std::string, std::function<void(Request&, Response&)>> get, post;
    std::vector<TCPConnection> conn;
};



#endif
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
    using ptr = std::shared_ptr<WebServer>;
    WebServer();
    void listen(uint16_t port);
    void get(const std::string &url, const std::function<void(HttpRequest &, HttpResponse &)> &func = nullptr);
    void post(const std::string &url, const std::function<void(HttpRequest &, HttpResponse &)> &func = nullptr);
    ~WebServer();
private:
    // 回调函数
    void acceptTask();
    void readTask(int fd_);
    void writeTask(int fd_);
    void closeTask(int fd_);
    void timeoutTask(int fd_);
    // 关闭连接
    void closeTCPConnection(int fd_);
    ServerSocket::ptr sock;
    uint32_t event;
    EventLoop::ptr loop;
    std::unordered_map<std::string, std::function<void(HttpRequest &, HttpResponse &)>> m_get, m_post;
    std::vector<TCPConnection::ptr> conn;
};



#endif
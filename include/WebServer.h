#pragma once

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
#include"Config.h"

class WebServer {
public:
    using ptr = std::shared_ptr<WebServer>;
    using callback = std::function<void(HttpRequest::ptr, HttpResponse::ptr)>;
    WebServer();
    void listen(uint16_t port);
    void get(const std::string &path, const callback &func = nullptr);
    void post(const std::string &path, const callback &func = nullptr);
    // global
    void use(const callback &func) { m_global_middleware.push_back(func); }
    void use(const std::string &path, const callback &func) { m_routers[getHash(path)]->use(func); }
    // local
    void use(const std::string &path, Router::ptr router);
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

    int getHash(const std::string &str);
    ServerSocket::ptr sock;
    uint32_t event;
    EventLoop::ptr loop;
    // std::unordered_map<std::string, Router::ptr> m_router;
    std::unordered_map<int, Router::ptr> m_routers;
    std::vector<TCPConnection::ptr> conn;
    std::list<callback> m_global_middleware;
};
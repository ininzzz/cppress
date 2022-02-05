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

class WebServer {
public:
    WebServer();
    void Listen(uint16_t port);
    void Use(const Router &r);
    void Get(const std::string &url, const std::function<void(Request&, Response&)> &func = nullptr);
    void Post(const std::string &url, const std::function<void(Request&, Response&)> &func = nullptr);
    ~WebServer();
private:
    void AcceptTask();
    void ReadTask(int fd_);
    void WriteTask(int fd_);
    void CloseTask(int fd_);
    void TimeoutTask(int fd_);
    int sockfd;
    uint32_t event;
    std::vector<Router> router;
    EventLoop loop;
    std::unordered_map<std::string, std::function<void(Request&, Response&)>> get, post;
    std::unordered_map<int, TCPConnection > conn;
};



#endif
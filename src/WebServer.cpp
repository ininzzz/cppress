#include"WebServer.h"

WebServer::WebServer() {}

WebServer::~WebServer() {
    close(sockfd);
}

void WebServer::Listen(uint16_t port) {
    int ret;

    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (sockfd == -1) throw std::runtime_error("server init error");

    // linger linger_;
    // linger_.l_onoff = 1;
    // linger_.l_linger = 1;
    // ret = setsockopt(sockfd, SOL_SOCKET, SO_LINGER, &linger_, sizeof(linger_));
    // if (ret == -1) throw std::runtime_error("server linger error");

    sockaddr_in sock;
    memset(&sock, 0, sizeof(sock));
    sock.sin_family = AF_INET;
    // 0.0.0.0对公网开放，127.0.0.1是回环地址，只适用于本机测试
    sock.sin_addr.s_addr = inet_addr("0.0.0.0");
    sock.sin_port = htons(port);

    ret = bind(sockfd, (sockaddr *)&sock, sizeof(sock));
    if (ret == -1) throw std::runtime_error("server bind error");

    ret = listen(sockfd, MAX_FD);
    if (ret == -1) throw std::runtime_error("server listen error");

    event = EPOLLONESHOT | EPOLLRDHUP;
    loop.AddEvent(sockfd, EPOLLIN | EPOLLONESHOT);

    loop.Set_AcceptCallBack(sockfd, [this] {
        this->AcceptTask();
    });
    loop.Set_ReadCallBack([this](int fd_) {
        this->ReadTask(fd_);
    });
    loop.Set_WriteCallBack([this](int fd_) {
        this->WriteTask(fd_);
    });
    loop.Set_CloseCallBack([this](int fd_) {
        this->CloseTask(fd_);
    });
    loop.Set_TimeoutCallBack([this](int fd_) {
        this->TimeoutTask(fd_);
    });

    loop.SetTimeout(5000);
    loop.Loop();
}

void WebServer::AcceptTask() {
    sockaddr_in temp;
    socklen_t len = sizeof(temp);
    int connfd = accept(sockfd, (sockaddr *)&temp, &len);
    if (connfd == -1) throw std::runtime_error("client accept error");
    // printf("new connection!\n");
    // printf("sockfd is %d!\n", connfd);

    conn[connfd].init(connfd, temp, &loop);
    loop.AddEvent(connfd, EPOLLIN | event);
    loop.ModEvent(sockfd, EPOLLIN | EPOLLONESHOT);
}

void WebServer::ReadTask(int fd_) {
    // printf("read task!\n");

    Request &req = conn[fd_].GetRequest();
    Response &res = conn[fd_].GetResponse();
    if (!conn[fd_].NeedClose()) loop.AddTimeoutTask(fd_);
    for (auto r : router) {
        r.init(req, res);
    }

    if (req.method() == HTTP_METHOD::GET) {
        if (get.count(req.url())) {
            get[req.url()](req, res);
        }
    }
    else if (req.method() == HTTP_METHOD::POST) {
        if (post.count(req.url())) {
            post[req.url()](req, res);
        }
    }
    
    if (conn[fd_].NeedWrite()) {
        loop.ModEvent(fd_, EPOLLOUT | event);
    }
}

void WebServer::WriteTask(int fd_) {
    // printf("write task!\n");
    conn[fd_].DealResponse();
    if (conn[fd_].NeedWrite()) {
        loop.ModEvent(fd_, EPOLLOUT | event);
    }
    else {
        if (conn[fd_].NeedClose()) {
            conn.erase(fd_);
        }
        else {
            loop.ModEvent(fd_, EPOLLIN | event);
        }
    }
}

void WebServer::CloseTask(int fd_) {
    conn.erase(fd_);
}

void WebServer::TimeoutTask(int fd_) {
    printf("%d is closed because of timeout\n", fd_);
    conn.erase(fd_);
}

void WebServer::Use(const Router &r) {
    router.push_back(r);
}

void WebServer::Get(const std::string &url, const std::function<void(Request &, Response &)> &func) {
    get[url] = func;
}

void WebServer::Post(const std::string &url, const std::function<void(Request &, Response &)> &func) {
    post[url] = func;
}

#include"WebServer.h"

WebServer::WebServer() :conn(MAX_FD) {}

WebServer::~WebServer() {}

void WebServer::Listen(uint16_t port) {
    sock.listen(port);

    event = EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    loop.AddEvent(sock.get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);

    loop.Set_AcceptCallBack(sock.get_fd(), [this] {
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
    Socket new_conn;
    while ((new_conn = sock.accept()).fd != -1) {
        printf("new connection! fd is %d\n", new_conn.fd);
        conn[new_conn.fd].init(new_conn, &loop);
        loop.AddEvent(new_conn.fd, EPOLLIN | event);
    }
    loop.ModEvent(sock.get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void WebServer::ReadTask(int fd_) {
    printf("read task!\n");

    Request &req = conn[fd_].GetRequest();
    Response &res = conn[fd_].GetResponse();
    
    if (!conn[fd_].NeedClose()) {
        loop.AddTimeoutTask(fd_);
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
        // WriteTask(fd_);
        loop.ModEvent(fd_, EPOLLOUT | event);
    }
}

void WebServer::WriteTask(int fd_) {
    printf("write task!\n");
    conn[fd_].DealResponse();
    if (conn[fd_].NeedWrite()) {
        loop.ModEvent(fd_, EPOLLOUT | event);
    }
    else {
        if (conn[fd_].NeedClose()) {
            Close(fd_);
        }
        else {
            loop.ModEvent(fd_, EPOLLIN | event);
        }
    }
}

void WebServer::Close(int fd_) {
    printf("%d is closed\n", fd_);
    // conn.erase(fd_);
    // conn[fd_].~TCPConnection();
    loop.DeleteEvent(fd_);
    ::close(fd_);
}

void WebServer::CloseTask(int fd_) {
    Close(fd_);
}

void WebServer::TimeoutTask(int fd_) {
    // printf("%d is closed because of timeout\n", fd_);
    Close(fd_);
}

void WebServer::Get(const std::string &url, const std::function<void(Request &, Response &)> &func) {
    get[url] = func;
}

void WebServer::Post(const std::string &url, const std::function<void(Request &, Response &)> &func) {
    post[url] = func;
}

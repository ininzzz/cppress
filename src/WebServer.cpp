#include"WebServer.h"

WebServer::WebServer()
    :loop(new EventLoop),sock(new ServerSocket) {
    for (int i = 0;i < MAX_FD;i++) {
        conn.push_back(TCPConnection::ptr(new TCPConnection(i)));
    }
}

WebServer::~WebServer() {}

void WebServer::listen(uint16_t port) {
    sock->listen(port);
    
    event = EPOLLET | EPOLLRDHUP | EPOLLONESHOT;
    loop->addEvent(sock->get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
    
    loop->setAcceptCallBack(sock->get_fd(), [this] {
        this->acceptTask();
    });
    loop->setReadCallBack([this](int fd_) {
        this->readTask(fd_);
    });
    loop->setWriteCallBack([this](int fd_) {
        this->writeTask(fd_);
    });
    loop->setCloseCallBack([this](int fd_) {
        this->closeTask(fd_);
    });
    loop->setTimeoutCallBack([this](int fd_) {
        this->timeoutTask(fd_);
    });
    
    loop->setTimeout(5000);
    loop->loop();
}

void WebServer::acceptTask() {
    Socket new_conn;
    while ((new_conn = sock->accept()).fd != -1) {
        printf("new connection! fd is %d\n", new_conn.fd);
        loop->addEvent(new_conn.fd, EPOLLIN | event);
    }
    loop->modEvent(sock->get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void WebServer::readTask(int fd_) {
    // printf("read task!\n");

    HttpRequest &req = conn[fd_]->getRequest();
    HttpResponse &res = conn[fd_]->getResponse();
    if (!conn[fd_]->validRequest()) return;

    if (!conn[fd_]->needClose()) loop->addTimeoutTask(fd_);
    else loop->eraseFromTimer(fd_);

    if (req.method() == HttpMethod::GET) {
        if (m_get.count(req.url())) {
            m_get[req.url()](req, res);
        }
    }
    else if (req.method() == HttpMethod::POST) {
        if (m_post.count(req.url())) {
            m_post[req.url()](req, res);
        }
    }
    if (conn[fd_]->needWrite()) loop->modEvent(fd_, EPOLLOUT | event);
    else loop->modEvent(fd_, EPOLLIN | event);
}

void WebServer::writeTask(int fd_) {
    printf("write task!\n");
    conn[fd_]->dealResponse();
    if (conn[fd_]->needWrite()) {
        loop->modEvent(fd_, EPOLLOUT | event);
    }
    else {
        if (conn[fd_]->needClose()) {
            closeTCPConnection(fd_);
        }
        else {
            if (conn[fd_]->needRead()) {
                readTask(fd_);
            }
            else {
                loop->modEvent(fd_, EPOLLIN | event);
            }
        }
    }
}

void WebServer::closeTCPConnection(int fd_) {
    printf("%d is closed\n", fd_);
    loop->delEvent(fd_);
    loop->eraseFromTimer(fd_);
    conn[fd_]->clear();
    ::close(fd_);
}

void WebServer::closeTask(int fd_) {
    closeTCPConnection(fd_);
}

void WebServer::timeoutTask(int fd_) {
    // printf("%d is closed because of timeout\n", fd_);
    closeTCPConnection(fd_);
}

void WebServer::get(const std::string &url, const std::function<void(HttpRequest &, HttpResponse &)> &func) {
    m_get[url] = func;
}

void WebServer::post(const std::string &url, const std::function<void(HttpRequest &, HttpResponse &)> &func) {
    m_post[url] = func;
}

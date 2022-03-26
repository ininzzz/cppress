#include"WebServer.h"

WebServer::WebServer()
    :loop(new EventLoop), sock(new ServerSocket) {
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
        // printf("new connection! fd is %d\n", new_conn.fd);
        loop->addEvent(new_conn.fd, EPOLLIN | event);
    }
    loop->modEvent(sock->get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void WebServer::readTask(int fd_) {
    // printf("read task!\n");

    HttpRequest::ptr req = conn[fd_]->getRequest();
    HttpResponse::ptr res = conn[fd_]->getResponse();
    if (!conn[fd_]->validRequest()) return;

    if (!conn[fd_]->needClose()) loop->addTimeoutTask(fd_);
    else loop->eraseFromTimer(fd_);

    for (auto &cb : m_global_middleware) {
        cb(req, res);
    }

    const std::string &url = req->path();
    int base = 233, prefix = 0;
    for (int i = 0;i < url.size();i++) {
        prefix = prefix * base + url[i];
        if (i + 1 == url.size() || url[i + 1] == '/'){
            if (m_routers.count(prefix)) {
                std::string suffix;
                if (i + 1 == url.size()) suffix.push_back('/');
                else suffix.append(url.begin() + i + 1, url.end());
                if (req->method() == HttpMethod::GET && m_routers[prefix]->validGet(suffix)) {
                    m_routers[prefix]->processMiddleware();
                    m_routers[prefix]->processGet(suffix)(req, res);
                } else if (req->method() == HttpMethod::POST && m_routers[prefix]->validPost(suffix)) {
                    m_routers[prefix]->processMiddleware();
                    m_routers[prefix]->processPost(suffix)(req, res);
                }
                break;
            }
        }
    }
    if (conn[fd_]->needWrite()) loop->modEvent(fd_, EPOLLOUT | event);
    else loop->modEvent(fd_, EPOLLIN | event);
}

void WebServer::writeTask(int fd_) {
    // printf("write task!\n");
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
    // printf("%d is closed\n", fd_);
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

void WebServer::get(const std::string &path, const callback &func) {
    int hash_value = getHash(path);
    if (m_routers[hash_value] == nullptr) m_routers[hash_value].reset(new Router(path));
    m_routers[hash_value]->get("/", func);
}

void WebServer::post(const std::string &path, const callback &func) {
    int hash_value = getHash(path);
    if (m_routers[hash_value] == nullptr) m_routers[hash_value].reset(new Router(path));
    m_routers[hash_value]->post("/", func);
}

void WebServer::use(const std::string &path, Router::ptr router) {
    int hash_value = getHash(path);
    if (m_routers[hash_value] == nullptr) m_routers[hash_value] = router;
    m_routers[hash_value]->set(path);
}

int WebServer::getHash(const std::string &str) {
    int base = 233, ans = 0;
    for (auto &c : str) {
        ans = ans * base + c;
    }
    return ans;
}
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
    
    loop->setTimeout(TIME_OUT);
    loop->loop();
}

void WebServer::acceptTask() {
    Socket new_conn;
    // use while because of ET
    while ((new_conn = sock->accept()).fd != -1) {
        // printf("new connection! fd is %d\n", new_conn.fd);
        loop->addEvent(new_conn.fd, EPOLLIN | event);
    }
    loop->modEvent(sock->get_fd(), EPOLLIN | EPOLLET | EPOLLONESHOT);
}

void WebServer::readTask(int fd_) {

    HttpRequest::ptr req = conn[fd_]->getRequest();
    HttpResponse::ptr res = conn[fd_]->getResponse();
    if (!conn[fd_]->validRequest()) {
        res->sendStatus(HttpStatus::BAD_REQUEST);
        return;
    }

    if (!conn[fd_]->needClose()) loop->addTimeoutTask(fd_);
    else loop->eraseFromTimer(fd_);

    for (auto &cb : m_global_middleware) {
        if (!cb(req, res)) {
            if (conn[fd_]->needWrite()) loop->modEvent(fd_, EPOLLOUT | event);
            else loop->modEvent(fd_, EPOLLIN | event);
            return;
        }
    }

    std::string url = req->path();
    // "/student/info" -> "/student/info/"
    if (url.back() != '/') url.push_back('/');

    int base = hash_base, prefix = 0;
    bool ok = false;
    for (int i = 0;i < url.size();i++) {
        prefix = prefix * base + url[i];
        if (url[i] == '/' && m_routers.count(prefix)) {
            std::string suffix(url.begin() + i, url.end());
            if (req->method() == HttpMethod::GET && m_routers[prefix]->validGet(suffix)) {
                ok = true;
                if (m_routers[prefix]->processMiddleware(req, res)) {
                    m_routers[prefix]->processGet(suffix)(req, res);
                    break;
                }
            }
            else if (req->method() == HttpMethod::POST && m_routers[prefix]->validPost(suffix)) {
                ok = true;
                if (m_routers[prefix]->processMiddleware(req, res)) {
                    m_routers[prefix]->processPost(suffix)(req, res);
                    break;
                }
            }
        }
    }
    if (!ok) res->sendStatus(HttpStatus::NOT_FOUND);
    if (conn[fd_]->needWrite()) loop->modEvent(fd_, EPOLLOUT | event);
    else loop->modEvent(fd_, EPOLLIN | event);
}

void WebServer::writeTask(int fd_) {
    // printf("write task!\n");
    // send to client
    conn[fd_]->dealResponse();
    // write buffer is still not empty, then register EPOLLOUT
    if (conn[fd_]->needWrite()) loop->modEvent(fd_, EPOLLOUT | event);
    else {
        // if connection:close , then close connection
        if (conn[fd_]->needClose()) closeTCPConnection(fd_);
        else {
            // read buffer is still not empty, then deal with read task
            if (conn[fd_]->needRead()) readTask(fd_);
            else loop->modEvent(fd_, EPOLLIN | event);
        }
    }
}

void WebServer::closeTCPConnection(int fd_) {
    // prework
    loop->delEvent(fd_);
    loop->eraseFromTimer(fd_);
    conn[fd_]->clear();
    
    // close fd
    ::close(fd_);
    // printf("%d is closed\n", fd_);
}

void WebServer::closeTask(int fd_) {
    closeTCPConnection(fd_);
}

void WebServer::timeoutTask(int fd_) {
    // printf("%d is closed because of timeout\n", fd_);
    closeTCPConnection(fd_);
}

// add callback with GET method
void WebServer::get(const std::string &path, const callback &func) {
    // add callback to router with root path
    int hash_value = getHash("/");
    if (m_routers[hash_value] == nullptr) m_routers[hash_value].reset(new Router(path));
    m_routers[hash_value]->get(path, func);
}

// add callback with POST method
void WebServer::post(const std::string &path, const callback &func) {
    // add callback to router with root path
    int hash_value = getHash("/");
    if (m_routers[hash_value] == nullptr) m_routers[hash_value].reset(new Router(path));
    m_routers[hash_value]->post(path, func);
}

// global middleware
void WebServer::use(const middleware &func) {
    m_global_middleware.push_back(func);
}

// local middleware
void WebServer::use(const std::string &path, const middleware &func) {
    // prework
    std::string now = path;
    if (now.back() != '/') now.push_back('/');
    
    // find router
    int val = getHash(now);
    auto itr = m_routers.find(val);
    
    // if not found, then return
    if (itr == m_routers.end()) return;
    
    // add middleware to router with 'path'
    itr->second->use(func);
}

// router
void WebServer::use(const std::string &path, Router::ptr router) {
    // prework
    std::string now = path;
    if (now.back() != '/') now.push_back('/');

    // find router
    int val = getHash(now);
    auto itr = m_routers.find(val);

    // if not found, then return
    if (itr != m_routers.end()) return;

    // mount router with 'path'
    m_routers.insert({ val, router });
    m_routers[val]->set(now);
}

// get hash value
int WebServer::getHash(const std::string &str) {
    int base = hash_base, ans = 0;
    for (auto &c : str) {
        ans = ans * base + c;
    }
    return ans;
}


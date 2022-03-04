#include"WebServer.h"

WebServer::WebServer()
    :loop(new EventLoop), sock(new ServerSocket), conn(MAX_FD) {
        // for(int i=0;i<MAX_FD;i++){
        //     conn[i].reset(new TCPConnection(i));
        // }
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
        // std::lock_guard<std::mutex> lock(mtx);
        // printf("new connection! fd is %d\n", new_conn.fd);
        
        if (free_conn.size()) {
            conn[new_conn.fd] = std::move(free_conn.front());
            conn[new_conn.fd]->reset(new_conn.fd);
            // conn[new_conn.fd].reset(new TCPConnection(new_conn.fd));
            free_conn.pop_front();
        }
        else {
            conn[new_conn.fd].reset(new TCPConnection(new_conn.fd));
        }
        
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

    for (auto cb : m_global_callback) {
        cb(req, res);
    }

    const std::string &url = req->url();
    std::string prefix;
    for (int i = 0;;i++) {
        if (url[i] == '/' || i == url.size()) {
            std::cout << "prefix: " << prefix << std::endl;
            if (m_router.count(prefix)) {
                std::string suffix;
                if (i == url.size()) suffix.push_back('/');
                else suffix.append(url.begin() + i, url.end());
                std::cout << "suffix: " << suffix << std::endl;
                if (req->method() == HttpMethod::GET && m_router[prefix]->validGet(suffix)) {
                    // printf("ok!\n");
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        m_router[prefix]->set(req, res);
                    }
                    m_router[prefix]->processGet(suffix);
                } else if (req->method() == HttpMethod::POST && m_router[prefix]->validPost(suffix)) {
                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        m_router[prefix]->set(req, res);
                    }
                    m_router[prefix]->processPost(suffix);
                }
            }
            if (i == url.size()) break;
        }
        prefix.push_back(url[i]);
    }
    
    // if (m_router.count(req->url())) {
    //     const std::string &router_url = m_router[req->url()]->url();
    //     std::string url;
    //     int id = 0;
    //     std::cout << "router url: " << router_url << std::endl;
    //     std::cout << "req url: " << req->url() << std::endl;
    //     while (router_url[id] == req->url()[id]) id++;
    //     if (id < req->url().size()) url.assign(req->url().begin() + id, req->url().end());
    //     else url.push_back('/');
    //     std::cout << "url: " << url << std::endl;
    //     if (req->method() == HttpMethod::GET) {
    //         m_router[req->url()]->set(req, res);
    //         m_router[req->url()]->processGet(url);
    //     }
    //     else if (req->method() == HttpMethod::POST) {
    //         m_router[req->url()]->set(req, res);
    //         m_router[req->url()]->processPost(url);
    //     }
    // }
    
    // if (req->method() == HttpMethod::GET) {
    //     if (m_router.count(req->url())) {
    //         std::lock_guard<std::mutex> lock(mtx);
    //         m_router[req->url()]->set(req, res);
    //         m_router[req->url()]->processGet("/");
    //     }
    // }
    // else if (req->method() == HttpMethod::POST) {
    //     if (m_router.count(req->url())) {
    //         std::lock_guard<std::mutex> lock(mtx);
    //         m_router[req->url()]->set(req, res);
    //         m_router[req->url()]->processGet("/");
    //     }
    // }
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
    {
        std::lock_guard<std::mutex> lock(mtx);
        free_conn.push_back(std::move(conn[fd_]));
    }
    ::close(fd_);
}

void WebServer::closeTask(int fd_) {
    closeTCPConnection(fd_);
}

void WebServer::timeoutTask(int fd_) {
    // printf("%d is closed because of timeout\n", fd_);
    closeTCPConnection(fd_);
}

void WebServer::get(const std::string &url, const callback &func) {
    if(m_router[url]==nullptr) m_router[url].reset(new Router(url));
    m_router[url]->get("/", func);
}

void WebServer::post(const std::string &url, const callback &func) {
    if (m_router[url] == nullptr) m_router[url].reset(new Router(url));
    m_router[url]->post("/", func);
}

void WebServer::use(const std::string &url, Router::ptr router) {
    if (m_router[url] == nullptr) m_router[url] = router;
    m_router[url]->setUrl(url);
}
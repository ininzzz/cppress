#include"WebServer.h"

WebServer::WebServer() {}

WebServer::~WebServer() {
    close(sockfd);
    for (auto &&cc : conn) close(cc.first);
}

void WebServer::Listen(uint16_t port) {
    int ret;
    // 监听连接
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

    // 注册监听事件
    loop.AddReadEvent(sockfd, [this] {
        sockaddr_in temp;
        socklen_t len = sizeof(temp);
        // 建立新连接
        int connfd = accept(this->sockfd, (sockaddr *)&temp, &len);
        if (connfd == -1) throw std::runtime_error("client accept error");
        printf("new connection!\n");
        printf("sockfd is %d!\n", connfd);
        // 处理新连接(初始化数据+注册超时/读事件)
        this->conn[connfd].init(connfd, temp, &(this->loop));

        this->loop.AddTimeout(connfd, [connfd, this] {
            this->loop.DeleteEvent(connfd);
            if (close(connfd) < 0) return;
            printf("%d is closed\n", connfd);
        });

        this->loop.AddReadEvent(connfd, [connfd, this] {
            this->loop.UpdateTimeout(connfd);
            // 拿到对应socket的req,res数据
            Request &req = this->conn[connfd].GetRequest();
            Response &res = this->conn[connfd].GetResponse();
            // 按加入顺序过一遍中间件
            for (auto r : this->router) {
                r.init(req, res);
            }
            // 处理get or post请求
            if (req.method() == HTTP_METHOD::GET) {
                if (this->get.count(req.url())) {
                    this->get[req.url()](req, res);
                }
            }
            else if (req.method() == HTTP_METHOD::POST) {
                if (this->post.count(req.url())) {
                    this->post[req.url()](req, res);
                }
            }
            // 重新激活epolloneshot
            this->conn[connfd].DealResponse();
        });
        // 重新激活epolloneshot
        this->loop.RestartEvent(this->sockfd);
    });

    loop.SetTimeout(5000);
    loop.Loop();
}

void WebServer::Use(const Router &r) {
    // 加入中间件
    router.push_back(r);
}

void WebServer::Get(const std::string &url, const std::function<void(Request &, Response &)> &func) {
    // get对应url的处理函数
    get[url] = func;
}

void WebServer::Post(const std::string &url, const std::function<void(Request &, Response &)> &func) {
    // post对应url的处理函数
    post[url] = func;
}

#include"TCPConnection.h"

TCPConnection::TCPConnection() {}

void TCPConnection::init(int sockfd_, sockaddr_in sock_, EventLoop *loop_) {
    req.sockfd = res.sockfd = sockfd = sockfd_;
    req._ip = inet_ntoa(sock_.sin_addr);
    req._port = ntohs(sock_.sin_port);
    // 设置socket非阻塞
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    loop = loop_;
}

Request &TCPConnection::GetRequest() {
    // 得到完整的http请求
    if (req.ReadFrom() == HTTP_CODE::ERROR) throw std::runtime_error("parse error\n");
    return req;
}

Response &TCPConnection::GetResponse() {
    return res;
}

void TCPConnection::DealResponse() {
    loop->AddWriteEvent(sockfd, [this] {
        if (this->res.buf.Readable()) {
            this->res.buf.WriteTo(this->sockfd);
        }
        loop->RestartEvent(this->sockfd);
    });
}

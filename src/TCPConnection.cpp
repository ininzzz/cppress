#include"TCPConnection.h"

TCPConnection::TCPConnection() {}

void TCPConnection::init(int sockfd_, sockaddr_in sock_, EventLoop *loop_) {
    req.sockfd = res.sockfd = sockfd = sockfd_;
    req._ip = inet_ntoa(sock_.sin_addr);
    req._port = ntohs(sock_.sin_port);
    // 设置socket非阻塞
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    loop = loop_;
    is_closed = false;
}

Request &TCPConnection::GetRequest() {
    // 得到完整的http请求
    HTTP_CODE ret = req.ReadFrom();
    if (ret == HTTP_CODE::ERROR) throw std::runtime_error("parse error\n");
    return req;
}

Response &TCPConnection::GetResponse() {
    res.Init(loop);
    return res;
}

bool TCPConnection::NeedWrite() {
    return res.need_write;
}

bool TCPConnection::NeedClose() {
    return req.connection();
}

void TCPConnection::DealResponse() {
    res.SendTo();
}

bool TCPConnection::IsClosed() {
    return is_closed;
}

void TCPConnection::Close() {
    loop->DeleteEvent(sockfd);
    shutdown(sockfd, SHUT_WR);
    int ret = close(sockfd);
    if (ret == -1) throw std::runtime_error("close error\n");
    is_closed = true;
    // printf("%d is closed\n", sockfd);
}

TCPConnection::~TCPConnection() {
    if (!IsClosed()) {
        Close();
    }
}

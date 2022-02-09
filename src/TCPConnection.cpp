#include"TCPConnection.h"

void TCPConnection::init(Socket sock_, EventLoop *loop_) {
    sock = std::move(sock_);
    loop = loop_;
    req.sockfd = res.sockfd = sock.fd;
    req._ip = inet_ntoa(sock.sock_addr.sin_addr);
    req._port = ntohs(sock.sock_addr.sin_port);
}

Request &TCPConnection::GetRequest() {
    HTTP_CODE ret;
    while ((ret = req.ReadFrom()) == HTTP_CODE::OPEN);
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

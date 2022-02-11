#include"TCPConnection.h"

void TCPConnection::init(Socket sock_, EventLoop *loop_) {
    req.clear();res.clear();
    sock = sock_;
    loop = loop_;
    valid_request = true;
    need_read = need_close = false;
    req.sockfd = res.sockfd = sock.fd;
    req._ip = inet_ntoa(sock.sock_addr.sin_addr);
    req._port = ntohs(sock.sock_addr.sin_port);
}

Request &TCPConnection::GetRequest() {
    HTTP_CODE ret = req.get();
    if (ret == HTTP_CODE::OPEN) valid_request = false;
    else if (ret == HTTP_CODE::PARTLY_DONE) need_read = true;
    else if (ret == HTTP_CODE::ERROR) throw std::runtime_error("parse error\n");
    
    if (req.connection() == HTTP_CONNECTION::CLOSE) need_close = true;
    return req;
}

Response &TCPConnection::GetResponse() {
    return res;
}

void TCPConnection::DealResponse() {
    res.send();
}

void TCPConnection::clear() {
    req.clear();
    res.clear();
}
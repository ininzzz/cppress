#ifndef REQUEST_H
#define REQUEST_H

#include"Buffer.h"
#include"HttpParser.h"

class Request {
public:
    Request();

    uint16_t port() { return _port; }
    std::string ip() { return _ip; }
    std::string url() { return parser.url(); }
    std::string host() { return parser.host(); }
    HTTP_METHOD method() { return parser.method(); }
    HTTP_VERSION version() { return parser.version(); }
private:
    friend class TCPConnection;
    HTTP_CODE ReadFrom();

    int sockfd;
    Buffer buf;

    // info
    uint16_t _port;
    std::string _ip;
    
    HttpParser parser;
};



#endif
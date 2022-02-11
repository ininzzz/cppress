#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include<unistd.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>
#include<string>
#include<vector>
#include<map>

#include"EventLoop.h"
#include"Request.h"
#include"Response.h"
#include"Socket.h"

class TCPConnection {
public:
    TCPConnection() = default;
    TCPConnection(const TCPConnection &) = delete;
    TCPConnection &operator=(const TCPConnection &) = delete;
    TCPConnection(TCPConnection &&) = default;
    TCPConnection &operator=(TCPConnection &&) = default;
    ~TCPConnection() = default;
    
    void init(Socket sock_, EventLoop *loop_);
    void clear();
    Request &GetRequest();
    Response &GetResponse();

    bool ValidRequest() { return valid_request; }
    bool NeedRead() { return need_read; }
    bool NeedWrite() { return res.need_write; }
    bool NeedClose() { return need_close; }
    
    void DealResponse();
private:
    Socket sock;
    Request req;
    Response res;
    EventLoop *loop;

    // check
    bool valid_request;
    bool need_read;
    bool need_close;
};


#endif
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
    Request &GetRequest();
    Response &GetResponse();
    
    bool NeedWrite();
    bool NeedClose();
    void DealResponse();
private:
    Socket sock;
    Request req;
    Response res;
    EventLoop *loop;
};


#endif
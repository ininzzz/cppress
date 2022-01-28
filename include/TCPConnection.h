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

class TCPConnection {
public:
    TCPConnection();
    void init(int sockfd_, sockaddr_in sock_, EventLoop *loop_);
    Request &GetRequest();
    Response &GetResponse();
    void DealResponse();
private:
    int sockfd;
    Request req;
    Response res;
    EventLoop *loop;
};


#endif
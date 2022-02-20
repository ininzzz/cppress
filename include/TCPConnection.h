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

#include"HttpRequest.h"
#include"HttpResponse.h"
#include"Socket.h"

class TCPConnection {
public:
    using ptr = std::shared_ptr<TCPConnection>;
    
    TCPConnection(int fd)
        :m_req(new HttpRequest(fd)), m_res(new HttpResponse(fd)), m_fd(fd) {
    }

    void clear();
    HttpRequest &getRequest();
    HttpResponse &getResponse();

    bool needClose() { return m_req->isclose(); }
    bool needRead() { return m_req->m_buffer->size() > 0; }
    bool needWrite() { return m_res->m_buffer->size() > 0; }
    bool validRequest() { return isvaild; }
    void dealResponse() { m_res->send(); }

private:
    int m_fd;
    HttpRequest::ptr m_req;
    HttpResponse::ptr m_res;
    bool isvaild = false;
};


#endif
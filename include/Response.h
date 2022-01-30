#ifndef RESPONSE_H
#define RESPONSE_H

#include"Buffer.h"
#include"HttpEnum.h"
#include"EventLoop.h"

class Response {
public:
    void Send(const std::string &msg);
    void SetStatus(HTTP_STATUS status_);
    void SetContentType(HTTP_CONTENT_TYPE type_);
private:
    friend class TCPConnection;
    void Init(EventLoop *loop_);
    void SendTo();
    int sockfd;
    Buffer buf;
    EventLoop *loop;

    HTTP_STATUS status;
    HTTP_CONTENT_TYPE type;
};



#endif
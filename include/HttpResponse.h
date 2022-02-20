#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include<memory>
#include<map>
#include<cstring>

#include"Http.h"
#include"Buffer.h"

class HttpResponse {
public:
    friend class TCPConnection;
    using ptr = std::shared_ptr<HttpResponse>;
    
    HttpResponse(int fd) :m_buffer(new Buffer), m_fd(fd) {};
    
    void setVersion(HttpVersion version) { m_version = version; }
    void setStatus(HttpStatus status) { m_status = status; }
    void setHeader(const std::string &key, const std::string &val) { m_headers[key] = val; }
    void send(const std::string &msg);
    void clear();
private:
    void send() { m_buffer->writeTo(m_fd); }
private:
    int m_fd;
    HttpVersion m_version = HttpVersion::HTTP_1_1;
    HttpStatus m_status = HttpStatus::OK;

    std::map<std::string, std::string> m_headers;
    std::string m_body;

    Buffer::ptr m_buffer;
};


#endif
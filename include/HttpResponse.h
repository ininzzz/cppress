#pragma once

#include<memory>
#include<map>
#include<cstring>
#include<iomanip>
#include<iostream>
#include<sstream>
#include<chrono>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>

#include"Http.h"
#include"Buffer.h"
#include"Json.h"

class HttpResponse {
public:
    friend class TCPConnection;
    using ptr = std::shared_ptr<HttpResponse>;
    
    HttpResponse(int fd) :m_buffer(new Buffer), m_fd(fd) {};
    
    void setVersion(HttpVersion version) { m_version = version; }
    void setStatus(HttpStatus status) { m_status = status; }
    void setHeader(const std::string &key, const std::string &val) { m_headers[key] = val; }

    HttpVersion getVersion() { return m_version; }
    HttpStatus getStatus() { return m_status; }
    const std::string &getHeader(const std::string &header);
    
    void send(const std::string &msg);
    void sendFile(const std::string &path);
    void sendFile(int fd);
    void sendJson(const Json &json);
    void sendStatus(HttpStatus status);
    void clear();
private:
    void makeHeader(int content_length);
    void send() { m_buffer->writeTo(m_fd); }
private:
    int m_fd;
    HttpVersion m_version = HttpVersion::HTTP_1_1;
    HttpStatus m_status = HttpStatus::OK;

    std::map<std::string, std::string> m_headers;
    std::string m_body;

    Buffer::ptr m_buffer;
};
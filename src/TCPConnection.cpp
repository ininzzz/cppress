#include"TCPConnection.h"

HttpRequest &TCPConnection::getRequest() {
    ParseCode ret = m_req->parse();
    if (ret == ParseCode::OK) isvaild = true;
    return *m_req.get();
}

HttpResponse &TCPConnection::getResponse() {
    return *m_res.get();
}

void TCPConnection::clear() {
    m_req->clear();
    m_res->clear();
    isvaild = false;
}
#include"TCPConnection.h"

HttpRequest::ptr TCPConnection::getRequest() {
    ParseCode ret = m_req->parse();
    if (ret == ParseCode::OK) isvaild = true;
    return m_req;
}

HttpResponse::ptr TCPConnection::getResponse() {
    return m_res;
}

void TCPConnection::clear() {
    m_req->clear();
    m_res->clear();
    isvaild = false;
}
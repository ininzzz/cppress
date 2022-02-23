#include"HttpResponse.h"

const std::string &HttpResponse::getHeader(const std::string &header) {
    auto it = m_headers.find(header);
    if (it == m_headers.end()) throw std::runtime_error("unknown header");
    return it->second;
}

void HttpResponse::send(const std::string &msg) {
    
    m_buffer->append(toString(m_version));
    m_buffer->append(" ");
    m_buffer->append(std::to_string(static_cast<int>(m_status)));
    m_buffer->append(" ");
    m_buffer->append(toString(m_status));
    m_buffer->append("\r\n");

    setHeader("Content-Length", std::to_string(msg.size()));
    for (auto &header : m_headers) {
        m_buffer->append(header.first);
        m_buffer->append(": ");
        m_buffer->append(header.second);
        m_buffer->append("\r\n");
    }
    m_buffer->append("\r\n");
    
    m_buffer->append(msg); 
}

void HttpResponse::sendJson(const Json &json) {
    std::string msg = std::move(json.dump());
    send(msg);
}

void HttpResponse::clear() {
    m_version = HttpVersion::HTTP_1_1;
    m_status = HttpStatus::OK;
    m_headers.clear();
    m_body.clear();
}
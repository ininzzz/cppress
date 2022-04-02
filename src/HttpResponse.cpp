#include"HttpResponse.h"

const std::string &HttpResponse::getHeader(const std::string &header) {
    auto it = m_headers.find(header);
    if (it == m_headers.end()) throw std::runtime_error("unknown header");
    return it->second;
}

void HttpResponse::send(const std::string &msg) {
    makeHeader(msg.size());
    m_buffer->append(msg); 
}

void HttpResponse::sendFile(const std::string &path) {
    int fd = ::open(path.c_str(), O_RDONLY);
    sendFile(fd);
}

void HttpResponse::sendFile(int fd) {
    struct stat st;
    fstat(fd, &st);
    int len = st.st_size;
    makeHeader(len);
    m_buffer->readFrom(fd);
}

void HttpResponse::sendJson(const Json &json) {
    std::string msg = std::move(json.dump());
    send(msg);
}

void HttpResponse::sendStatus(HttpStatus status) {
    setStatus(status);
    send(std::to_string(static_cast<int>(m_status)) + " " + toString(status));
}


void HttpResponse::clear() {
    m_version = HttpVersion::HTTP_1_1;
    m_status = HttpStatus::OK;
    m_headers.clear();
    m_body.clear();
}

void HttpResponse::makeHeader(int content_length) {
    m_buffer->append(toString(m_version));
    m_buffer->append(" ");
    m_buffer->append(std::to_string(static_cast<int>(m_status)));
    m_buffer->append(" ");
    m_buffer->append(toString(m_status));
    m_buffer->append("\r\n");

    setHeader("Content-Length", std::to_string(content_length));
    for (auto &header : m_headers) {
        m_buffer->append(header.first);
        m_buffer->append(": ");
        m_buffer->append(header.second);
        m_buffer->append("\r\n");
    }
    m_buffer->append("\r\n");
}
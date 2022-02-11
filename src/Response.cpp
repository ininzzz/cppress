#include"Response.h"

void Response::Send(const std::string &msg) {
    // line
    buf.Append("HTTP/1.1 " + std::to_string(static_cast<int>(status)) + " " + StatusToString(status) + "\r\n");
    // header
    buf.Append("Content-Type: " + ContentTypeToString(type) + "\r\n");
    buf.Append("Content-Length: " + std::to_string(msg.size() + 2) + "\r\n");
    buf.Append("\r\n");
    // body
    buf.Append(msg + "\r\n");
    need_write = true;
}

void Response::SetContentType(HTTP_CONTENT_TYPE type_) {
    type = type_;
}

void Response::SetStatus(HTTP_STATUS status_) {
    status = status_;
}

void Response::clear() {
    status = HTTP_STATUS::UNKNOWN;
    type = HTTP_CONTENT_TYPE::UNKOWN;
    need_write = false;
}

void Response::send() {
    int ret = buf.WriteTo(sockfd);
    if (ret == -1) throw std::runtime_error("buf send error");
    if (!buf.Readable()) need_write = false;
    return;
}
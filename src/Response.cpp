#include"Response.h"

void Response::Send(const std::string &msg) {
    // line
    buf.Append("HTTP/1.1 " + std::to_string(status) + " " + StatusToString(status) + "\r\n");
    // header
    buf.Append("Content-Type: " + ContentTypeToString(type) + "\r\n");
    buf.Append("Content-Length: " + std::to_string(msg.size() + 2) + "\r\n");
    buf.Append("\r\n");
    // body
    buf.Append(msg + "\r\n");
}

void Response::SetContentType(HTTP_CONTENT_TYPE type_) {
    type = type_;
}

void Response::SetStatus(HTTP_STATUS status_) {
    status = status_;
}

void Response::Init() {
    status = HTTP_STATUS::OK;
    type = HTTP_CONTENT_TYPE::TEXT_HTML;
}
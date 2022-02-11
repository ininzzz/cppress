#include"HttpParser.h"

HttpParser::HttpParser() :start(0), check(0), status(HTTP_CODE::LINE) {}

void HttpParser::Append(const std::string &str) {
    buffer.append(str);
}

HTTP_CODE HttpParser::Parse() {
    LINE_CODE ret;
    while (((ret = CheckLine()) == LINE_CODE::LINE_OK) || (status == HTTP_CODE::DONE || status == HTTP_CODE::ERROR)) {
        switch (status) {
        case HTTP_CODE::LINE: {
            ParseLine();
            break;
        }
        case HTTP_CODE::HEADER: {
            ParseHeader();
            break;
        }
        case HTTP_CODE::BODY: {
            ParseBody();
            break;
        }
        case HTTP_CODE::DONE: {
            status = HTTP_CODE::LINE;
            if (check == buffer.size()) {
                start = check = 0;
                buffer.clear();
                return HTTP_CODE::DONE;
            }
            start = check = 0;
            buffer.assign(buffer.begin() + check, buffer.end());
            return HTTP_CODE::PARTLY_DONE;
            break;
        }
        case HTTP_CODE::ERROR: {
            start = check = 0;
            buffer.clear();
            status = HTTP_CODE::LINE;
            return HTTP_CODE::ERROR;
            break;
        }
        default: {
            throw std::runtime_error("unexpected status\n");
            break;
        }
        }
        start = ++check;
    }
    if (ret == LINE_CODE::LINE_OPEN) return HTTP_CODE::OPEN;
    else return HTTP_CODE::ERROR;
}

void HttpParser::Clear() {
    _method = HTTP_METHOD::UNKNOWN;
    _url.clear();
    _version = HTTP_VERSION::UNKNOWN;

    _host.clear();
    _connection = HTTP_CONNECTION::UNKNOWN;
    _content_length = 0;

    _body.clear();
}


LINE_CODE HttpParser::CheckLine() {
    for (int &i = check;i < buffer.size();i++) {
        if (buffer[i] == '\n') {
            if (i > 0 && buffer[i - 1] == '\r') {
                return LINE_CODE::LINE_OK;
            }
            return LINE_CODE::LINE_BAD;
        }
        if (buffer[i] == '\r') {
            if (i + 1 < buffer.size() && buffer[i + 1] != '\n') {
                return LINE_CODE::LINE_BAD;
            }
        }
    }
    return LINE_CODE::LINE_OPEN;
}

void HttpParser::ParseLine() {
    Clear();
    if (strncasecmp(buffer.c_str() + start, "GET", 3) == 0) {
        _method = HTTP_METHOD::GET;
        start += 3;
        while (isblank(buffer[start])) start++;
    }
    else if (strncasecmp(buffer.c_str() + start, "POST", 4) == 0) {
        _method = HTTP_METHOD::POST;
        start += 4;
        while (isblank(buffer[start])) start++;
    }

    while (!isblank(buffer[start]) && buffer[start] != '?') {
        _url.push_back(buffer[start++]);
    }
    while (!isblank(buffer[start])) {
        _body.push_back(buffer[start++]);
    }
    while (isblank(buffer[start])) start++;

    if (strncasecmp(buffer.c_str() + start, "HTTP/1.1", 8) == 0) {
        _version = HTTP_VERSION::HTTP1_1;
        start += 8;
    }

    status = HTTP_CODE::HEADER;
}

void HttpParser::ParseHeader() {
    if (buffer[start] == '\r' && buffer[start + 1] == '\n') {
        // body为空的话可以把解析结果送出去了
        if (_method == HTTP_METHOD::GET || _content_length == 0) {
            status = HTTP_CODE::DONE;
            return;
        }
        else {
            status = HTTP_CODE::BODY;
            return;
        }
    }

    if (strncasecmp(buffer.c_str() + start, "Connection:", 11) == 0) {
        start += 11;
        while (isblank(buffer[start])) start++;
        if (strncasecmp(buffer.c_str() + start, "Keep-Alive", 10) == 0) {
            start += 10;
            _connection = HTTP_CONNECTION::KEEP_ALIVE;
        }
        else if (strncasecmp(buffer.c_str() + start, "Close", 5) == 0) {
            start += 5;
            _connection = HTTP_CONNECTION::CLOSE;
        }
    }
    else if (strncasecmp(buffer.c_str() + start, "Host:", 5) == 0) {
        start += 5;
        while (isblank(buffer[start])) start++;
        while (!isspace(buffer[start])) _host.push_back(buffer[start++]);
    }
    if (strncasecmp(buffer.c_str() + start, "Content-Length:", 15) == 0) {
        start += 15;
        while (isblank(buffer[start])) start++;
        std::string len;
        while (!isspace(buffer[start])) len.push_back(buffer[start++]);
        _content_length = ::atoi(len.c_str());
    }

    status = HTTP_CODE::HEADER;
}

void HttpParser::ParseBody() {
    // 全部内容都到了
    if (start + _content_length <= buffer.size()) {
        _body.append(buffer.begin() + start, buffer.begin() + start + _content_length);
        status = HTTP_CODE::DONE;
        check = start + _content_length - 1;
    }
    // 只有一部分到了
    else {
        _body.append(buffer.begin() + start, buffer.begin() + check + 1);
        status = HTTP_CODE::BODY;
    }

}
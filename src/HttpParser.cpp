#include"HttpParser.h"

HttpParser::HttpParser() :start(0), check(0), status(HTTP_CODE::LINE) {}

void HttpParser::Init() {
    _url.clear();
    _host.clear();
    _body.clear();
    _content_length = 0;
}

void HttpParser::Append(const std::string &str) {
    req.append(str);
    // printf("%s", str.c_str());
}

HTTP_CODE HttpParser::Parse() {
    printf("parsing...\n");
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
            printf("parse done\n");
            if (check == req.size()) req.clear();
            else req.assign(req.begin() + check, req.end());
            start = check = 0;
            status = HTTP_CODE::LINE;
            return HTTP_CODE::DONE;
            break;
        }
        case HTTP_CODE::ERROR: {
            start = check = 0;
            req.clear();
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


LINE_CODE HttpParser::CheckLine() {
    for (int &i = check;i < req.size();i++) {
        if (req[i] == '\n') {
            if (i > 0 && req[i - 1] == '\r') {
                return LINE_CODE::LINE_OK;
            }
            return LINE_CODE::LINE_BAD;
        }
        if (req[i] == '\r') {
            if (i + 1 < req.size() && req[i + 1] != '\n') {
                return LINE_CODE::LINE_BAD;
            }
        }
    }
    return LINE_CODE::LINE_OPEN;
}

void HttpParser::ParseLine() {
    Init();
    if (strncasecmp(req.c_str() + start, "GET", 3) == 0) {
        _method = HTTP_METHOD::GET;
        start += 3;
        while (isblank(req[start])) start++;
    }
    else if (strncasecmp(req.c_str() + start, "POST", 4) == 0) {
        _method = HTTP_METHOD::POST;
        start += 4;
        while (isblank(req[start])) start++;
    }

    while (!isblank(req[start]) && req[start] != '?') {
        _url.push_back(req[start++]);
    }
    while (!isblank(req[start])) {
        _body.push_back(req[start++]);
    }
    while (isblank(req[start])) start++;

    if (strncasecmp(req.c_str() + start, "HTTP/1.1", 8) == 0) {
        _version = HTTP_VERSION::HTTP1_1;
        start += 8;
    }

    status = HTTP_CODE::HEADER;
}

void HttpParser::ParseHeader() {
    if (req[start] == '\r' && req[start + 1] == '\n') {
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

    if (strncasecmp(req.c_str() + start, "Connection:", 11) == 0) {
        start += 11;
        while (isblank(req[start])) start++;
        if (strncasecmp(req.c_str() + start, "Keep-Alive", 10) == 0) {
            start += 10;
            _connection = HTTP_CONNECTION::KEEP_ALIVE;
        }
        else if (strncasecmp(req.c_str() + start, "Close", 5) == 0) {
            start += 5;
            _connection = HTTP_CONNECTION::CLOSE;
        }
    }
    else if (strncasecmp(req.c_str() + start, "Host:", 5) == 0) {
        start += 5;
        while (isblank(req[start])) start++;
        while (!isspace(req[start])) _host.push_back(req[start++]);
    }


    status = HTTP_CODE::HEADER;
}

void HttpParser::ParseBody() {
    // 全部内容都到了
    if (start + _content_length <= check + 1) {
        _body.append(req.begin() + start, req.begin() + start + _content_length);
        status = HTTP_CODE::DONE;
    }
    // 只有一部分到了
    else {
        _body.append(req.begin() + start, req.begin() + check + 1);
        status = HTTP_CODE::BODY;
    }

}
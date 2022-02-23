#include"HttpRequest.h"

const std::string HttpRequest::getHeader(const std::string &header) const {
    auto it = m_headers.find(header);
    if (it == m_headers.end()) return "";
    else return it->second;
}
const std::string HttpRequest::getParam(const std::string &header) const {
    auto it = m_params.find(header);
    if (it == m_params.end()) return "";
    else return it->second;
}

void HttpRequest::clear() {
    m_method = HttpMethod::UNKNOWN;
    
    m_url.clear();
    m_query.clear();
    
    m_version = HttpVersion::UNKNOWN;
    
    m_headers.clear();
    m_params.clear();
    m_content_length = 0;
    m_keep_alive = true;
    
    m_body.clear();
    
    m_line_status = LineStatus::NORMAL;
    m_parse_status = ParseStatus::LINE;
}

ParseCode HttpRequest::parse() {
    clear();
    m_buffer->readFrom(m_fd);
    LineCode ret;
    while ((m_parse_status == ParseStatus::BODY) || ((ret = checkLine()) == LineCode::OK)) {
        switch (m_parse_status) {
        case ParseStatus::LINE: {
            ParseCode ret = parseLine();
            if (ret == ParseCode::OK) {
                m_parse_status = ParseStatus::HEADER;
            } else if (ret == ParseCode::BAD) {
                return ParseCode::BAD;
            }
            break;
        }
        case ParseStatus::HEADER: {
            ParseCode ret = parseHeader();
            if (ret == ParseCode::OK) {
                return ParseCode::OK;
            } else if (ret == ParseCode::BAD) {
                return ParseCode::BAD;
            }
            break;
        }
        case ParseStatus::BODY: {
            ParseCode ret = parseBody();
            if (ret == ParseCode::OK) {
                return ParseCode::OK;
            }
            break;
        }
        default: {
            throw std::runtime_error("something wrong");
        }
        }
    }
    if (ret == LineCode::OPEN) return ParseCode::OPEN;
    else return ParseCode::BAD;
}

LineCode HttpRequest::checkLine() {
    m_line.clear();
    while (!m_buffer->empty()) {
        char c = m_buffer->front();
        m_line.push_back(c);
        m_buffer->pop();
        switch (m_line_status) {
        case LineStatus::NORMAL: {
            if (c == '\r') m_line_status = LineStatus::RR;
            else if (c == '\n') {
                m_line_status = LineStatus::NORMAL;
                return LineCode::BAD;
            } 
            break;
        }
        case LineStatus::RR: {
            m_line_status = LineStatus::NORMAL;
            if (c == '\n') return LineCode::OK;
            else return LineCode::BAD;
            break;
        }
        default: {
            throw std::runtime_error("something wrong");
        }
        }
    }
    return LineCode::OPEN;
}

ParseCode HttpRequest::parseLine() {
    int start = 0;
    if (strncasecmp(m_line.c_str() + start, "GET", 3) == 0) {
        m_method = HttpMethod::GET;
        start += 3;
        while (isblank(m_line[start])) start++;
    } else if (strncasecmp(m_line.c_str() + start, "POST", 4) == 0) {
        m_method = HttpMethod::POST;
        start += 4;
        while (isblank(m_line[start])) start++;
    }

    while (!isblank(m_line[start]) && m_line[start] != '?') {
        m_url.push_back(m_line[start++]);
    }
    while (!isblank(m_line[start])) {
        m_query.push_back(m_line[start++]);
    }
    while (isblank(m_line[start])) start++;

    if (strncasecmp(m_line.c_str() + start, "HTTP/1.1", 8) == 0) {
        m_version = HttpVersion::HTTP_1_1;
        start += 8;
    }
    return ParseCode::OK;
}

ParseCode HttpRequest::parseHeader() {
    if (m_line[0] == '\r' && m_line[1] == '\n') {
        if (m_method == HttpMethod::GET || m_content_length == 0) {
            return ParseCode::OK;
        }
        m_parse_status = ParseStatus::BODY;
        return ParseCode::OPEN;
    }
    std::string key, val;
    int start = 0;
    while (!isspace(m_line[start]) && (m_line[start] != ':')) {
        key.push_back(m_line[start++]);
    }
    while (isspace(m_line[start]) || m_line[start] == ':') {
        start++;
    }
    val.append(m_line.begin() + start, m_line.end() - 2);
    m_headers[key] = val;
    if (strcasecmp(key.c_str(), "Content-Length") == 0) m_content_length = ::atoi(val.c_str());
    if (strcasecmp(key.c_str(), "Connection") == 0) m_keep_alive = ((::toupper(val[0]) == 'K') ? true : false);
    return ParseCode::OPEN;
}

ParseCode HttpRequest::parseBody() {
    while ((m_body.size() < m_content_length) && (!m_buffer->empty())) {
        m_body.push_back(m_buffer->front());
        m_buffer->pop();
    }
    if (m_body.size() == m_content_length) return ParseCode::OK;
    else return ParseCode::OPEN;
}

void HttpRequest::json() {
    if (m_headers["Content-Type"] == "application/json") {
        m_json.reset(new Json(JsonParser(m_body).parse()));
    }
}
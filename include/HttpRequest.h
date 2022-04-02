#pragma once

#include<memory>
#include<map>
#include<cstring>
#include<algorithm>
#include<string>

#include"Http.h"
#include"Buffer.h"
#include"Json.h"

enum class LineCode {
    OK, BAD, OPEN
};
enum class LineStatus {
    NN, RR, NORMAL
};
enum class ParseStatus {
    LINE, HEADER, BODY
};
enum class ParseCode {
    OK, BAD, OPEN
};

class HttpRequest {
public:
    friend class TCPConnection;
    using ptr = std::shared_ptr<HttpRequest>;
    
    HttpRequest(int fd)
        : m_buffer(new Buffer), m_fd(fd) {
    };
    
    HttpMethod method() const { return m_method; }
    const std::string& path() const { return m_path; }
    const std::string& query() const { return m_query; }
    HttpVersion version() const { return m_version; }
    const std::string &body() const { return m_body; }

    // void json();

    const std::string &getHeader(const std::string &header) const;
    const std::string &getQuery(const std::string &header) const;
    const std::string &getParam(const std::string &header) const;
    
    void setAttribute(const std::string &key, const Json &value);
    const Json getAttribute(const std::string &header) const;
    // const Json &getJson() { return *m_json.get(); }
    bool isclose() { return !m_keep_alive; }

    ParseCode parse();
    void reset(int fd) { m_fd = fd; }
    void clear();
private:
    LineCode checkLine();
    ParseCode parseLine();
    ParseCode parseHeader();
    ParseCode parseBody();
private:
    int m_fd;
    HttpMethod m_method = HttpMethod::UNKNOWN;

    std::string m_path;
    std::string m_query;

    HttpVersion m_version = HttpVersion::UNKNOWN;

    std::map<std::string, std::string> m_headers;
    std::map<std::string, std::string> m_params;
    std::map<std::string, std::string> m_querys;
    
    std::map<std::string, Json> m_attribute;
    
    int m_content_length = 0;
    bool m_keep_alive = true;

    std::string m_body;

    Buffer::ptr m_buffer;
    LineStatus m_line_status = LineStatus::NORMAL;
    ParseStatus m_parse_status = ParseStatus::LINE;
    std::string m_line;

    // JsonParser::ptr m_json_parser;
    // Json::ptr m_json;
};

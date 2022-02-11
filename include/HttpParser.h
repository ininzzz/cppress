#ifndef HTTPPARSER_H
#define HTTPPARSER_H

#include<string>
#include<functional>
#include<cstring>

#include"HttpEnum.h"

enum LINE_CODE { LINE_OK, LINE_OPEN, LINE_BAD };
enum HTTP_CODE { LINE, HEADER, BODY, DONE, PARTLY_DONE, ERROR, OPEN };

class HttpParser {
public:
    HttpParser();
    
    void Append(const std::string &str);
    HTTP_CODE Parse();
    
    std::string url() { return _url; }
    std::string host() { return _host; }
    HTTP_METHOD method() { return _method; }
    HTTP_VERSION version() { return _version; }
    HTTP_CONNECTION connection() { return _connection; }
    std::string body() { return _body; }
private:
    LINE_CODE CheckLine();
    void ParseLine();
    void ParseHeader();
    void ParseBody();
    void Clear();
    
    std::string buffer;
    int start, check;
    HTTP_CODE status;

    // line
    HTTP_METHOD _method;
    std::string _url;
    HTTP_VERSION _version;
    // header
    std::string _host;
    HTTP_CONNECTION _connection;
    int _content_length;
    // body
    std::string _body;

};


#endif
#ifndef HTTPENUM_H
#define HTTPENUM_H

#include<string>

enum class HTTP_METHOD { GET, POST, UNKNOWN };
enum class HTTP_CONNECTION{ KEEP_ALIVE, CLOSE, UNKNOWN };
enum class HTTP_VERSION { HTTP1_1, UNKNOWN };

enum class HTTP_CONTENT_TYPE { TEXT_HTML, X_WWW_FROM_URLENCODED, JSON, UNKOWN };
enum class HTTP_STATUS { OK = 200, BAD_REQUEST = 400, NOT_FOUND = 404, UNKNOWN };

std::string ContentTypeToString(HTTP_CONTENT_TYPE type);

std::string StatusToString(HTTP_STATUS status);

#endif
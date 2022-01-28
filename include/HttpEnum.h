#ifndef HTTPENUM_H
#define HTTPENUM_H

#include<string>

enum HTTP_METHOD { GET, POST };
enum HTTP_CONNECTION { KEEP_ALIVE, CLOSE };
enum HTTP_VERSION { HTTP1_1 };

enum HTTP_CONTENT_TYPE { TEXT_HTML, X_WWW_FROM_URLENCODED, JSON };
enum HTTP_STATUS { OK = 200, BAD_REQUEST = 400, NOT_FOUND = 404 };

std::string ContentTypeToString(HTTP_CONTENT_TYPE type);

std::string StatusToString(HTTP_STATUS status);

#endif
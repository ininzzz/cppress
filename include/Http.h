#pragma once

#include<string>

enum class HttpStatus {
    OK = 200,
    NO_CONTENT = 204,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    SEE_OTHER = 303,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503,
    UNKNOWN,
};

enum class HttpMethod {
    GET,
    POST,
    UNKNOWN,
};

enum class HttpVersion {
    HTTP_1_1,
    UNKNOWN,
};

std::string toString(HttpStatus status);
std::string toString(HttpMethod method);
std::string toString(HttpVersion version);

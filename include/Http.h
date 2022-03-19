#pragma once

#include<string>

enum class HttpStatus {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
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

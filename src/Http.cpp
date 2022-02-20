#include"Http.h"

std::string toString(HttpStatus status) {
    switch (status) {
    case HttpStatus::OK:
        return "OK";
    case HttpStatus::BAD_REQUEST:
        return "Bad Request";
    case HttpStatus::NOT_FOUND:
        return "Not found";
    default:
        return "Unknown";
    }
}
std::string toString(HttpMethod method) {
    switch (method) {
    case HttpMethod::GET:
        return "GET";
    case HttpMethod::POST:
        return "POST";
    default:
        return "UNKNOWN";
    }
}
std::string toString(HttpVersion version) {
    switch (version) {
    case HttpVersion::HTTP_1_1:
        return "HTTP/1.1";
    default:
        return "UNKNOWN";
    }
}

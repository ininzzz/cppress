#include"Http.h"

std::string toString(HttpStatus status) {
    switch (status) {
    case HttpStatus::OK:                    return "OK";
    case HttpStatus::NO_CONTENT:            return "No Content";

    case HttpStatus::MOVED_PERMANENTLY:     return "Moved Permanently";
    case HttpStatus::FOUND:                 return "Found";
    case HttpStatus::SEE_OTHER:             return "See Other";

    case HttpStatus::BAD_REQUEST:           return "Bad Request";
    case HttpStatus::UNAUTHORIZED:          return "Unauthorized";
    case HttpStatus::NOT_FOUND:             return "Not found";
        
    case HttpStatus::INTERNAL_SERVER_ERROR: return "Internal Server Error";
    case HttpStatus::SERVICE_UNAVAILABLE:   return "Service Unavailable";
    default:                                return "Unknown";
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

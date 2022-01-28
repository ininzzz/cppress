#include"HttpEnum.h"

std::string ContentTypeToString(HTTP_CONTENT_TYPE type) {
    switch (type) {
    case HTTP_CONTENT_TYPE::TEXT_HTML: {
        return "text/html";
        break;
    }
    case HTTP_CONTENT_TYPE::X_WWW_FROM_URLENCODED: {
        return "application/x-www-form-urlencoded";
        break;
    }
    case HTTP_CONTENT_TYPE::JSON: {
        return "application/json";
        break;
    }
    default: {
        return "";
        break;
    }
    }
}

std::string StatusToString(HTTP_STATUS status) {
    switch (status) {
    case HTTP_STATUS::OK: {
        return "OK";
        break;
    }
    case HTTP_STATUS::BAD_REQUEST: {
        return "Bad Request";
        break;
    }
    case HTTP_STATUS::NOT_FOUND: {
        return "Not Found";
        break;
    }
    default: {
        return "";
        break;
    }
    }
}
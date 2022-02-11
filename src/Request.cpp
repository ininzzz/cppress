#include"Request.h"

Request::Request() {}

HTTP_CODE Request::get() {
    while (buf.ReadFrom(sockfd) != -1) {
        parser.Append(buf.GetBuf());
    }
    return parser.Parse();
}

void Request::clear() {
    buf.Clear();
    
    _port = 0;
    _ip.clear();
}


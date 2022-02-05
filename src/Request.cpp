#include"Request.h"

Request::Request() {}

HTTP_CODE Request::ReadFrom() {
    if (buf.ReadFrom(sockfd) == 0) return HTTP_CODE::OPEN;
    parser.Append(buf.GetBuf());
    return parser.Parse();
}
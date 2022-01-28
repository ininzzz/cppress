#include"Request.h"

Request::Request() {}

HTTP_CODE Request::ReadFrom() {
    buf.ReadFrom(sockfd);
    parser.Append(buf.GetBuf());
    return parser.Parse();
}
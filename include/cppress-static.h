#pragma once

#include"cppress.h"
#include"Json.h"

bool staticFile(HttpRequest::ptr req, HttpResponse::ptr res) {
    if (req->method() != HttpMethod::GET) return true;
    std::string path = req->path();
    bool ok = false;
    for (int i = path.size() - 1;i >= 0;i--) {
        if (path[i] == '.') {
            ok = true;
            break;
        }
    }
    if (!ok) {
        if (path.back() != '/') path.push_back('/');
        path += "index.html";
    }
    path.insert(path.begin(), '.');
    int fd = ::open(path.c_str(), O_RDONLY);
    if (fd == -1) return true;
    res->sendFile(fd);
    return false;
}
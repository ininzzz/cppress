#pragma once

#include"cppress.h"
#include"Json.h"

bool json(HttpRequest::ptr req, HttpResponse::ptr res) {
    if (req->method() == HttpMethod::GET) return true;
    if (req->getHeader("Content-Type") == "application/json") {
        req->setAttribute("json", JsonParser(req->body()).parse());
    }
    return true;
}
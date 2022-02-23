#ifndef ROUTER_H
#define ROUTER_H

#include"HttpRequest.h"
#include"HttpResponse.h"

class Router {
public:
    using ptr = std::shared_ptr<Router>;
    Router(HttpRequest::ptr req, HttpResponse::ptr res)
        :m_req(req), m_res(res) {
    }
    void set(const std::function<void(HttpRequest &, HttpResponse &)> &func);
    void process();
private:
    HttpRequest::ptr m_req;
    HttpResponse::ptr m_res;
    std::function<void(HttpRequest &, HttpResponse &)> m_func;
};


#endif
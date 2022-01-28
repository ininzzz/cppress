#ifndef ROUTER_H
#define ROUTER_H

#include"Request.h"
#include"Response.h"

class Router {
public:
    Router();
    void init(const Request &req_, const Response &res_);
    void set(const std::function<void(Request &, Response &)> &func_);
    void process();
private:
    Request req;
    Response res;
    std::function<void(Request &, Response &)> func;
};


#endif
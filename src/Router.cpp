#include"Router.h"

Router::Router() {}

void Router::init(const Request &req_, const Response &res_) {
    req = req_;
    res = res_;
}

void Router::set(const std::function<void(Request &, Response &)> &func_) {
    func = func_;
}

void Router::process() {
    func(req, res);
}
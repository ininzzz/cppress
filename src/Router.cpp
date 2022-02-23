#include"Router.h"


void Router::set(const std::function<void(HttpRequest &, HttpResponse &)> &func) {
    m_func = func;
}

void Router::process() {
    m_func(*m_req.get(), *m_res.get());
}
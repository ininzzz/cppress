#include"Router.h"

void Router::processMiddleware() {
    for (auto &cb : m_middleware) {
        cb(m_req, m_res);
    }
}
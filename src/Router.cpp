#include"Router.h"

bool Router::processMiddleware() {
    for (auto &cb : m_middleware) {
        if (!cb(m_req, m_res)) return false;
    }
    return true;
}
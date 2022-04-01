#include"Router.h"

void Router::get(const std::string &path, const callback &func) {
    std::string now = path;
    if (path.back() != '/') now.push_back('/');
    m_get[now] = func;
}

void Router::post(const std::string &path, const callback &func) {
    std::string now = path;
    if (path.back() != '/') now.push_back('/');
    m_post[now] = func;
}

bool Router::processMiddleware(HttpRequest::ptr req, HttpResponse::ptr res) const {
    for (auto &cb : m_middleware) {
        if (!cb(req, res)) return false;
    }
    return true;
}

const Router::callback Router::processGet(const std::string &path) const {
    auto itr = m_get.find(path);
    if (itr == m_get.end()) return nullptr;
    return itr->second;
}

const Router::callback Router::processPost(const std::string &path) const{
    auto itr = m_post.find(path);
    if (itr == m_post.end()) return nullptr;
    return itr->second;
}
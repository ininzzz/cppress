#pragma once

#include<unordered_map>

#include"HttpRequest.h"
#include"HttpResponse.h"

class Router {
public:
    using ptr = std::shared_ptr<Router>;
    using callback = std::function<void(HttpRequest::ptr, HttpResponse::ptr)>;
    using middleware = std::function<bool(HttpRequest::ptr, HttpResponse::ptr)>;

    Router(const std::string path = "") { m_path = path; }
    
    void set(const std::string &path) { m_path = path; }

    const std::string &path() const { return m_path; }

    void get(const std::string &path, const callback &func);
    void post(const std::string &path, const callback &func);

    void use(const middleware &func) { m_middleware.push_back(func); }
    
    const callback processGet(const std::string &path) const;
    const callback processPost(const std::string &path) const;
    bool processMiddleware(HttpRequest::ptr req, HttpResponse::ptr res) const;

    bool validGet(const std::string &path) { return m_get.count(path); }
    bool validPost(const std::string &path) { return m_post.count(path); }
private:
    std::string m_path;
    std::unordered_map<std::string, callback> m_get;
    std::unordered_map<std::string, callback> m_post;
    std::list<middleware> m_middleware;
};
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
    void set(HttpRequest::ptr req, HttpResponse::ptr res) { m_req = req, m_res = res; }
    const std::string &path() { return m_path; }
    
    void get(const std::string &path, const callback &func) { m_get[path] = func; }
    void post(const std::string &path, const callback &func) { m_post[path] = func; }

    void use(const middleware &func) { m_middleware.push_back(func); }
    
    const callback &processGet(const std::string &path) { return m_get[path]; }
    const callback &processPost(const std::string &path) { return m_post[path]; }
    bool processMiddleware();

    bool validGet(const std::string &path) { return m_get.count(path); }
    bool validPost(const std::string &path) { return m_post.count(path); }
private:
    std::string m_path;
    std::unordered_map<std::string, callback> m_get;
    std::unordered_map<std::string, callback> m_post;
    std::list<middleware> m_middleware;
    HttpRequest::ptr m_req;
    HttpResponse::ptr m_res;
};
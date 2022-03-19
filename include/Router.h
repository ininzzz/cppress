#pragma once

#include<unordered_map>

#include"HttpRequest.h"
#include"HttpResponse.h"

class Router {
public:
    using ptr = std::shared_ptr<Router>;
    using callback = std::function<void(HttpRequest::ptr, HttpResponse::ptr)>;
    Router(const std::string url = "") { m_url = url; }
    
    void set(const std::string &url) { m_url = url; }
    const std::string &url() { return m_url; }
    
    void get(const std::string &url, const callback &func) { m_get[url] = func; }
    void post(const std::string &url, const callback &func) { m_post[url] = func; }
    
    const callback &processGet(const std::string &url) { return m_get[url]; }
    const callback &processPost(const std::string &url) { return m_post[url]; }
    
    bool validGet(const std::string &url) { return m_get.count(url); }
    bool validPost(const std::string &url) { return m_post.count(url); }
private:
    std::string m_url;
    std::unordered_map<std::string, callback> m_get;
    std::unordered_map<std::string, callback> m_post;
    HttpRequest::ptr m_req;
    HttpResponse::ptr m_res;
};
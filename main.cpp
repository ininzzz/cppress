#include<iostream>
#include"cppress.h"

int main() {
    LOG_STDOUT_FORMAT("%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");
    
    WebServer::ptr server(new WebServer);
    Router::ptr router(new Router);
    router->get("/info", [](HttpRequest::ptr req, HttpResponse::ptr res) {
        res->send("user info");
    });
    server->use("/user", router);
    
    server->use([](HttpRequest::ptr req, HttpResponse::ptr res) {
        req->json();
    });
    
    server->get("/", [](HttpRequest::ptr req, HttpResponse::ptr res) {
        std::cout << toString(req->method()) << std::endl;
        std::cout << req->path() << std::endl;
        std::cout << toString(req->version()) << std::endl;
        std::cout << req->getHeader("Connection") << std::endl;
        res->send("ok");
    });
    server->post("/", [](HttpRequest::ptr req, HttpResponse::ptr res) {
        std::cout << toString(req->method()) << std::endl;
        std::cout << req->path() << std::endl;
        std::cout << toString(req->version()) << std::endl;
        std::cout << req->getHeader("Connection") << std::endl;
        std::cout << req->getJson().dump() << std::endl;
        res->sendJson(Json::object{
            {"name","Jack"},
            {"age",20},
        });
    });
    
    LOG_DEBUG("%s", "server is running...")
    server->listen(12345);
    return 0;
}
#include<iostream>
#include"Log.h"
#include"cppress.h"
#include"cppress-json.h"

int main() {
    LOG_STDOUT_FORMAT("%c\t%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");
    LOG_FILE_FORMAT("../log.txt","%c\t%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");


    WebServer::ptr server(new WebServer);

    // add router
    Router::ptr router(new Router);
    router->post("/info", [](HttpRequest::ptr req, HttpResponse::ptr res) {
        res->send("hello");
    });
    server->use("/user", router);
    server->use("/user",[](HttpRequest::ptr req, HttpResponse::ptr res) {
        if (req->getAttribute("json")["usr"] == "admin" && req->getAttribute("json")["pwd"] == "123456") {
            return true;
        }
        res->send("permisson denied");
        return false;
    });

    // use global middleware
    server->use(json);
    
    server->get("/", [](HttpRequest::ptr req, HttpResponse::ptr res) {
        std::cout << toString(req->method()) << std::endl;
        std::cout << req->path() << std::endl;
        std::cout << toString(req->version()) << std::endl;
        std::cout << req->getHeader("Connection") << std::endl;
        res->setHeader("Content-Type", "image/png");
        res->sendFile("../xxx.png");
        LOG_DEBUG("%s", "new connection!");
        res->send("OK");
    });
    server->post("/", [](HttpRequest::ptr req, HttpResponse::ptr res) {
        std::cout << toString(req->method()) << std::endl;
        std::cout << req->path() << std::endl;
        std::cout << toString(req->version()) << std::endl;
        std::cout << req->getHeader("Connection") << std::endl;
        std::cout << req->getAttribute("json").dump() << std::endl;
        res->sendJson(Json::object{
            {"name","Jack"},
            {"age",20},
        });
    });
    
    LOG_DEBUG("%s", "server is running...")
    server->listen(12345);
    return 0;
}
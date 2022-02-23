#include<iostream>
#include"WebServer.h"

int main() {
    WebServer::ptr server(new WebServer);
    server->get("/", [](HttpRequest &req, HttpResponse &res) {
        std::cout << toString(req.method()) << std::endl;
        std::cout << req.url() << std::endl;
        std::cout << toString(req.version()) << std::endl;
        std::cout << req.getHeader("Connection") << std::endl;
        res.send("hi");
    });
    server->post("/", [](HttpRequest &req, HttpResponse &res) {
        req.json();
        std::cout << toString(req.method()) << std::endl;
        std::cout << req.url() << std::endl;
        std::cout << toString(req.version()) << std::endl;
        std::cout << req.getHeader("Connection") << std::endl;
        std::cout << req.getJson().dump() << std::endl;
        res.sendJson(Json::object{
            {"name","Jack"},
            {"age",20},
        });
    });
    server->listen(12345);

    return 0;
}
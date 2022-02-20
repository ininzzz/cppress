#include<iostream>
#include"WebServer.h"

int main() {
    WebServer::ptr server(new WebServer);
    server->get("/", [](HttpRequest &req, HttpResponse &res) {
        std::cout << toString(req.method()) << std::endl;
        std::cout << req.path() << std::endl;
        std::cout << toString(req.version()) << std::endl;
        std::cout << req.getHeader("Connection") << std::endl;
        res.send("fuck you");
    });
    server->post("/", [](HttpRequest &req, HttpResponse &res) {
        std::cout << toString(req.method()) << std::endl;
        std::cout << req.path() << std::endl;
        std::cout << toString(req.version()) << std::endl;
        std::cout << req.getHeader("Connection") << std::endl;
        res.send("fuck you");
    });
    server->listen(12345);

    return 0;
}
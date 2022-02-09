#include<iostream>

#include"WebServer.h"

int main() {
    WebServer server;
    server.Get("/", [](Request &req, Response &res) {
        // std::cout << req.ip() << " " << req.port() << std::endl;
        // std::cout << req.method() << std::endl;
        // std::cout << req.url() << std::endl;
        // std::cout << req.version() << std::endl;
        // std::cout << req.host() << std::endl;

        res.Send("fuck you");
    });
    server.Post("/", [](Request &req, Response &res) {
        std::cout << req.ip() << " " << req.port() << std::endl;
        std::cout << req.method() << std::endl;
        std::cout << req.url() << std::endl;
        std::cout << req.version() << std::endl;
        std::cout << req.host() << std::endl;

        res.Send("fuck you");
    });
    server.Listen(12345);
    return 0;
}
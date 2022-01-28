# CPPRESS

- 用C++实现的Web网络框架，仿照node.js的express框架

- IO复用epoll+非阻塞socket+线程池的Reactor并发模型

- EPOLLONESHOT确保每个socket只被一个线程处理

- 有限状态机解析Http请求报文

- 自动增长的缓冲区确保数据的正常收发

- TODO：日志系统，定时器，Router路由，json解析，若干基础组件，若干bug和异常处理

- 压力测试等bug改差不多了再做

- 目前只是个半成品，还在慢慢完善/(ㄒoㄒ)/~~

  ```cpp
  #include<iostream>
  
  #include"WebServer.h"
  
  int main() {
      WebServer server;
      server.Get("/", [](Request &req, Response &res) {
          std::cout << req.ip() << " " << req.port() << std::endl;
          std::cout << req.method() << std::endl;
          std::cout << req.url() << std::endl;
          std::cout << req.version() << std::endl;
          std::cout << req.host() << std::endl;
  
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
  ```

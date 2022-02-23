# CPPRESS

- 用C++实现的Web网络框架，仿照node.js的express框架

- IO复用epoll+非阻塞socket+线程池的Reactor并发模型

- 有限状态机解析Http请求报文

- 参考内存分页机制实现的链式缓冲区

- 优先队列(最小堆)实现定时器，定时检查并断开不活跃的连接

- 类Log4j风格的同步日志

- 支持JSON解析和序列化

- TODO：代码优化和异常处理

  ```cpp
  #include<iostream>
  
  #include"WebServer.h"
  
  int main() {
      WebServer::ptr server(new WebServer);
      server->get("/", [](HttpRequest &req, HttpResponse &res) {
          std::cout << toString(req.method()) << std::endl;
          std::cout << req.url() << std::endl;
          std::cout << toString(req.version()) << std::endl;
          std::cout << req.getHeader("Connection") << std::endl;
          res.send("hello");
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
  ```
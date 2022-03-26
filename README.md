# CPPRESS

- 用C++实现的Web网络框架，仿照node.js的express框架

- IO复用epoll+非阻塞socket+线程池的Reactor并发模型

- 有限状态机解析Http请求报文

- 参考内存分页机制实现的链式缓冲区

- 优先队列(最小堆)实现定时器，定时检查并断开不活跃的连接

- 基于C++11局部静态变量的线程安全性实现单例模式

- 类Log4j风格的同步日志

- 支持JSON序列化和反序列化

- 支持中间件(应用级中间件，路由级中间件)

- TODO：代码优化和异常处理，中间件优化

  ```cpp
  int main() {
      LOG_STDOUT_FORMAT("%c\t%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");
      LOG_FILE_FORMAT("../log.txt","%c\t%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");
  
  
      WebServer::ptr server(new WebServer);
      Router::ptr router(new Router);
      router->use([](HttpRequest::ptr req, HttpResponse::ptr res) {
          printf("middleware1...\n");
      });
      router->use([](HttpRequest::ptr req, HttpResponse::ptr res) {
          printf("middleware2...\n");
      });
      router->get("/info", [](HttpRequest::ptr req, HttpResponse::ptr   res) {
          res->send("user info");
      });
      server->use("/user", router);
  
      server->use([](HttpRequest::ptr req, HttpResponse::ptr res) {
          printf("global...\n");
      });
  
      server->use([](HttpRequest::ptr req, HttpResponse::ptr res) {
          req->json();
      });
      
      server->get("/", [](HttpRequest::ptr req, HttpResponse::ptr res) {
          std::cout << toString(req->method()) << std::endl;
          std::cout << req->path() << std::endl;
          std::cout << toString(req->version()) << std::endl;
          std::cout << req->getHeader("Connection") << std::endl;
          res->setHeader("Content-Type", "image/png");
          res->sendFile("../xxx.png");
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
  ```
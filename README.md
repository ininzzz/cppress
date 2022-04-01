# CPPRESS

## 概述

- 用C++实现的Web网络框架，利用若干C++11特性
- 风格仿照node.js的express框架

## 基本用法

- 注册某路径下的GET/POST回调函数

- ```cpp
  server->get("/", [](HttpRequest::ptr req, HttpResponse::ptr res) {
      std::cout << toString(req->method()) << std::endl;
      std::cout << req->path() << std::endl;
      std::cout << toString(req->version()) << std::endl;
      std::cout << req->getHeader("Connection") << std::endl;
      res->setHeader("Content-Type", "image/png");
      res->sendFile("./xxx.png");
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
  ```

- 格式化风格打印日志信息

- ```cpp
  // %m -- 消息体
  // %p -- level
  // %c -- 日志名称
  // %t -- 线程id
  // %n -- 回车换行
  // %d -- 时间
  // %f -- 文件名
  // %l -- 行号
  
  // 输出到终端
  LOG_STDOUT_FORMAT("%c\t%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");
  // 输出到文件log.txt
  LOG_FILE_FORMAT("./log.txt","%c\t%p\t%d{%Y-%m-%d %H:%M:%S}\t%m%n");
  
  // 输出debug信息
  // root    DEBUG   2022-02-14 17:05:16     server is running...
  LOG_DEBUG("%s", "server is running...");
  ```

## 中间件

### 应用级中间件

- 注册中间件需要实现一个函数签名为`(HttpRequest::ptr, HttpResponse::ptr)->bool`的函数，然后用`WebServer::use()`或`Router::use()`注册即可，流入下个中间件返回`true`，否则返回`false`

- ```cpp
  // 注册json解析中间件
  #include"Json.h"
  
  bool json(HttpRequest::ptr req, HttpResponse::ptr res) {
      if (req->getHeader("Content-Type") == "application/json") {
          req->setAttribute("json", JsonParser(req->body()).parse());
      }
      return true;
  }
  
  int main(){
      // ...
      server->use(json);
      // ...
  }
  ```

### 路由级中间件

- 注册中间件的时候挂载路径，中间件就会注册到挂载路径对应的路由上，也可以在路由上直接注册

- ```cpp
  // 在挂载在/user目录下的路由上注册中间件
  server->use("/user",[](HttpRequest::ptr req, HttpResponse::ptr res) {
      if (req->getAttribute("json")["usr"] == "admin" && req->getAttribute("json")["pwd"] == "123456") {
          return true;
      }
      res->send("permisson denied");
      return false;
  });
  ```

## 实现细节

### 连接

- epoll监听连接，当出现新的连接后，扔给线程池处理(接受连接并进行一些初始化，并在EPOLL上注册读事件)，EPOLLONESHOT可以保证连接处理的线程安全性
- 当收到一个新的请求后，从socket缓冲区读到buffer中，然后利用HttpParser解析请求(有限状态机实现)
- 然后将req,res交给中间件和回调函数处理业务逻辑
- 如果有send的需求，则在epoll上注册写事件，在可写的时候发送到客户端即可

### 异步日志

- 利用基于生产者消费者模型的双缓冲区实现的异步日志，后台线程在缓冲区有数据的时候被唤醒，然后写入磁盘，此时交换两个缓冲区，日志写到另一个缓冲区，避免阻塞，提高并发度
- 缓冲区采用分页式的链表将大小为4KB的若干页连接起来，兼顾空间和性能

- 为了提高性能，在缓冲区大于某个阈值的时候再持久化到磁盘

- ```cpp
  LogBuffer::LogBuffer(int fd) :
  	// ...
      m_producer([this] {
      while (true) {
          std::unique_lock<std::mutex> lock(m_mtx);
          m_cond.wait(lock, [this] {return !empty() && size() > LOG_THRESHOLD;});
          m_buffer.swap(m_buffer_temp);
          lock.unlock();
          dump();
      }
  }) {}
  
  void LogBuffer::append(const std::string &str) {
      if (str.size() == 0) return;
      std::unique_lock<std::mutex> lock(m_mtx);
      // ...
      m_cond.notify_one();
  }
  ```

- 利用C++局部变量的线程安全性实现单例模式

- ```cpp
  template <class T>
  class Singleton {
  public:
      using handleType = std::shared_ptr<T>;
  
      Singleton(const Singleton &inst) = delete;
      Singleton &operator=(const Singleton &inst) = delete;
      
      static handleType &GetInstance() {
          static handleType m_inst(new T);
          return m_inst;
      }
  
  private:
      static handleType m_inst;
  };
  ```

### 连接缓冲区

- 参考muduo网络库，为每一个read和write设置缓冲区，保证数据正常收发(如果不设置缓冲区有可能导致socket缓冲区已满而无法发送数据)
- 缓冲区依然采用分页式，但是不需要确保线程安全性，因为每个TCP connection独享两个读/写缓冲区，从而提高性能

### 定时器

- 优先队列(最小堆)实现的定时器，考虑到优先队列的不可随机访问性，用一个桶记录和更新每个连接的超时时间，保证O(1)的更新，同时每次handle的时候根据桶中的超时时间踢掉对应的连接，空出资源

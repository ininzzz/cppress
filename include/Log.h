#pragma once

#include<string>
#include<memory>
#include<list>
#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include<iomanip>
#include<chrono>
#include<map>
#include<functional>
#include<thread>
#include<stdint.h>
#include<stdarg.h>
#include<fcntl.h>

#include"Singleton.h"
#include"LogBuffer.h"


// %m -- 消息体
// %p -- level
// %c -- 日志名称
// %t -- 线程id
// %n -- 回车换行
// %d -- 时间
// %f -- 文件名
// %l -- 行号

#define LOG_STDOUT_FORMAT(formatter)\
    {LogAppender::ptr ptr(new LogAppender_stdout);\
    ptr->setFormatter(formatter);\
    Singleton<Logger>::GetInstance()->addAppender(ptr);}\

#define LOG_FILE_FORMAT(filepath, formatter)\
    {LogAppender::ptr ptr(new LogAppender_file(filepath));\
    ptr->setFormatter(formatter);\
    Singleton<Logger>::GetInstance()->addAppender(ptr);}\

#define LOG_LEVEL(level, fmt, ...)\
    if(Singleton<Logger>::GetInstance()->getLevel() <= LogLevel::level)\
        Singleton<Logger>::GetInstance()->log(LogLevel::level, LogEvent::ptr(new LogEvent(\
            LogLevel::level,\
            __FILE__,\
            __LINE__,\
            std::this_thread::get_id(),\
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),\
            fmt,\
            __VA_ARGS__\
)));

#define LOG_DEBUG(fmt, ...) LOG_LEVEL(DEBUG, fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...)  LOG_LEVEL(INFO,  fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_LEVEL(WARN,  fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_LEVEL(ERROR, fmt, __VA_ARGS__)
#define LOG_FATAL(fmt, ...) LOG_LEVEL(FATAL, fmt, __VA_ARGS__)


// 日志级别
enum class LogLevel { DEBUG, INFO, WARN, ERROR, FATAL };
std::string toString(LogLevel level);

class Logger;

// 日志事件
class LogEvent {
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent(LogLevel level, const char *file, int32_t line, std::thread::id thread_id, time_t time, const char *fmt, ...)
        : m_file(file), m_line(line), m_threadid(thread_id), m_time(time), m_level(level) {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }

    const char *getFile() const { return m_file; }
    int32_t getLine() const { return m_line; }
    const std::thread::id& getThreadId() const { return m_threadid; }
    const time_t& getTime() const { return m_time; }
    const std::string &getContent() const { return m_content; }
    LogLevel getLevel() const { return m_level; }

    void format(const char *fmt, ...);
    void format(const char *fmt, va_list al);
private:
    const char *m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    std::thread::id m_threadid;        //线程id
    time_t m_time = 0;                //时间戳
    LogLevel m_level = LogLevel::DEBUG;                 //级别
    std::string m_content;
};

// 格式单元(基类)
class FormatItem {
public:
    using ptr = std::shared_ptr<FormatItem>;
    virtual ~FormatItem() {}
    virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;
};

// 日志格式器
class LogFormatter {
public:
    using ptr = std::shared_ptr<LogFormatter>;
    LogFormatter(const std::string &pattern);
    
    std::string format(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event);
    
private:
    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    static std::map<char, std::function<FormatItem::ptr(const std::string &str)>> m_format_mp;
};

// 日志输出器(基类)
class LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender>;
    virtual ~LogAppender() {}
    virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;

    void setFormatter(const std::string &pattern) { m_formatter = LogFormatter::ptr(new LogFormatter(pattern)); }
    void setFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr getFormatter() const { return m_formatter; }
protected:
    LogLevel m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
};

// 标准输出
class LogAppender_stdout : public LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender_stdout>;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override;
private:

};

// 文件输出
class LogAppender_file : public LogAppender {
public:
    using ptr = std::shared_ptr<LogAppender_file>;
    LogAppender_file(const std::string &filename);
    ~LogAppender_file() { ::close(m_fd); }
    virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override;
private:
    std::string m_filename;
    int m_fd;
    LogBuffer::ptr m_buffer;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger> {
public:
    using ptr = std::shared_ptr<Logger>;

    Logger(const std::string &name = "root");

    void log(LogLevel level, LogEvent::ptr event);
    
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel getLevel() const { return m_level; }
    void setLevel(LogLevel level) { m_level = level; }

    const std::string &getName() const { return m_name; }
    
private:
    std::string m_name;                         //日志名称
    LogLevel m_level = LogLevel::DEBUG;                          //日志级别
    std::list<LogAppender::ptr> m_appender_list;    //Appender集合
    // LogFormatter::ptr m_formatter;
};
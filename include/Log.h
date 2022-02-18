#ifndef LOG_H
#define LOG_H

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


// %m -- 消息体
// %p -- level
// %c -- 日志名称
// %t -- 线程id
// %n -- 回车换行
// %d -- 时间
// %f -- 文件名
// %l -- 行号

#define LOG_LEVEL(logger, level, fmt, ...)\
    logger->log(LogLevel::level, LogEvent::ptr(new LogEvent(\
    LogLevel::level,\
    __FILE__,\
    __LINE__,\
    std::this_thread::get_id(),\
    std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()),\
    fmt,\
    __VA_ARGS__\
)));

#define LOG_DEBUG(logger, fmt, ...) LOG_LEVEL(logger, DEBUG, fmt, __VA_ARGS__)
#define LOG_INFO(logger, fmt, ...)  LOG_LEVEL(logger, INFO,  fmt, __VA_ARGS__)
#define LOG_WARN(logger, fmt, ...)  LOG_LEVEL(logger, WARN,  fmt, __VA_ARGS__)
#define LOG_ERROR(logger, fmt, ...) LOG_LEVEL(logger, ERROR, fmt, __VA_ARGS__)
#define LOG_FATAL(logger, fmt, ...) LOG_LEVEL(logger, FATAL, fmt, __VA_ARGS__)


// 日志级别
enum class LogLevel { DEBUG, INFO, WARN, ERROR, FATAL };
std::string toString(LogLevel level);

class Logger;

// 日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
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
    typedef std::shared_ptr<FormatItem> ptr;
    virtual ~FormatItem() {}
    virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;
};

// 日志格式器
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
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
    typedef std::shared_ptr<LogAppender> ptr;
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
    typedef std::shared_ptr<LogAppender_stdout> ptr;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override;
private:

};

// 文件输出
class LogAppender_file : public LogAppender {
public:
    typedef std::shared_ptr<LogAppender_file> ptr;
    LogAppender_file(const std::string &filename);
    virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override;
    
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

// 日志器
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;
    
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





#endif
#include"Log.h"

std::string toString(LogLevel level) {
    switch (level) {
    case LogLevel::DEBUG:
        return "DEBUG";
    case LogLevel::INFO:
        return "INFO";
    case LogLevel::WARN:
        return "WARN";
    case LogLevel::ERROR:
        return "ERROR";
    case LogLevel::FATAL:
        return "FATAL";
    default:
        return "UNKNOWN";
    }
    return "UNKNOWN";
}

void LogEvent::format(const char *fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char *fmt, va_list al) {
    char *buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != -1) {
        m_content.append(buf, len);
        free(buf);
    }
}


class MessageFormatItem : public FormatItem {
public:
    MessageFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getContent();
    }
private:
    std::string m_format;
};

class LevelFormatItem : public FormatItem {
public:
    LevelFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << toString(level);
    }
private:
    std::string m_format;
};

class NameFormatItem : public FormatItem {
public:
    NameFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << logger->getName();
    }
private:
    std::string m_format;
};

class ThreadIdFormatItem : public FormatItem {
public:
    ThreadIdFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
private:
    std::string m_format;
};

class DateTimeFormatItem : public FormatItem {
public:
    DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << std::put_time(std::localtime(&event->getTime()), m_format.c_str());
    }
private:
    std::string m_format;
};

class FilenameFormatItem : public FormatItem {
public:
    FilenameFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getFile();
    }
private:
    std::string m_format;
};

class LineFormatItem : public FormatItem {
public:
    LineFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << event->getLine();
    }
private:
    std::string m_format;
};

class NewLineFormatItem : public FormatItem {
public:
    NewLineFormatItem(const std::string format = "")
        : m_format(format) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << std::endl;
    }
private:
    std::string m_format;
};

class StringFormatItem : public FormatItem {
public:
    StringFormatItem(const std::string str = "")
        :m_str(str) {
    }
    void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) override {
        os << m_str;
    }
private:
    std::string m_str;
};

// Logger
Logger::Logger(const std::string &name)
    :m_name(name) {
}

void Logger::addAppender(LogAppender::ptr appender) {
    m_appender_list.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    for (auto itr = m_appender_list.begin();itr != m_appender_list.end();itr++) {
        if (*itr == appender) {
            m_appender_list.erase(itr);
            break;
        }
    }
}

void Logger::log(LogLevel level, LogEvent::ptr event) {
    if (static_cast<int>(level) >= static_cast<int>(m_level)) {
        auto self = shared_from_this();
        for (auto &x : m_appender_list) {
            x->log(self, level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}


void LogAppender_stdout::log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) {
    if (static_cast<int>(level) >= static_cast<int>(m_level)) {
        std::cout << m_formatter->format(logger, level, event);
    }
}

LogAppender_file::LogAppender_file(const std::string &filename)
    :m_filename(filename) {
    m_filestream.open(filename, std::ios::trunc);
}

bool LogAppender_file::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

void LogAppender_file::log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) {
    if (static_cast<int>(level) >= static_cast<int>(m_level)) {
        m_filestream << m_formatter->format(logger, level, event);
    }
}

// LogFormatter
LogFormatter::LogFormatter(const std::string &pattern)
    :m_pattern(pattern) {
    init();
}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) {
    std::stringstream ss;
    for (auto &i : m_items) {
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

std::map<char, std::function<FormatItem::ptr(const std::string &str)>> LogFormatter::m_format_mp = {
    {'m', [](const std::string &fmt) {return FormatItem::ptr(new MessageFormatItem(fmt));}},
    {'p', [](const std::string &fmt) {return FormatItem::ptr(new LevelFormatItem(fmt));}},
    {'c', [](const std::string &fmt) {return FormatItem::ptr(new NameFormatItem(fmt));}},
    {'t', [](const std::string &fmt) {return FormatItem::ptr(new ThreadIdFormatItem(fmt));}},
    {'n', [](const std::string &fmt) {return FormatItem::ptr(new NewLineFormatItem(fmt));}},
    {'d', [](const std::string &fmt) {return FormatItem::ptr(new DateTimeFormatItem(fmt));}},
    {'f', [](const std::string &fmt) {return FormatItem::ptr(new FilenameFormatItem(fmt));}},
    {'l', [](const std::string &fmt) {return FormatItem::ptr(new LineFormatItem(fmt)); }}
};

void LogFormatter::init() {
    std::string str, format;
    for (int i = 0;i < m_pattern.size();i++) {
        if (m_pattern[i] != '%') {
            str.push_back(m_pattern[i]);
            continue;
        }
        if (i + 1 < m_pattern.size() && m_pattern[i + 1] == '%') {
            str.push_back(m_pattern[++i]);
            continue;
        }
        if (!str.empty()) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(str)));
            str.clear();
        }
        auto it = m_format_mp.find(m_pattern[++i]);
        if (it != m_format_mp.end()) {
            if (i + 1 < m_pattern.size() && m_pattern[i + 1] == '{') {
                i++;
                while (i + 1 < m_pattern.size() && m_pattern[i + 1] != '}') {
                    format.push_back(m_pattern[++i]);
                }
                if (i + 1 < m_pattern.size() && m_pattern[i + 1] == '}') {
                    i++;
                } else {
                    throw std::runtime_error("logformatter parse error");
                }
            }
            m_items.push_back(it->second(format));
            format.clear();
        } else {
            throw std::runtime_error("logformatter parse error");
        }
    }
    if (!str.empty()) {
        m_items.push_back(FormatItem::ptr(new StringFormatItem(str)));
    }
}

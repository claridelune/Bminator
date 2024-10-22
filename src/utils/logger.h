#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <ctime>
#include <mutex>

class Logger {
public:
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        NONE
    };

    // (singleton)
    static Logger& getInstance();

    void setLogLevel(LogLevel level);

    void setActive(bool active);

    void setOutputStream(std::ostream& outputStream);

    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

private:
    LogLevel currentLogLevel;
    bool isActive;
    std::ostream* outputStream;
    std::mutex logMutex;

    Logger();
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string getCurrentTime();
    std::string logLevelToString(LogLevel level);
};

#endif

#include "logger.h"

Logger::Logger() : currentLogLevel(LogLevel::DEBUG), isActive(true), outputStream(&std::cout) {}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    currentLogLevel = level;
}

void Logger::setActive(bool active) {
    std::lock_guard<std::mutex> lock(logMutex);
    isActive = active;
}

void Logger::setOutputStream(std::ostream& stream) {
    std::lock_guard<std::mutex> lock(logMutex);
    outputStream = &stream;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (isActive && level >= currentLogLevel) {
        // std::string time = getCurrentTime();
        // *outputStream << "[" << time << "] [" << logLevelToString(level) << "]: " << message << std::endl;
        *outputStream << "[" << logLevelToString(level) << "]: " << message << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::NONE: return "NONE";
    }
    return "UNKNOWN";
}

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return buf;
}

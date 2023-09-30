#pragma once

#include <fmt/color.h>

#define DEBUG 1

enum LogLevel
{
    ERROR = 1,
    WARNING = 2,
    INFO = 4,
    OPENGLAPI = 8
};

#if DEBUG
#define LOG_ERROR(...) fmt::print(fg(fmt::terminal_color::red), "[ERROR] {}\n", fmt::format( __VA_ARGS__))
#define LOG_WARN(...)  fmt::print(fg(fmt::terminal_color::yellow), "[WARN] {}\n", fmt::format( __VA_ARGS__))
#define LOG_INFO(...)  fmt::print(fg(fmt::terminal_color::green), "[INFO] {}\n", fmt::format( __VA_ARGS__))
#define LOG_OPENGLAPI(...)  fmt::print(fg(fmt::terminal_color::cyan), "[GLAPI] {}\n", fmt::format( __VA_ARGS__))

extern int logLevel;

#define LOG(level, ...) \
    if ((level & logLevel) == level) \
    { \
        if (level == LogLevel::ERROR) \
            LOG_ERROR(__VA_ARGS__); \
        else if (level == LogLevel::WARNING) \
            LOG_WARN(__VA_ARGS__); \
        else if (level == LogLevel::INFO) \
            LOG_INFO(__VA_ARGS__); \
        else \
            LOG_OPENGLAPI(__VA_ARGS__); \
    }

#else
#define LOG_ERROR(...)
#define LOG_WARN(...)
#define LOG_INFO(...)
#define LOG(...)
#endif

#if DEBUG
#define ASSERT(exp, msg)  \
    if (!(exp))           \
        fmt::print(fg(fmt::terminal_color::red), "[ASSERT] {}:{} \'{}\' failed: {}\n", __FILE__, __LINE__, #exp, msg)
#else
#define ASSERT(exp, msg)
#endif

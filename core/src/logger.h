#pragma once

#include <fmt/color.h>


namespace logger {
    enum class Level {
        Debug = 1,
        Info = 2,
        Warning = 4,
        Error = 8,
    };

    inline Level operator&(Level a, Level b) {
        return static_cast<Level>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline Level operator|(Level a, Level b) {
        return static_cast<Level>(static_cast<int>(a) | static_cast<int>(b));
    }

#ifdef DEBUG
    constexpr static bool debug = true;
#else
    constexpr static bool debug = false;
#endif

    void setFilter(Level level, bool isInclusive);

    bool checkLevel(Level level);

    template<typename... T>
    inline void log(Level level, fmt::format_string<T...> msg, T&&... args) {
        if constexpr (debug) {
            switch (level) {
                case Level::Debug:
                    if (checkLevel(Level::Debug)) {
                        fmt::print("[DEBUG] {}\n", fmt::format(msg, std::forward<T>(args)...));
                    }
                    break;
                case Level::Info:
                    if (checkLevel(Level::Info)) {
                        fmt::print(fg(fmt::terminal_color::green), "[INFO] {}\n", fmt::format(msg, std::forward<T>(args)...));
                    }
                    break;
                case Level::Warning:
                    if (checkLevel(Level::Warning)) {
                        fmt::print(fg(fmt::terminal_color::yellow), "[WARNING] {}\n", fmt::format(msg, std::forward<T>(args)...));
                    }
                    break;
                case Level::Error:
                    if (checkLevel(Level::Error)) {
                        fmt::print(fg(fmt::terminal_color::red), "[ERROR] {}\n", fmt::format(msg, std::forward<T>(args)...));
                    }
                    break;
                default:
                    break;
            }
        }
    }

    template<typename... T>
    inline void log(fmt::format_string<T...> msg, T&&... args) {
        if constexpr (debug) {
            if (checkLevel(Level::Debug)) {
                fmt::print("[DEBUG] {}\n", fmt::format(msg, std::forward<T>(args)...));
            }
        }
    }

    template<typename... T>
    inline void logInfo(fmt::format_string<T...> msg, T&&... args) {
        if constexpr (debug) {
            if (checkLevel(Level::Info)) {
                fmt::print(fg(fmt::terminal_color::green), "[INFO] {}\n", fmt::format(msg, std::forward<T>(args)...));
            }
        }
    }

    template<typename... T>
    inline void logWarning(fmt::format_string<T...> msg, T&&... args) {
        if constexpr (debug) {
            if (checkLevel(Level::Warning)) {
                fmt::print(fg(fmt::terminal_color::yellow), "[WARNING] {}\n", fmt::format(msg, std::forward<T>(args)...));
            }
        }
    }

    template<typename... T>
    inline void logError(fmt::format_string<T...> msg, T&&... args) {
        if constexpr (debug) {
            if (checkLevel(Level::Error)) {
                fmt::print(fg(fmt::terminal_color::red), "[ERROR] {}\n", fmt::format(msg, std::forward<T>(args)...));
            }
        }
    }
}


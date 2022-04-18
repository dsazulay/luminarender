//
// Created by Diego Azulay on 18/04/22.
//

#ifndef INTERACTIVEGRAPHICS_LOG_H
#define INTERACTIVEGRAPHICS_LOG_H

#include <iostream>

#define DEBUG 1

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#if DEBUG
#define LOG_ERROR(msg) log_(RED, "[ERROR]", msg)
#define LOG_WARN(msg)  log_(YELLOW, "[WARN]", msg)
#define LOG_INFO(msg)  log_(GREEN, "[INFO]", msg)
#else
#define LOG_ERROR(x)
#define LOG_WARN(x)
#define LOG_INFO(x)
#endif

#if DEBUG
#define ASSERT(exp, msg) if (!(exp)) assert_(__ASSERT_FILE_NAME, __LINE__, #exp, msg)
#else
#define ASSERT(cond, msg)
#endif

void log_(const char* color, const char* tag, const char* msg)
{
    std::cout << color << tag << " " << msg << RESET << std::endl;
}

void assert_(const char* filename, int line, const char* exp, const char* msg)
{
    std::cout << RED << "[ASSERT] " << filename << ":" << line << " \'" << exp
        << "\' failed: " << msg << RESET << std::endl;
}

#endif //INTERACTIVEGRAPHICS_LOG_H

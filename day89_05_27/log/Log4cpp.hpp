#ifndef __WD_Logger_HPP_
#define __WD_Logger_HPP_

#include <stdio.h>
#include <iostream>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/RollingFileAppender.hh>

using std::string;
using namespace log4cpp;

class Logger {
   public:
    static Logger* getInstance();

    static void destroy() ;
    
    template <class... Args>
    void error(const char* msg, Args... args) {
        _rootCat.error(msg, args...);
        // _subCat.error(msg, args...);
    }

    template <class... Args>
    void warn(const char* msg, Args... args) {
        _rootCat.warn(msg, args...);
        // _subCat.warn(msg, args...);
    }

    template <class... Args>
    void debug(const char* msg, Args... args) {
        _rootCat.debug(msg, args...);
        // _subCat.debug(msg, args...);
    }

    template <class... Args>
    void info(const char* msg, Args... args) {
        _rootCat.info(msg, args...);
        // _subCat.info(msg, args...);
    }

   private:
    Logger(log4cpp::Category& root, log4cpp::Category& sub1)
        : _rootCat(root), _subCat(sub1) {}
    ~Logger() { log4cpp::Category::shutdown(); }

   private:
    static Logger* _pInstance;
    log4cpp::Category& _rootCat;
    log4cpp::Category& _subCat;
};

//Logger* Logger::_pInstance = nullptr;

inline string int2str(int number) {
    std::ostringstream oss;
    oss << number;
    return oss.str();
}

#define addprefix(msg)             \
    string("[")                    \
        .append(__FILE__)          \
        .append("]")               \
        .append("-")               \
        .append("[")               \
        .append(__FUNCTION__)      \
        .append(": ")              \
        .append(int2str(__LINE__)) \
        .append("] => ")           \
        .append(msg)               \
        .c_str()

#define LogError(msg, ...) \
    Logger::getInstance()->error(addprefix(msg), ##__VA_ARGS__)
#define LogWarn(msg, ...) \
    Logger::getInstance()->warn(addprefix(msg), ##__VA_ARGS__)
#define LogInfo(msg, ...) \
    Logger::getInstance()->info(addprefix(msg), ##__VA_ARGS__)
#define LogDebug(msg, ...) \
    Logger::getInstance()->debug(addprefix(msg), ##__VA_ARGS__)

#endif

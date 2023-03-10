#pragma once
#include "LogStream.h"
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string>
#include <functional>

//Logger 类为用户提供使用日志库的接口，其内部有一个 Impl（实现类）具体实现功能。Logger 内部定义了日志的等级。
class Logger {
    public:
     enum LogLevel{
        INFO,
        WARN,
        ERROR,
        LEVEL_COUNT,
     };
     Logger(const char* file, int line);
     Logger(const char* file, int line, LogLevel level);
     Logger(const char* file, int line, LogLevel level, const char* func);
     ~Logger();
     //流是会改变的
     LogStream& stream() { return impl_.stream_; }
     //
     static LogLevel logLevel();
     static void setLogLevel(LogLevel level);
     //输出函数和刷新缓冲区函数
     using OutputFunc = std::function<void(const char* msg, int len)>;
     using FlushFunc = std::function<void()>;
     static void setOutput(OutputFunc);
     static void setFlush(FlushFunc);

    private:
    //内部类
     class Impl{
        public:
         using LogLevel = Logger::LogLevel;
         Impl(LogLevel level, const char* file, int line);
         void formatTime();
         void finish();

         LogStream stream_;
         LogLevel level_;
         int line_;
         std::string basename_;
     };

     Impl impl_;
     static std::string logFileName_; //log文件名，用于代码内部设置，也可以不用这个，外部传入

};

extern Logger::LogLevel g_logLevel;
inline Logger::LogLevel logLevel(){
    return g_logLevel;
}

#define LOG_INFO Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
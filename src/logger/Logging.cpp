#include "Logging.h"
#include "CurrentThread.h"
#include <time.h>
#include <sys/time.h>

const char* getLevelName[Logger::LogLevel::LEVEL_COUNT]
{
    "INFO ",
    "WARN ",
    "ERROR ",
};


//默认等级是INFO
Logger::LogLevel initLogLevel(){
    return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();


//默认输出到终端
static void defaultOutput(const char* data, int len){
    fwrite(data, len, sizeof(char), stdout);
}
//默认刷新到终端
static void defaultFlush(){
    fflush(stdout);
}
Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Impl::Impl(Logger::LogLevel level, const char* file, int line)
    :stream_(),
     level_(level),
     line_(line),
     basename_(file){
        //输出时间
        formatTime();
        //输出日志等级
        stream_ << GeneralTemplate(getLevelName[level], 6);
     }

void Logger::Impl::formatTime(){
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream_ << str_t;
}

void Logger::Impl::finish(){
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(const char* file, int line)
    : impl_(INFO, file, line){}

Logger::Logger(const char* file, int line, Logger::LogLevel level)
    : impl_(level, file, line){}

//可以打印调用的函数
Logger::Logger(const char* file, int line, Logger::LogLevel level, const char* func)
    : impl_(level, file, line){
        impl_.stream_ << func << ' ';
    }

Logger::~Logger(){
    impl_.finish();
    //获取buffer
    const LogStream::Buffer& buf(stream().buffer());
    //输出，默认是向终端输出
    g_output(buf.data(), buf.length());
}

void Logger::setLogLevel(Logger::LogLevel level){
    g_logLevel = level;
}

void Logger::setOutput(OutputFunc out){
    g_output = out;
}

void Logger::setFlush(FlushFunc flush){
    g_flush = flush;
}


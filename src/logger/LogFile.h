#pragma once
#include <memory>
#include <string>
#include "FileUtil.h"
#include <mutex>
#include "noncopyable.h"

//提供对日志文件的操作
class LogFile : noncopyable {
    public:
    //每被append N次以后flush一下，往文件里写，文件也是带缓冲区的
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile(); //滚动日志

    private:
     static std::string getLogFileName(const std::string& basename, time_t* now);
     void append_unlocked(const char* logline, int len);
     const std::string basename_;
     const int flushEveryN_;
     int count_;
     
     std::unique_ptr<std::mutex> mutex_;
     std::unique_ptr<AppendFile> file_;
};
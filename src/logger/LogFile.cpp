#include "LogFile.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "FileUtil.h"

using namespace std;

LogFile::LogFile(const string& basename, int flushEveryN)
    : basename_(basename),
      flushEveryN_(flushEveryN),
      count_(0),
      mutex_(new std::mutex) //md，之前漏了这里的mutex初始化，直接报段错误，让我好找，不过竟然是AsyncLogging.h里调用cond.notify_one()时候报的段错误，果然是因为用了这个mutex但是没初始化吗
      {
        time_t now = 0;
        std::string filename = getLogFileName(basename_, &now);
        file_.reset(new AppendFile(filename)); //重置智能指针
}

LogFile::~LogFile()=default;

void LogFile::append(const char* logline, int len){
     std::lock_guard<std::mutex> guard(*mutex_); 
     append_unlocked(logline, len);
}

void LogFile::flush() {
     std::lock_guard<std::mutex> guard(*mutex_); 
     file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) {
    file_->append(logline, len);
    ++count_;
    if(count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now){
    std::string filename;
    //filename.reverse(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    localtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S", &tm);
    filename += timebuf;
    filename += ".log";
    return filename;
    
}
#pragma once
#include <string>
#include "noncopyable.h"


//封装FILE对文件操作的方法
class AppendFile : noncopyable {
    public:
     explicit AppendFile(std::string& filename);
     ~AppendFile();
     void append(const char *logline, const size_t len); //添加log消息到文件末尾
     void flush(); //冲刷文件到磁盘
     off_t writtenBytes() const { return writtenBytes_; }  //返回已写的字节数

    private:
     size_t write(const char *logling, size_t len); //写数据到文件
     FILE *fp_;  //文件指针
     char buffer_[64 * 1024];  //文件操作的缓冲区
     off_t writtenBytes_;   //已写的字节数
};
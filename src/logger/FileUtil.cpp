#include "FileUtil.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

AppendFile::AppendFile(string& filename) : fp_(fopen(filename.c_str(), "ae")), writtenBytes_(0) {
    //用户提供缓冲区，即将缓冲区设置为本地的buffer_
    setbuffer(fp_, buffer_, sizeof buffer_);  //在打开文件流后,读取内容之前,调用 setbuffer() 可用来设置文件流的缓冲区.
    //参数分别是文件流、自定的缓冲区起始地址、缓冲区大小
}

AppendFile::~AppendFile(){ fclose(fp_); }

void AppendFile::append(const char* logline, const size_t len){
    size_t n = this->write(logline, len); //写进了多少
    size_t remain = len - n; //剩下多少没写
    //循环直到写完为止
    while(remain > 0) {
        size_t x = this->write(logline + n, remain);
        if(x == 0) {
            int err = ferror(fp_);
            if(err) fprintf(stderr, "ApendFile::append() failed !\n");
            break;
        }
        n += x;  //更新写入的
        remain = len -n;  //更新剩下的
    }
}

void AppendFile::flush() { fflush(fp_); } //大多数文件都是全缓冲的，当缓冲区满了才会写到磁盘，fflush这个函数可以马上写磁盘

size_t AppendFile::write(const char* logline, size_t len) {
    return fwrite_unlocked(logline, 1, len, fp_); //写的时候是单线程操作，因此可以使用fwrite非加锁版本::fwrite_unlocked函数提高效率
    //写文件的函数，参数分别是数据存储地址、要读取字节大小、取多少个、等待被读取的数据源，是一个指向FILE结构的文件指针
    //返回写入元素个数
}
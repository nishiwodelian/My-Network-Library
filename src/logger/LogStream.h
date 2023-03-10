#pragma once
#include <assert.h>
#include <string.h>
#include <string>
#include "noncopyable.h"

class GeneralTemplate : noncopyable{
   public:
    GeneralTemplate()
      :data_(nullptr),
      len_(0){}

    explicit GeneralTemplate(const char* data, int len)
      :data_(data),
      len_(len){}
   
    const char* data_;
    int len_;
};


//FixedBuffer是存放日志信息的缓冲区
class AsyncLogging;
const int kSmallBuffer = 4000;  // 4KB
const int kLargeBuffer = 4000*4000;  // 4MB

template <int SIZE>
class FixedBuffer : noncopyable{
    public:
     FixedBuffer() : cur_(data_) {}
     ~FixedBuffer() {}
     
     //向缓冲区添加数据
     void append(const char* buf, size_t len) {
        //添加一部分
        if(avail() > static_cast<int>(len)) //强制类型转换了一下
        {
            //将buf地址，长为len的数据拷贝到cur_地址处
            memcpy(cur_, buf, len);
            cur_ += len;
        }
     }

     const char* data() const {return data_;}
     int length() const { return static_cast<int>(cur_ - data_);} //返回已存数据大小
     //直接写入data_
     char* current() { return cur_; }   //返回当前地址
     int avail() const { return static_cast<int>(end() - cur_); } //返回可用缓冲区大小
     void add(size_t len) {cur_ += len;}  

     void reset() {cur_ = data_;}  //重置缓冲区
     void bzero() {memset(data_, 0, sizeof data_);} //清空缓冲区

    private:
     const char* end() const { return data_ + sizeof data_; }  //缓冲区末尾
     char data_[SIZE];
     char* cur_;

};


//LogStream 重载了一系列 operator<< 操作符，将不同格式数据转化为字符串，并存入 LogStream::buffer_
//LogStream 类用于重载正文信息，一次信息大小是有限的，其使用的缓冲区的大小就是 kSmallBuffer
class LogStream : noncopyable {
    public:
     typedef FixedBuffer<kSmallBuffer> Buffer;

     LogStream& operator<<(bool v) {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
     }
     LogStream& operator<<(short);
     LogStream& operator<<(unsigned short);
     LogStream& operator<<(int);
     LogStream& operator<<(unsigned int);
     LogStream& operator<<(long);
     LogStream& operator<<(unsigned long);
     LogStream& operator<<(long long);
     LogStream& operator<<(unsigned long long);

     LogStream& operator<<(const void*);
     LogStream& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
     }
     LogStream& operator<<(double);
     LogStream& operator<<(long double);
     LogStream& operator<<(char v) {
        buffer_.append(&v, 1);
        return *this;
     }
     LogStream& operator<<(const char* str) {
        if (str)
            buffer_.append(str, strlen(str));
        else
            buffer_.append("(null)", 6);
        return *this;
     }
     LogStream& operator<<(const unsigned char* str) {
        return operator<<(reinterpret_cast<const char*>(str));//类型转换
     }
     LogStream& operator<<(const std::string& v) {
        buffer_.append(v.c_str(), v.size());
        return *this;
     }
      // (const char*, int)的重载
     LogStream& operator<<(const GeneralTemplate& g);
    
     void append(const char* data, int len) { buffer_.append(data, len);}
     const Buffer& buffer() const { return buffer_; }
     void resetBuffer() { buffer_.reset(); }


    private:
     void staticCheck();

     template <typename T>
     void formatInteger(T);

     Buffer buffer_;
     static const int kMaxNumericSize = 32;
};
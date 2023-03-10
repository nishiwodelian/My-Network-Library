#include "LogStream.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <limits>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

template <typename T>
size_t convert(char buf[], T value){
    T i=value;
    char *p = buf;

    //末尾取值加入，最后反转
    do{
        int lsd = static_cast<int>(i%10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if(value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template class FixedBuffer<kSmallBuffer>;
template class FixedBuffer<kLargeBuffer>;

template< typename T>
void LogStream::formatInteger(T v) {
    // buffer容不下kMaxNumbericSize个字符的话会被直接丢弃
    if(buffer_.avail() >= kMaxNumericSize) {
        size_t len = convert(buffer_.current(), v);
        buffer_.add(len);
    }
}

LogStream& LogStream::operator<<(short v){
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
  *this << static_cast<unsigned int>(v);
  return *this;
}

LogStream& LogStream::operator<<(int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
  formatInteger(v);
  return *this;
}

LogStream& LogStream::operator<<(double v) {
    if (buffer_.avail() >= kMaxNumericSize) {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v); //% 开头的可选格式说明符，表示输出12位有效数字
        buffer_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v) {
  if (buffer_.avail() >= kMaxNumericSize) {
    int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12Lg", v);
    buffer_.add(len);
  }
  return *this;
}

LogStream& LogStream::operator<<(const GeneralTemplate& g){
  buffer_.append(g.data_, g.len_);
  return *this;
}
#pragma once
#include <string>

class Buffer{
    public:
     Buffer() = default;
     ~Buffer() = default;

     void Append(const char *_str, int _size);
     //void Append(const char *_str);
     void Append(std::string str);
     ssize_t Size();
     const char *ToStr();
     void Clear();
     void GetLine();
     void SetBuf(const char *buf);

    private:
     std::string buf_;
};
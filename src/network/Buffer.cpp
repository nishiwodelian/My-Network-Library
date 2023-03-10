#include "Buffer.h"
#include <cstring>
#include <iostream>

void Buffer::Append(const char *str, int size){
    for(int i=0;i<size;++i){
        if(str[i] == '\0'){
            break;
        }
        buf_.push_back(str[i]);
    }
}



void Buffer::Append(std::string str){
    buf_ = buf_ + str;
}

ssize_t Buffer::Size(){
    return buf_.size();
}

const char *Buffer::ToStr(){
    return buf_.c_str();
}

void Buffer::Clear(){
    buf_.clear();
}

void Buffer::GetLine(){
    buf_.clear();
    std::getline(std::cin, buf_);
}

void Buffer::SetBuf(const char *buf){
    buf_.clear();
    buf_.append(buf);
}
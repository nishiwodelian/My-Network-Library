#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "noncopyable.h"
#include <unordered_map>

class HttpRequest{
    public:
     enum Method {
        Invalid,
        Get,
        Post,
        Head, 
        Put, 
        Delete,
     };
     enum Version{
        Unknown,
        Http10,
        Http11,
     };

     HttpRequest()
        : method_(Invalid),
          version_(Unknown){}
    
     void setVersion(Version v){
        version_ = v;
     }

     Version version() const {
        return version_;
     }

     bool setMethod(const char *start, const char *end){
        std::string m(start, end);
        if (m == "GET")
        {
            method_ = Get;
        }
        else if (m == "POST")
        {
            method_ = Post;
        }
        else if (m == "HEAD")
        {
            method_ = Head;
        }
        else if (m == "PUT")
        {
          method_ = Put;
        }
        else if (m == "DELETE")
        {
          method_ = Delete;
        }
        else
        {
          method_ = Invalid;
        }
        return method_ != Invalid;
     }

     Method method() const {
        return method_; 
     }

     const char* methodString() const{
        const char* result = "UNKNOWN";
        switch(method_)
        {
          case Get:
            result = "GET";
            break;
          case Post:
            result = "POST";
            break;
          case Head:
            result = "HEAD";
            break;
          case Put:
            result = "PUT";
            break;
          case Delete:
            result = "DELETE";
            break;
          default:
            break;
        }
        return result;
     }

     void setPath(const char* start, const char* end){
        path_.assign(start, end);
     }
     const std::string& path() const{
        return path_;
     }
     void setQuery(const char *start, const char* end){
        query_.assign(start, end);
     }
     const std::string& query() const{
        return query_;
     }
     void addHeader(const char* start, const char* colon, const char* end){
        std::string field(start, colon);
        ++colon;
        //跳过空格
        while(colon < end && isspace(*colon)){
            ++colon;
        }
        std::string value(colon, end);
        while(!value.empty() && isspace(value[value.size()-1])){
            value.resize(value.size()-1);  //丢掉后面的空格
        }
        headers_[field] = value;
     }
     //获取请求头部的对应值
     std::string getHeader(const std::string &field) const {
        std::string result;
        auto it = headers_.find(field);
        if(it != headers_.end()){
            result = it->second;
        }
        return result;
     }
     const std::unordered_map<std::string, std::string>& headers() const {
        return headers_;
     }
     void swap(HttpRequest &rhs){  //右值
        std::swap(method_, rhs.method_);
        std::swap(version_, rhs.version_);
        path_.swap(rhs.path_);
        query_.swap(rhs.query_);
        headers_.swap(rhs.headers_);
     }

    private:
     Method method_;  //请求方法
     Version version_; //协议版本号
     std::string path_; //请求路径
     std::string query_;  //询问参数
     std::unordered_map<std::string, std::string> headers_;  //请求头部列表
};

#endif /* HTTPREQUEST_H */
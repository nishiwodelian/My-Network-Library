#include "HttpRequest.h"


class ParseWorker;
class HttpContext{
    public:
     enum HttpRequestParseState{
        ExpectRequestLine, //解析请求行状态
        ExpectHeaders,   //解析请求头部状态
        ExpectBody,   //解析请求体状态
        GotAll,   //解析完毕状态
     };

     HttpContext()
        : state_(ExpectRequestLine){}

     bool parseRequest(ParseWorker* worker);

     bool gotAll() const {
        return state_ == GotAll;
     }
     //重置HttpContext状态，异常安全
     void reset(){
        state_ = ExpectRequestLine;
        HttpRequest dumy;
        request_.swap(dumy);
     }

     const HttpRequest& request() const {
        return request_;
     }
     HttpRequest& request() {
        return request_;
     }


    private:
     bool processRequestLine(const char* begin, const char* end);
     HttpRequestParseState state_;
     HttpRequest request_;
};
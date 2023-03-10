#include "HttpContext.h"
#include "ParseWorker.h"

//解析请求行
bool HttpContext::processRequestLine(const char* begin, const char* end){
    bool succeed = false;
    const char *start = begin;
    const char *space = std::find(start, end, ' ');

    //如果不是最后一个空格并且，成功获取了method并设置到request_
    if(space !=end && request_.setMethod(start, space)){
        start = space+1;
        space = std::find(start, end, ' ');
        if(space != end){
            //查看是否有请求参数
            const char* question = std::find(start, space, '?');
            if(question != space){
                request_.setPath(start, question);//设置访问路径
                request_.setQuery(question, space);//设置访问变量
            }else{
                request_.setPath(start, space);
            }
            start = space +1;
            succeed = (end-start == 8 && std::equal(start, end-1, "HTTP/1."));
            if(succeed){
                if(*(end-1) == '1'){
                    request_.setVersion(HttpRequest::Http11);
                }else if(*(end-1) == '0'){
                    request_.setVersion(HttpRequest::Http10);
                }else{
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(ParseWorker* worker){
    bool ok = false;
    bool hasMore = true;
    while(hasMore){
        //请求行状态
        if(state_ == ExpectRequestLine){
            const char* crlf = worker->findCRLF(); // CRLF是\r\n
            if(crlf){
                 ok = processRequestLine(worker->peek(), crlf);
                 if(ok){
                    worker->retrieveUntil(crlf + 2);
                    state_ = ExpectHeaders;
                 }else{
                    hasMore = false;
                 }
            }else{
                hasMore = false;
            }
        }else if(state_ = ExpectHeaders){
            const char* crlf = worker->findCRLF();
            if(crlf){
                const char* colon = std::find(worker->peek(), crlf, ':');
                if(colon != crlf){
                    request_.addHeader(worker->peek(), colon, crlf);
                }else{
                    state_ = GotAll;
                    hasMore = false;
                }
                worker->retrieveUntil(crlf +2);
            }else{
                hasMore = false;
            }
        }else if(state_ = ExpectBody){

        }
    }
    return ok; 
}
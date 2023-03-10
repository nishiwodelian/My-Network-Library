#include "HttpResponse.h"
#include "Buffer.h"

#include <stdio.h>
#include <string.h>

void HttpResponse::appendToBuffer(Buffer* output) const{
    char buf[32];
    memset(buf, '\0', sizeof(buf));
    sprintf(buf, "HTTP/1.1 %d ", statusCode_);
    output->Append(buf);
    output->Append(statusMessage_);
    output->Append("\r\n");

    if(closeConnection_){
        output->Append("Connection: closer\r\n");
    }
    else{
        snprintf(buf, sizeof(buf), "Content-Length: %zd\r\n", body_.size());
        output->Append(buf);
        output->Append("Connection: Keep-Alive\r\n");
    }

    for(const auto& header : headers_){
        output->Append(header.first);
        output->Append(": ");
        output->Append(header.second);
        output->Append("\r\n");
    }
    output->Append("\r\n");
    output->Append(body_);
}
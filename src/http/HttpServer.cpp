#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpContext.h"
#include "Buffer.h"
#include <memory>

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->setStatusCode(HttpResponse::_404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer()
    : worker_(),
      httpCallback_(defaultHttpCallback)
{
    server_ = std::make_unique<Server>();
    server_->NewConnect(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_->OnMessage(std::bind(&HttpServer::onMessage, this,std::placeholders::_1));
}

void HttpServer::Start(){
    server_->Start();
}

void HttpServer::onConnection(Connection *conn){
    if (conn->Connected)
    {
        //LOG_INFO << "new Connection arrived";
    }
    else 
    {
        LOG_INFO << "Connection closed";
    }
}

void HttpServer::onMessage(Connection *conn){
    if(!conn->isConnected()){
//         LOG_INFO << "disconnected! :  ";
         return;
    }
    std::unique_ptr<HttpContext> context(new HttpContext);
    std::string str = conn->ReadBuffer();
    worker_.SetStr(str);
    if(!context->parseRequest(&worker_)){
//        LOG_INFO << "parseRequest failed!  str:  " << str;
        //LOG_INFO << worker_.Getstr();
        
        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
    //     LOG_INFO << "success send 400 !";
        conn->shutdown();
        //conn->deleteConnection();//神奇神奇
        //conn->Close();
        //注意要清理这个conn
    }
    if(context->gotAll()){
        //LOG_INFO << "parseRequest success!";
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(Connection *conn, const HttpRequest& req){
    const std::string& connection = req.getHeader("Connection");
     // 判断长连接还是短连接
    bool close = connection == "close" ||
        (req.version() == HttpRequest::Http10 && connection != "Keep-Alive");
    // TODO:这里有问题，但是强制改写了
    close = true;
    HttpResponse response(close);
    // httpCallback_ 由用户传入，怎么写响应体由用户决定
    // 此处初始化了一些response的信息，比如响应码，回复OK
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    // TODO:需要重载 TcpConnection::send 使其可以接收一个缓冲区
    conn->Send(buf.ToStr());
    if (response.closeConnection())
    {
        conn->shutdown();
        //conn->deleteConnection();//神奇神奇
        //conn->Close();
        //注意要清理这个conn
    }

}
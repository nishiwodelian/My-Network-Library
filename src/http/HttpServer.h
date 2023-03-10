#include "Server.h"
#include "Connection.h"
#include "noncopyable.h"
#include "Logging.h"
#include "ParseWorker.h"
#include <string>
#include <memory>

class HttpRequest;
class HttpResponse;

class HttpServer : noncopyable{
    public:
     using HttpCallback = std::function<void (const HttpRequest&, HttpResponse*)>;

     HttpServer();

     void setHttpCallback(const HttpCallback& cb){
        httpCallback_ = cb;
     }
     void Start();
    
    private:
     void onConnection(Connection *conn);
     void onMessage(Connection *conn);
     void onRequest(Connection *conn, const HttpRequest&);

     std::unique_ptr<Server> server_;
     ParseWorker worker_;
     HttpCallback httpCallback_;

};
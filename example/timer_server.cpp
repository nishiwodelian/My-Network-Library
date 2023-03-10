#include "Server.h"
#include <iostream>
#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "TimerQueue.h"
#include "AsyncLogging.h"
#include "Logging.h"
#include <sys/time.h>
#include <string>

size_t getNow() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
}

int main(int argc, char* argv[]){
    Server *server = new Server();
    server->NewConnect(
        [](Connection *conn) {
            std::cout << "New connection fd: " << conn->GetSocket()->GetFd() << std::endl;
             if (conn->GetState() == Connection::State::Connected){
                conn->setContext(getNow());
             }
        }
    );

    server->OnMessage(
        [](Connection *conn) {
            std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
            if (conn->GetState() == Connection::State::Connected) {
                //std::string str = std::to_string(getNow() - conn->getContext());
                //str = str + "  " + conn->ReadBuffer();
                conn->Send(conn->ReadBuffer()); //处理业务
                conn->setContext(getNow());
            }
        }
    );
    AsyncLogging log(::basename(argv[0]));
    //LOG_INFO << argv[0];
    server->CheckActiveConnection(5000);
    server->Start();
    delete server;
    return 0;
}
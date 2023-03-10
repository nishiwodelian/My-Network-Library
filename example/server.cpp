#include "Server.h"
#include <iostream>
#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"

int main(){
    Server *server = new Server();
    server->NewConnect(
        [](Connection *conn) {
            std::cout << "New connection fd: " << conn->GetSocket()->GetFd() << std::endl;
        }
    );

    server->OnMessage(
        [](Connection *conn) {
            std::cout << "Message from client " << conn->ReadBuffer() << std::endl;
            if (conn->GetState() == Connection::State::Connected) {
                conn->Send(conn->ReadBuffer()); //处理业务
            }
        }
    );

    server->Start();
    delete server;
    return 0;
}
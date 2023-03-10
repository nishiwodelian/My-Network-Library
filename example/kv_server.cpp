#include "Server.h"
#include <iostream>
#include "Buffer.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "SkipListWorker.h"
#include <string>

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
                std::string cmd = conn->ReadBuffer();
                std::string res = SkipListWorker::dealWithCmd(cmd);
                //SkipListWorker::_myDB.dumpFile();
                conn->Send(res); //处理业务
            }
        }
    );

    server->Start();
    delete server;
    return 0;
}
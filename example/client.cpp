#include <Connection.h>
#include <Socket.h>
#include <iostream>

int main(){
    Socket *sock = new Socket();
    sock->Connect("127.0.0.1", 1234);

    Connection *conn = new Connection(sock->GetFd(), nullptr);

    while(true){
        conn->GetlineSendBuffer();
        conn->Write();
        if(conn->GetState() == Connection::State::Closed){
            conn->Close();
            break;
        }
        conn->Read();
        std::cout << "Message from server: " << conn->ReadBuffer() << std::endl;
    }
    delete conn;
    delete sock;
    return 0;
}
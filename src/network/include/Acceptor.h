#pragma once
#include <functional>
#include <memory>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor{
    public:
     Acceptor(EventLoop *_loop);
     ~Acceptor();
     void AcceptConnection();
     void SetNewConnectionCallback(std::function<void(int)> const &callback);
    private:
     std::unique_ptr<Socket> sock_;
     std::unique_ptr<Channel> accept_channel_;
     std::function<void(int)> new_connection_callback_;
};

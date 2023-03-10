#pragma once
#include <functional>
#include <map>
#include <vector>
#include <memory>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server {
    public:
     explicit Server();
     ~Server();

     void Start();
     void CheckActiveConnection(size_t t);

     void NewConnection(int fd);
     void DeleteConnection(int fd);
     void OnConnect(std::function<void(Connection *)> fn);
     void OnMessage(std::function<void(Connection *)> fn);
     void NewConnect(std::function<void(Connection *)> fn);
     size_t getNow();

    private:
     std::unique_ptr<EventLoop> main_reactor_;
     std::unique_ptr<Acceptor> acceptor_;
     std::unordered_map<int, std::unique_ptr<Connection>> connections_;
     std::vector<std::unique_ptr<EventLoop>> sub_reactors_;
     std::unique_ptr<ThreadPool> thread_pool_;

     std::function<void(Connection *)> on_connect_callback_;
     std::function<void(Connection *)> on_message_callback_;   //用户处理业务逻辑，connection里的Bussiness函数把如缓冲区和用户处理业务放在了一起，客户端的请求存在connection的读缓冲区里
     std::function<void(Connection *)> new_connect_callback_;   //新建连接时的反应
};
#include "Server.h"
#include <unistd.h>
#include <functional>
#include "Acceptor.h"
#include "Connection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

#include "Logging.h"
#include <string>
#include <vector>

Server::Server() {
    main_reactor_ = std::make_unique<EventLoop>();
    acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
    std::function<void(int)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor_->SetNewConnectionCallback(cb);

    int size = static_cast<int>(std::thread::hardware_concurrency());
    thread_pool_ = std::make_unique<ThreadPool>(size);
    for(int i=0; i<size; ++i){
        std::unique_ptr<EventLoop> sub_reactor = std::make_unique<EventLoop>();
        sub_reactors_.push_back(std::move(sub_reactor));
    }
   
}

Server::~Server(){}

void Server::Start() {
    for (size_t i = 0; i < sub_reactors_.size(); ++i) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i].get());
    thread_pool_->Add(std::move(sub_loop));
    }
    main_reactor_->Loop();
}

void Server::NewConnection(int fd){
    ErrorIf(fd == -1, "new connection error");
    uint64_t random = fd % sub_reactors_.size();

    std::unique_ptr<Connection> conn = std::make_unique<Connection>(fd, sub_reactors_[random].get());
    std::function<void(int)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    //conn->SetOnConnectCallback(on_connect_callback_);
    conn->SetOnMessageCallback(on_message_callback_);
    connections_[fd] = std::move(conn);
    if(new_connect_callback_) 
        new_connect_callback_(connections_[fd].get());
}

void Server::DeleteConnection(int fd){
    auto it = connections_.find(fd);
    if(it != connections_.end()){
        connections_.erase(fd);
    }
}

void Server::OnConnect(std::function<void(Connection *)> fn){
    on_connect_callback_ = std::move(fn);
}

void Server::OnMessage(std::function<void(Connection *)> fn){
    on_message_callback_ = std::move(fn);
}

void Server::NewConnect(std::function<void(Connection *)> fn){
    new_connect_callback_ = std::move(fn);
}

void Server::CheckActiveConnection(size_t t){
    //LOG_INFO << "Set the runevery";
    main_reactor_->runEvery(t, [this]() {
        //LOG_INFO<< "connection size " <<connections_.size();
        std::vector<int> delete_after;
        for(const auto& kv : connections_){
            //LOG_INFO << "Check!";
           if(connections_.empty())break; //这里必不可少，没太搞懂为什么connections的size已经为0还是会进入循环，不过现在直接强行退出了，没毛病
           if(kv.second){
                //LOG_INFO << "connection is not null!";
                size_t last = kv.second->getContext();
                size_t now = main_reactor_->getNow();
               // LOG_INFO<< std::to_string(now - last);
                if(now - last > 1000 * 60 * 2){ //两分钟
                    LOG_INFO<< "Clear Inactive connection: " << kv.first <<"  after waiting: "<< std::to_string(now - last);
                     kv.second->Send("You are cleared!");
                    //LOG_INFO<< "before delete connection size " <<connections_.size();
                     kv.second->deleteConnection();
                     delete_after.push_back(kv.first);
                     //DeleteConnection(kv.first);
                    //LOG_INFO<< "connection size " <<connections_.size();
                }
            }
        }
        for(int i=0;i<delete_after.size();i++){
            DeleteConnection(delete_after[i]);
        }
    });
}

size_t  Server::getNow(){
        return main_reactor_->getNow();
     }
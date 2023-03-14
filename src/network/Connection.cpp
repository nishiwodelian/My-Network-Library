#include "Connection.h"
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <utility>
#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "util.h"
#include "Logging.h"
#define ASSERT(expr, message) assert((expr) && (message))

Connection::Connection(int fd, EventLoop *loop):context_(0){
    sock_ = std::make_unique<Socket>();
    sock_->set_fd(fd);
    if(loop != nullptr){
        channel_ = std::make_unique<Channel>(loop, fd);
        channel_->SetCloseCallback(std::bind(&Connection::Close, this));
        channel_->EnableRead();
        channel_->UseET();
    }
    read_buffer_ = std::make_unique<Buffer>();
    send_buffer_ = std::make_unique<Buffer>();
    state_ = State::Connected;
}

Connection::~Connection(){}

void Connection::Read(){
     if(state_ != State::Connected)return;
    //assert(state_ == State::Connected);
    read_buffer_->Clear();
    if(sock_->IsNonBlocking()){
        ReadNonBlocking();
    } else{
        ReadBlocking();
    }
}

void Connection::Write() {
    //assert(state_ == State::Connected);
    if(state_ != State::Connected)return;
    if(sock_->IsNonBlocking()){
        WriteNonBlocking();
    }else{
        WriteBlocking();
    }
    send_buffer_->Clear();
}

void Connection::ReadNonBlocking(){
    int sockfd = sock_->GetFd();
    char buf[1024]; //这个buf大小无所谓
    while (true) {  //非阻塞读取，每次读取的大小跟buf大小一样，每次先初始化再读
        memset(buf, 0, sizeof(buf)); //初始化内存
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));//将文件中的数据读入内存，返回的是读取的字节数。read和write函数为Linux系统函数，其缓冲区由用户来维护，即用户指定其大小
        if(bytes_read > 0) {
            read_buffer_->Append(buf, bytes_read);
        } else if(bytes_read == -1 && errno ==EINTR){ //程序正常中断、继续读取
            printf("continue reading\n");
            continue;
        } else if(bytes_read == -1 && ((errno==EAGAIN) || (errno == EWOULDBLOCK))){ // 非阻塞IO，这个条件表示数据全部读取完毕
            break;
        } else if(bytes_read == 0) {//EOF，客户端断开连接
            //printf("read EOF, client fd %d disconnected\n", sockfd);
 //           LOG_ERROR << "read EOF, client fd" << sockfd ;
            state_ = State::Closed;
            Close();
//            LOG_ERROR << "success close" << sockfd ;
            break;
        } else{
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            Close();
            break;
        }
    }
}

void Connection::WriteNonBlocking(){
    int sockfd = sock_->GetFd();
    char buf[send_buffer_->Size()];
    memcpy(buf, send_buffer_->ToStr(), send_buffer_->Size());
    int data_size = send_buffer_->Size();
    int data_left = data_size;
    while(data_left > 0){
        ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
        if(bytes_write == -1 && errno ==EINTR){  // 程序正常中断、继续读取
            printf("continue writing\n");
            continue;
        }
        if(bytes_write == -1 && errno==EAGAIN){  //表明在非阻塞模式下调用了阻塞操作，当前没有数据可读了
            break;
        }
        if(bytes_write == -1){
            printf("Other error on client fd %d\n", sockfd);
            state_ = State::Closed;
            break;
        }
        data_left -= bytes_write;
    }
}

void Connection::ReadBlocking() {
    int sockfd = sock_->GetFd();
    unsigned int rcv_size = 0;
    socklen_t len = sizeof(rcv_size);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,&rcv_size, &len);
    char buf[rcv_size];
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if(bytes_read > 0){
        read_buffer_->Append(buf, bytes_read);
    }else if(bytes_read==0){
        //printf("read EOF, blocking client fd %d disconnected\n", sockfd);
        LOG_ERROR << "read EOF, client fd" << sockfd ;
        state_ = State::Closed;
    }else if(bytes_read == -1){
        printf("Other error on blocking client fd %d\n", sockfd);
        state_ = State::Closed;
    }
}

void Connection::WriteBlocking() {    //没有处理send_buffer_数据大于TCP写缓存区的情况，可能有bug
    int sockfd = sock_->GetFd();
    ssize_t bytes_write = write(sockfd, send_buffer_->ToStr(), send_buffer_->Size());
    if(bytes_write == -1){
        printf("Other error on blocking client fd %d\n", sockfd);
        state_ = State::Closed;
    }
}

void Connection::Send(std::string msg){
    SetSendBuffer(msg.c_str());
    Write();
}

void Connection::Business(){
    Read();
    on_message_callback_(this);
}

void Connection::Close(){
    state_ = State::Closed;
    //channel_->disableAll();
    delete_connection_callback_(sock_->GetFd());

}
Connection::State Connection::GetState(){
    return state_;
}
void Connection::SetSendBuffer(const char *str){
    send_buffer_->SetBuf(str);
}
Buffer *Connection::GetReadBuffer(){
    return read_buffer_.get();
}
const char *Connection::ReadBuffer(){
    return read_buffer_->ToStr();
}
Buffer *Connection::GetSendBuffer(){
    return send_buffer_.get();
}
const char *Connection::SendBuffer(){
    return send_buffer_->ToStr();
}

void Connection::SetDeleteConnectionCallback(std::function<void(int)> const &callback){
    delete_connection_callback_ = callback;
}
void Connection::SetOnConnectCallback(std::function<void(Connection*)> const &callback){
    on_connect_callback_ = callback;
    //channel_->SetReadCallback([this]() {on_connect_callback_(this);});
}
void Connection::SetOnMessageCallback(std::function<void(Connection*)> const &callback){
    on_message_callback_ = callback;
    std::function<void()> bus = std::bind(&Connection::Business, this);
    channel_->SetReadCallback(bus);
}

void Connection::GetlineSendBuffer(){
    send_buffer_->GetLine();
}

Socket *Connection::GetSocket() {
    return sock_.get();
}

void Connection::deleteConnection(){
    state_ = State::Closed;
         //LOG_ERROR << "before disableAll" ;
    channel_->disableAll();
         //LOG_ERROR << "before delete channel" ;
    channel_->Delete();
}

void Connection::shutdown(){
    if(!channel_->isWriting()){
        sock_->shutdownWrite();
    }
}

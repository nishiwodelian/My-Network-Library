#pragma once
#include <functional>
#include <memory>
class EventLoop;
class Socket;
class Channel;
class Buffer;
class Connection{
    public:
     enum State {
        Invalid = 1,
        Handshaking,
        Connected,
        Closed,
        Failed,
     };
     Connection(int fd, EventLoop *loop);
     ~Connection();
     bool isConnected() {return state_ == State::Connected;}
     void Read();
     void Write();
     void Send(std::string msg); //直接向客户端发送字符串
     void Business(); //处理业务

     void SetDeleteConnectionCallback(std::function<void(int)> const &callback);
     void SetOnConnectCallback(std::function<void(Connection *)> const &callback);//处理连接反馈用这个传入函数
     void SetOnMessageCallback(std::function<void(Connection *)> const &callback);//处理业务用这个传入函数

     State GetState();
     void Close();
     void SetSendBuffer(const char *str);
     Buffer *GetReadBuffer();
     const char *ReadBuffer();
     Buffer *GetSendBuffer();
     const char *SendBuffer();
     void GetlineSendBuffer();
     Socket *GetSocket();
     void OnConnect(std::function<void()> fn);

     void setContext(size_t t){
      context_ = t;
     }
     size_t getContext(){
      return context_;
     }
     void deleteConnection();
     void shutdown();

    private:
     std::unique_ptr<Socket> sock_;
     std::unique_ptr<Channel> channel_;
     State state_{State::Invalid};
     std::unique_ptr<Buffer> read_buffer_;
     std::unique_ptr<Buffer> send_buffer_;
     std::function<void(int)> delete_connection_callback_;
     std::function<void(Connection *)> on_connect_callback_;
     std::function<void(Connection *)> on_message_callback_;
     size_t context_;//记录时间来着

     void ReadNonBlocking();
     void WriteNonBlocking();
     void ReadBlocking();
     void WriteBlocking();
};
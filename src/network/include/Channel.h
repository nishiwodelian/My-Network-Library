#pragma once
#include <functional>
class Socket;
class EventLoop;
class Channel
{
    public:
     Channel(EventLoop *_loop, int _fd);
     ~Channel();

     void HandleEvent();
     void EnableRead();

     int GetFd();
     uint32_t GetListenEvents();
     uint32_t GetReadyEvents();
     bool GetInEpoll();
     void SetInEpoll(bool _in = true);
     void UseET();
     void disableAll(); 
      bool isWriting();

     void SetReadyEvents(uint32_t);
     void SetReadCallback(std::function<void()>);
     void SetCloseCallback(std::function<void()>);
     void Delete();

    private:
     EventLoop *loop_;
     int fd_;
     uint32_t listen_events_;
     uint32_t ready_events_;
     bool in_epoll_;
     std::function<void()> read_callback_;
     std::function<void()> write_callback_;
     std::function<void()> close_callback_;
};
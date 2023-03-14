#include "Channel.h"
#include "EventLoop.h"
#include "Socket.h"
#include <unistd.h>
#include <sys/epoll.h>
#include "Logging.h"

Channel::Channel(EventLoop *_loop, int _fd)
 : loop_(_loop), fd_(_fd), listen_events_(0), ready_events_(0),in_epoll_(false){}

Channel::~Channel(){
    if(fd_ != -1){
        close(fd_);
        fd_ = -1;
    }
}

void Channel::disableAll() {
      listen_events_ &= 0;
//      LOG_ERROR << "before update channel";
      loop_->UpdateChannel(this);
}
bool Channel::isWriting(){ return listen_events_ & EPOLLOUT; }


void Channel::HandleEvent(){
    if ((ready_events_ & EPOLLHUP) && !(ready_events_ & EPOLLIN))
    {
        // 确认是否拥有回调函数
        if (close_callback_)
        {
            close_callback_();
        }
    }
    if(ready_events_ & (EPOLLIN | EPOLLPRI)) { //EPOLLIN数据可读，EPOLLPRI高优先级数据可读，包括TCP带外数据
        read_callback_();
    }
    if(ready_events_ & (EPOLLOUT)){//EPOLLOUT数据可写
        write_callback_();
    }
}

void Channel::EnableRead(){
    listen_events_ |= EPOLLIN | EPOLLPRI;
    loop_->UpdateChannel(this);
}
void Channel::UseET(){
    listen_events_ |= EPOLLET;
    loop_->UpdateChannel(this);
}
int Channel::GetFd(){
    return fd_;
}
uint32_t Channel::GetListenEvents(){ //表示希望监听这个文件描述符的哪些事件，因为不同事件的处理方式不同
    return listen_events_;
}
uint32_t Channel::GetReadyEvents(){//表示epoll返回该channel时文件描述符正在发生的事件
    return ready_events_;
}
bool Channel::GetInEpoll(){
    return in_epoll_;
}
void Channel::SetInEpoll(bool _in){
    in_epoll_ = _in;
}
void Channel::SetReadyEvents(uint32_t _ev){
    ready_events_ = _ev;
}
void Channel::SetReadCallback(std::function<void()> _cb){
    read_callback_ = _cb;
}
void Channel::SetCloseCallback(std::function<void()> _cb){
    close_callback_ = _cb;
}

void Channel::Delete(){
    //LOG_ERROR << "before loop remove";
    loop_->RemoveChannel(this);
}
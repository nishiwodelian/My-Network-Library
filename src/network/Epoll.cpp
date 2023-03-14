#include "Epoll.h"
#include "util.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>
#include "Logging.h"

#define MAX_EVENTS 1000

Epoll::Epoll():epfd_(-1),events_(nullptr){
    epfd_ = epoll_create1(0); //如果flags为0，epoll_create1（）和删除了过时size参数的epoll_create（）相同。创建一个新的epoll实例
    ErrorIf(epfd_ == -1, "epoll create error");
    events_ = new epoll_event[MAX_EVENTS]; //结构包含events表示epoll事件，data表示用户数据
    bzero(events_, sizeof(*events_) * MAX_EVENTS);
}
Epoll::~Epoll(){
    if(epfd_ != -1){
        close(epfd_);
        epfd_ = -1;
    }
    delete [] events_;
}

std::vector<Channel*> Epoll::Poll(int timeout){
    std::vector<Channel*> activeChannels;
    int nfds = epoll_wait(epfd_, events_, MAX_EVENTS,timeout);//epoll_wait如果检测到事件，将所有就绪事件从内核事件表拷贝到events_指向的数组中
    ErrorIf(nfds == -1, "epoll wait error"); //返回的nfds是就绪文件描述符的个数
    
    for(int i = 0; i< nfds;++i){
        Channel *ch = (Channel*)events_[i].data.ptr; //指针指向某个文件描述符对应的channel
        ch->SetReadyEvents(events_[i].events); //uint32_t格式的，表示epoll事件
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void Epoll::UpdateChannel(Channel *ch){
    int fd = ch->GetFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.ptr = ch;
    ev.events = ch->GetListenEvents();
    if(!ch->GetInEpoll()){
        ErrorIf(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) ==-1, "epoll add error");//往时间表注册fd上的事件
        ch->SetInEpoll(); //默认参数是true的
    } else{
        ErrorIf(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev)==-1, "epoll modify error");//修改fd上的注册事件
    }
}

void Epoll::DeleteChannel(Channel *ch){
   
    //LOG_ERROR << "before getfd";
    int fd = ch->GetFd();
    //LOG_ERROR << "before epoll_ctl";
    ErrorIf(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL) == -1, "epoll delete error");//删除fd上的注册事件
    //LOG_ERROR << "before set in epoll";
    ch->SetInEpoll(false); //修改channel在不在epoll事件表的标识
}
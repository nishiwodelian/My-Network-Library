#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>


EventLoop::EventLoop() : quit_(false)
{
    epoll_ = std::make_unique<Epoll>();
    timerQueue_ = std::make_unique<TimerQueue>(this);
}
EventLoop::~EventLoop(){
}

void EventLoop::Loop(){
    //std::cout << "start loop"<<std::endl;
    while(!quit_){
        std::vector<Channel*> chs;
        chs = epoll_->Poll();
        //std::cout << "start loop"<<std::endl;
        for(auto it = chs.begin(); it!=chs.end();++it){
            (*it)->HandleEvent();
        }
    }
}

void EventLoop::UpdateChannel(Channel *ch){
    epoll_->UpdateChannel(ch);
}
void EventLoop::RemoveChannel(Channel *ch){
    epoll_->DeleteChannel(ch);
}

void EventLoop::runInLoop(std::function<void()> cb) {
    cb(); //这里直接运行了，应该判断一下是不是当前执行线程tid的
}
#pragma once
#include <functional>
#include <memory>
#include "TimerQueue.h"



class Epoll;
class Channel;
class EventLoop{
    public:
     EventLoop();
     ~EventLoop();

     void Loop();
     void UpdateChannel(Channel*);
     void RemoveChannel(Channel*);

     void runInLoop(std::function<void()> cb); 
     //定时器相关
     void runAt(int timeout, std::function<void()> &&cb){
        timerQueue_->addTimer(std::move(cb), timeout, 0.0);
     }//单位都是毫秒
     void runAfter(int waitTime, std::function<void()> &&cb) {
        //int timeout = timerQueue_->getNow() + waitTime;
        int timeout = waitTime;
        runAt(timeout, std::move(cb));
     }
     void runEvery(int interval, std::function<void()> &&cb) {
        int timeout = interval;
         timerQueue_->addTimer(std::move(cb), timeout, interval);
     }
     size_t getNow() { 
        return timerQueue_->getNow();
     }


    private:
     std::unique_ptr<Epoll> epoll_;
     std::unique_ptr<TimerQueue> timerQueue_;

     bool quit_{false};
};
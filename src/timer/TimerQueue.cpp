#include "EventLoop.h"
#include "Channel.h"
#include "Logging.h"

#include "TimerQueue.h"

#include <sys/timerfd.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

int createTimerfd(){
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        LOG_ERROR << "Failed in timerfd_create";
    }
    return timerfd;
}


TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
      timerfd_(createTimerfd()),
      timerNodeQueue(),
      callingExpiredTimers_(false){
        if(loop_ != nullptr && timerfd_ >= 0){
             channel_ = std::make_unique<Channel>(loop_, timerfd_);
             channel_->SetReadCallback(std::bind(&TimerQueue::handleRead, this));
             channel_->EnableRead();
             channel_->UseET();
        }
    
}

TimerQueue::~TimerQueue(){


}

size_t TimerQueue::getNow() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
}


void TimerQueue::addTimer(TimerCallback cb, int timeout, double interval){
   //  LOG_INFO << " add timer ! timeout is: " << timeout;
    TimerNode* timer = new TimerNode(std::move(cb), timeout, interval);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
}

void TimerQueue::addTimerInLoop(TimerNode* timer){
   // LOG_INFO << " add timer in loop!";
    bool Changed = insert(timer); 
    if(Changed){
        resetTimerfd(timer->expiration());   //如果改变了最早的触发时间，那么就需要重新设置触发时间
    }
    
}

void TimerQueue::resetTimerfd(size_t expiration){
    //  LOG_INFO << "reset timerfd: " <<expiration;
      struct itimerspec newValue;
      struct itimerspec oldValue;
      memset(&newValue, '\0', sizeof(newValue));
      memset(&oldValue, '\0', sizeof(oldValue));
      struct timeval now;
      gettimeofday(&now, NULL);
      size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
      //LOG_INFO << "now: " <<temp;
      size_t dif = expiration - temp; //单位是毫秒
       //LOG_INFO << "dif: " <<dif;
      if( dif < 100) dif = 100;
      struct timespec ts;
      ts.tv_sec = static_cast<time_t>(dif / 1000);
      ts.tv_nsec = static_cast<long>((dif % 1000) * 1000000);
      newValue.it_value = ts;
      if(::timerfd_settime(timerfd_, 0, &newValue, &oldValue)){
        LOG_ERROR << "timerfd_settime failed" ;
      }
}

bool TimerQueue::insert(TimerNode* timer) {
    size_t when = timer->expiration();
    bool Changed = false;
    if(timerNodeQueue.empty()){
        Changed = true;
    }else{
        TimerNode* ptimer_now = timerNodeQueue.top();
        if(when < ptimer_now->expiration())  Changed = true;
    }
    timerNodeQueue.push(timer);   //将新定时器加入队列
    //LOG_INFO << "insert! queue size: " <<timerNodeQueue.size();
    return Changed;
}





void TimerQueue::handleRead() {
    //LOG_INFO << " handleRead !";
    callingExpiredTimers_ = true;
     while(!timerNodeQueue.empty()){
        TimerNode* ptimer_now = timerNodeQueue.top();
        if(ptimer_now->isDeleted()){
            timerNodeQueue.pop();
        }
        else if(ptimer_now->isValid() == false){
            ptimer_now->run();
            timerNodeQueue.pop();
            if(ptimer_now->repeat()){
                // << " before restart:  " << ptimer_now->expiration();
                ptimer_now->restart();
                //LOG_INFO << " after restart:  " << ptimer_now->expiration();
                insert(ptimer_now);
                //LOG_INFO << " isvalid:  " << ptimer_now->isValid();
                //LOG_INFO << " isdeleted:  " << ptimer_now->isDeleted();
            }else{
                delete ptimer_now;
            }
            }
        else{
            break;
        }
    }
    // LOG_INFO << "insert! queue size: " <<timerNodeQueue.size();
    callingExpiredTimers_ = false;
    if(!timerNodeQueue.empty()){
        resetTimerfd(timerNodeQueue.top()->expiration());
    }
}

void TimerQueue::handleExpiredEvent() {
    while(!timerNodeQueue.empty()){
        TimerNode* ptimer_now = timerNodeQueue.top();
        if(ptimer_now->isDeleted())
            timerNodeQueue.pop();
        else if(ptimer_now->isValid() == false)
            timerNodeQueue.pop();
        else
            break;
    }
}
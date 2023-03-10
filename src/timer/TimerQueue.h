#ifndef FILENAME_H
#define FILENAME_H

#include "Channel.h"
#include <queue>
#include <memory>
#include <vector>
#include "TimerNode.h"


class EventLoop;
class TimerNode;

struct TimerCmp {
  bool operator()(TimerNode *a, TimerNode *b) const {
    return a->expiration() > b->expiration();
  }
};
/*
struct TimerCmp {
  bool operator()(std::shared_ptr<TimerNode> &a,
                  std::shared_ptr<TimerNode> &b) const {
    return a->expiration() > b->expiration();
  }
};*/

class TimerQueue{
    public:
     using TimerCallback = std::function<void()>;
     explicit TimerQueue(EventLoop* loop);
     ~TimerQueue();

     void addTimer(TimerCallback cb,
                  int timeout,
                  double interval); //插入定时器，注意回调函数是这里存在timernode的，同时还有到期时间，是否重复
     void handleExpiredEvent(); //在loop循环中，处理到期定时器，到期的给删掉
     size_t getNow();

     private:
      void addTimerInLoop(TimerNode* timer); //在loop中添加定时器
      void handleRead();  //这个是真正处理定时器回调函数的，这是挂给timerchannel的回调函数
      void resetTimerfd( size_t expiration);//重置timerfd
      bool insert(TimerNode* timer); //在队列中插入定时器，如果最早到达时间改变了，则返回true


      EventLoop* loop_;
      const int timerfd_;         // timerfd是Linux提供的定时器接口
      //Channel timerfdChannel_;     
      //using SPTimerNode = std::shared_ptr<TimerNode>;
      //std::priority_queue<SPTimerNode, std::deque<SPTimerNode>, TimerCmp> timerNodeQueue;
      std::priority_queue<TimerNode*, std::vector<TimerNode*>, TimerCmp> timerNodeQueue;
      bool callingExpiredTimers_;
      std::unique_ptr<Channel> channel_;
      
};

#endif /* FILENAME_H */
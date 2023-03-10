#include "TimerNode.h"
#include <sys/time.h>

TimerNode::TimerNode(TimerCallback cb, int timeout, double interval)
    : callback_(std::move(cb)),
      interval_(interval),
      repeat_(interval > 0.0),
      deleted_(false) {
    struct timeval now;
    gettimeofday(&now, NULL);
    expiration_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec /1000)) + timeout; //毫秒
}



void TimerNode::update(int timeout){
    struct timeval now;
  gettimeofday(&now, NULL);
  expiration_ =
      (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid() {
  struct timeval now;
  gettimeofday(&now, NULL);
  size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
  if (temp < expiration_)
    return true;
  else {
    this->setDeleted();
    return false;
  }
}

void TimerNode::restart(){
    if(repeat_){ //如果是重复任务，那么加interval以后重新加入队列
        expiration_ += interval_;
        deleted_ = false;
    }else{  //不是的话就把这个时间点设置成触发时间
        struct timeval now;
        gettimeofday(&now, NULL);
        size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
        expiration_ = temp;
    }
}


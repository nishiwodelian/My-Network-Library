#pragma once
#include <sys/epoll.h>
#include <vector>
#include <mutex>


class Channel;
class Epoll{
    public:
     Epoll();
     ~Epoll();

     int test(){return 1234;}
     void UpdateChannel(Channel*);
     void DeleteChannel(Channel*);
     std::vector<Channel*> Poll(int timeout = -1);
    private:
     int epfd_{-1};
     std::mutex mutex_;
     struct epoll_event *events_{nullptr};
};

#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll{
    public:
     Epoll();
     ~Epoll();

     void UpdateChannel(Channel*);
     void DeleteChannel(Channel*);
     std::vector<Channel*> Poll(int timeout = -1);
    private:
     int epfd_{-1};
     struct epoll_event *events_{nullptr};
};

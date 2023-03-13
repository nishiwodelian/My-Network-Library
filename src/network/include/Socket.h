#pragma once
#include <arpa/inet.h>

class InetAddress{
    public:
     InetAddress();
     InetAddress(const char *ip, uint16_t port);
     ~InetAddress() = default;

     void SetInetAddr(sockaddr_in _addr);
     sockaddr_in GetAddr();
     char* GetIp();
     uint16_t GetPort();

    private:
     struct sockaddr_in addr_;  //IPv4专用socket地址结构体
};

class Socket{
    public:
     Socket();
     Socket(int _fd);
     ~Socket();
     void set_fd(int fd) { fd_ = fd; }

     void Bind(InetAddress*);
     void Listen();
     int Accept(InetAddress*);
     void Connect(InetAddress*);
     void Connect(const char *ip, uint16_t port);
     int SetNonBlocking();
     bool IsNonBlocking();
     int GetFd();
     void shutdownWrite();
    private:
     int fd_{-1};
};
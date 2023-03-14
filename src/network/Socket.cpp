#include "Socket.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

Socket::Socket() : fd_(-1){
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    ErrorIf(fd_ == -1, "socket create error");
}
Socket::Socket(int _fd) : fd_(_fd){
    ErrorIf(fd_ == -1, "socket create error");
}
Socket::~Socket(){
    if(fd_ != -1){
        close(fd_);
        fd_ = -1;
    }
}
void Socket::Bind(InetAddress *_addr){
    struct sockaddr_in addr = _addr->GetAddr();
    ErrorIf(::bind(fd_, (sockaddr*)&addr, sizeof(addr)) == -1, "socket bind error"); //绑定地址给socket
}
void Socket::Listen(){
    ErrorIf(::listen(fd_, SOMAXCONN)==-1, "socket listen error");//SOMAXCONN是系统建议的监听队列最大长度，被定义为128
}
int Socket::SetNonBlocking(){
    int old_option = fcntl(fd_, F_GETFL);//获取文件描述符旧的状态标志
    fcntl(fd_, F_SETFL,  old_option| O_NONBLOCK); //设置非阻塞标志
    return old_option; //返回旧的，以便以后恢复
}

bool Socket::IsNonBlocking(){
    return (fcntl(fd_, F_GETFL)& O_NONBLOCK)!=0; //获取状态描述符和阻塞符
}

int Socket::Accept(InetAddress *_addr){
    int clnt_sockfd = -1;
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    socklen_t addr_len = sizeof(addr); //accept函数第三个参数要求这个变量类型
    if(fcntl(fd_, F_GETFL) & O_NONBLOCK){//非阻塞
        while(true){
            clnt_sockfd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
            if(clnt_sockfd == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){ //EAGAIN是当前不可读，只要继续重试就好
                //printf("no connection yet\n");
                continue;//非阻塞模式做I/O操作的细心的人会检查errno是不是EAGAIN、EWOULDBLOCK、EINTR，如果是就应该重读，一般是用循环
            }else if(clnt_sockfd == -1){
                ErrorIf(true, "socket accept error");
            }else{
                //printf("connected!!\n");
                break;
            }
        }
    }
    else{
        //printf("waiting connect...\n");
        clnt_sockfd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
        ErrorIf(clnt_sockfd == -1, "socket accept error");
    }
    _addr->SetInetAddr(addr);
    return clnt_sockfd;
}
void Socket::Connect(InetAddress *_addr){
    struct sockaddr_in addr = _addr->GetAddr();
    if(fcntl(fd_, F_GETFL) & O_NONBLOCK){ //非阻塞
        while(true){
            int ret = ::connect(fd_, (sockaddr*)&addr, sizeof(addr));
            if(ret == 0){
                break;
            }else if(ret == -1 && (errno == EINPROGRESS)){//如果 errno == EINPROGRESS，表示连接还在进行中
                continue;  //直到连接上为止，即阻塞式，这里先写成阻塞的了
            }
            else if(ret == -1){
                ErrorIf(true, "socket connect error");
            }
        }
    }
    else{
        ErrorIf(::connect(fd_, (sockaddr*)&addr, sizeof(addr))==-1, "socket connect error");
    }
}

void Socket::Connect(const char *ip, uint16_t port) {
  InetAddress *addr = new InetAddress(ip, port);
  Connect(addr);
  delete addr;
}

int Socket::GetFd(){
    return fd_;
}

void Socket::shutdownWrite(){
    //close(fd_);
     ErrorIf(shutdown(fd_, SHUT_WR) == -1, "socket accept error");
     //ErrorIf(shutdown(fd_, SHUT_RD) == -1, "socket accept error");
}

InetAddress::InetAddress(){
    bzero(&addr_, sizeof(addr_));
}
InetAddress::InetAddress(const char* _ip, uint16_t _port){
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;//地址族S
    addr_.sin_addr.s_addr = inet_addr(_ip); //将用点分十进制字符串表示的IPv4地址转化为网络字节序整数表示的IPv4地址
    addr_.sin_port = htons(_port); //端口号，要求网络字节序，htons把主机字节序转换为网络字节序
}

void InetAddress::SetInetAddr(sockaddr_in _addr){
    addr_ = _addr;
}
sockaddr_in InetAddress::GetAddr(){
    return addr_;
}
char* InetAddress::GetIp(){
    return inet_ntoa(addr_.sin_addr);//网络字节序再转回点分十进制字符串表示的IPv4地址
}
uint16_t InetAddress::GetPort(){
    return ntohs(addr_.sin_port);
}


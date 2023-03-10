## My-Network-Library

### 项目介绍

本项目是基于Reactor模型实现的多线程网络库。内部实现了小型HTTP服务器，支持GET请求和静态资源访问，实现了异步日志记录服务器运行状态。内部实现了基于跳表实现的轻量级键值存储引擎，支持数据插入、删除、查询、显示、落盘以及文件加载等，并基于网络库实现了存储引擎服务器。

项目目前实现了包含Channel，Epoll，Eventloop等的网络模块，异步日志模块，使用priority_queue小根堆实现的定时器模块，使用跳表实现的存储模块，以及HTTP模块。



### 项目特点

- 底层使用Epoll边沿触发的IO多路复用，结合非阻塞I/O实现主从Reactor模型
- 采用 one loop per thread 线程模型，封装线程池避免线程频繁创建销毁的开销
- 基于双缓冲区实现异步日志，由后端线程负责定时向磁盘写入前端日志信息，避免数据落盘阻塞网络服务
- 基于小根堆实现定时器，内部使用Linux的timerfd通知到期服务，基于定时器实现关闭超时请求
- 遵循RAII手法使用智能指针管理内存，减少内存泄漏
- 利用有限状态机解析HTTP请求报文
- 基于跳表实现键值存储引擎，提供了存储引擎客户端和服务端

### 开发环境

- 操作系统：Ubuntu 20.04.5 LTS
- 编译器：g++ 9.4.0

### 项目构建

安装Cmake

```
sudo apt-get update
sudo apt-get install cmake
```

构建项目

```
mkdir build && cd build
cmake ..
make
```

### 运行案例

```
cd ./example
```

#### ./Client

客户端，默认端口1234

#### ./EcoServer

回声服务器，默认端口1234

#### ./HTTPServer

http服务器，默认端口1234

#### ./KVServer

存储引擎服务器，默认端口1234

#### ./TimerServer

定时处理不活跃连接的回声服务器，默认端口1234



### 代码统计

![代码统计](https://github.com/nishiwodelian/My-Network-Library/blob/master/%E4%BB%A3%E7%A0%81%E7%BB%9F%E8%AE%A1.png)



### 优化计划

- 实现内存池模块管理小块内存空间

- 支持解析POST请求，实现用户注册、登录功能，支持请求图片和视频文件

- 计划加上一致性协议，如raft构成分布式存储，使用网络库构建http server对外提供分布式存储服务

  

  

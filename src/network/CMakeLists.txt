cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(LIB_NETWORK
    Acceptor.cpp
    Buffer.cpp
    Channel.cpp
    Connection.cpp
    Epoll.cpp
    EventLoop.cpp
    Server.cpp
    Socket.cpp
    ThreadPool.cpp 
    util.cpp
    ../timer/TimerQueue.cpp
    ../timer/TimerNode.cpp
    ../logger/Logging.cpp
    ../logger/LogStream.cpp
)

include_directories(./include)
include_directories(../timer)
include_directories(../logger)
include_directories(../base)
add_library(network ${LIB_NETWORK})
target_compile_options(network PUBLIC -g)
#add_subdirectory(test)

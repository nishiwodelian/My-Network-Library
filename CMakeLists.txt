cmake_minimum_required(VERSION 3.10) #声明使用CMAKE的最低版本要求

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)#开启后，其生成的文件compile_commands.json，包含所有编译单元所执行的指令
set(BUILD_SHARED_LIBS ON)#这个开关用来控制默认的库编译方式
#如果不进行设置，使用add_library又没有指定库类型的情况下，默认编译生成的库都是静态库。如果set(BUILD_SHARED_LIBS ON)后，默认生成的为动态库
set(CMAKE_CXX_STANDARD 17)#设置C++版本
set(CMAKE_CXX_STANDARD_REQUIRED ON)#设置指定的C++编译器版本，如果不设置或者为OFF，则指定版本不可用时，会使用上一版本。

project(MyWebServer VERSION 0.1) #声明名字


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
#set(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
#set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")



add_subdirectory(src/network)
add_subdirectory(src/logger)
add_subdirectory(src/http)
add_subdirectory(src/storage)
add_subdirectory(example)



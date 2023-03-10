cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
include_directories(./)
include_directories(../network/include)
add_library(storage SkipListWorker.cpp)
#add_subdirectory(test)

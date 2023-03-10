cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(LIB_LOGGER
    AsyncLogging.cpp
    CurrentThread.cpp
    FileUtil.cpp
    LogFile.cpp
    Logging.cpp
    LogStream.cpp
    Thread.cpp
)
#SET(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
include_directories(./)
include_directories(../base)
add_library(logger ${LIB_LOGGER})
#add_subdirectory(test)

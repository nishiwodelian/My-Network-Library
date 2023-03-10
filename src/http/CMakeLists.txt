cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(LIB_HTTP
    HttpContext.cpp
    HttpResponse.cpp
    HttpServer.cpp
    ../network/Buffer.cpp
    ../logger/Logging.cpp
    ../logger/LogStream.cpp
)
#SET(CMAKE_BUILD_TYPE "Debug")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
include_directories(./)
include_directories(../base/)
include_directories(../network/include)
include_directories(../logger)
add_library(http ${LIB_HTTP})
#add_subdirectory(test)
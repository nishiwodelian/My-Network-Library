set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example)
include_directories(../src/base)
include_directories(../src/http)
include_directories(../src/logger)
include_directories(../src/storage)
include_directories(../src/timer)
include_directories(../src/network/include)

add_executable(KVServer kv_server.cpp)
target_link_libraries(KVServer storage)
target_link_libraries(KVServer network)

add_executable(HTTPServer http_server.cpp ../src/network/EventLoop.cpp)
target_link_libraries(HTTPServer network)
target_link_libraries(HTTPServer logger)
target_link_libraries(HTTPServer http)

add_executable(EcoServer server.cpp)
target_link_libraries(EcoServer network)

add_executable(Client client.cpp)
target_link_libraries(Client network)

add_executable(TimerServer timer_server.cpp)
target_link_libraries(TimerServer network)
target_link_libraries(TimerServer logger)



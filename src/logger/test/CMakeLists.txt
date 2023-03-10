
add_executable(LogTest LogTest.cpp)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
target_link_libraries(LogTest logger)
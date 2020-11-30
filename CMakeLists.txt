cmake_minimum_required(VERSION 3.17)
project(cnt5106_v4)

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(PANIC_BACKTRACE_FRAME_N=20)
add_compile_definitions(ACCEPT_QUEUE_SIZE=20)

add_executable(cnt5106_v4 main.cpp)

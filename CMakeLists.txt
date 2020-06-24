cmake_minimum_required(VERSION 3.16)
project(uvmdp)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(uvmdp include/uvmdp/bus.hpp include/uvmdp/device.hpp src/test.cpp include/uvmdp/memory.hpp include/cexpr_utils.hpp)
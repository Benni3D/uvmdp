cmake_minimum_required(VERSION 3.16)
project(uvmdp)

set(CMAKE_CXX_STANDARD 20)
include_directories(include)

add_executable(uvmdp main.cpp)
cmake_minimum_required(VERSION 3.24)
project(is)

set(CMAKE_CXX_STANDARD 17)

add_executable(is src/main.cpp src/utils/array.h src/utils/common.cpp src/utils/common.h)

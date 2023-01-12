cmake_minimum_required(VERSION 3.24)
project(is)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-IS_DEBUG_MODE)

add_executable(is src/main.cpp src/utils/array.h src/utils/common.cpp src/utils/common.h src/utils/options.h src/utils/options.h src/utils/bytecode.cpp src/utils/bytecode.h)


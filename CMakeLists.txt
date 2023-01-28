cmake_minimum_required(VERSION 3.24)
project(is)

set(CMAKE_CXX_STANDARD 17)

add_executable(is src/main.cpp src/container/iarray.h src/utils/common.cpp src/utils/common.h src/utils/ioptions.h src/utils/ioptions.h src/basic/ibytecode.cpp src/basic/ibytecode.h src/lexer/ilexer.cpp src/lexer/ilexer.h src/lexer/ilexer.h src/lexer/itoken.cpp src/lexer/itoken.h src/lexer/itoken.h src/lexer/itoken.h src/string/istring.cpp src/string/istring.h src/basic/ibasic.h src/basic/ibasic.h src/report/ireport.cpp src/report/ireport.h src/report/ireport.h src/basic/ibasic.cpp src/vm/ivm.cpp src/vm/ivm.h src/compiler/icompiler.cpp src/compiler/icompiler.h src/container/imodifier.h src/basic/instream.cpp src/basic/instream.h src/container/istack.h src/container/ihashMap.h)


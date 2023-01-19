cmake_minimum_required(VERSION 3.24)
project(is)

set(CMAKE_CXX_STANDARD 17)

add_executable(is src/main.cpp src/utils/iarray.h src/utils/common.cpp src/utils/common.h src/utils/ioptions.h src/utils/ioptions.h src/utils/ibytecode.cpp src/utils/ibytecode.h src/lexer/ilexer.cpp src/lexer/ilexer.h src/lexer/ilexer.h src/lexer/itoken.cpp src/lexer/itoken.h src/lexer/itoken.h src/lexer/itoken.h src/utils/istring.cpp src/utils/istring.h src/utils/ibasic.h src/utils/ibasic.h src/utils/ireport.cpp src/utils/ireport.h src/utils/ireport.h src/utils/ibasic.cpp src/vm/ivm.cpp src/vm/ivm.h src/compiler/icompiler.cpp src/compiler/icompiler.h src/utils/imodifier.h src/utils/instream.cpp src/utils/instream.h)


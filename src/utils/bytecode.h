#ifndef IS_BYTECODE_H
#define IS_BYTECODE_H

#include "array.h"

//变长字节码参数的写入
void writeArg(Instream &in, uint64_t arg);

//变长字节码参数的读取
uint64_t readArg(Instream &in, isize idx);


#endif //IS_BYTECODE_H

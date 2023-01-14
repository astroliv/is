#ifndef IS_IBYTECODE_H
#define IS_IBYTECODE_H

#include "iarray.h"

//变长字节码参数的写入
void writeArg(Instream &in, uint64_t arg);

//变长字节码参数的读取
uint64_t readArg(Instream &in, isize idx);


#endif //IS_IBYTECODE_H

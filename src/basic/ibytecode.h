//这里将放着字节码的实现,以后大概会搬到basic文件里面~
#ifndef IS_IBYTECODE_H
#define IS_IBYTECODE_H

#include "../utils/common.h"

enum class Bytecode : byte {
	#define loadEnum(n, effect, opNum, ...) n,
	#include "bytecode.enum"
	#undef loadEnum
};

struct BytecodeInfo {
	const char *strName{nullptr};       //该字节码名称的字符串形式
	int8_t effect{0};                   //对栈空间的影响量
	int8_t opNum{0};                    //参数个数
	int8_t fopLen[2]{0};                //各个参数的字节数
	//由于我设计的字节码的参数最多才只有两个,于是就给fopLen数组宽度设为2
};

extern BytecodeInfo bytecodeInfo[];

#endif //IS_IBYTECODE_H

#include "ibytecode.h"

BytecodeInfo bytecodeInfo[] = {
		#define loadEnum(n, effect, opNum, ...) {#n, effect, opNum, {__VA_ARGS__}},
		#include "../enum/bytecode.enum"
		#undef loadEnum
};

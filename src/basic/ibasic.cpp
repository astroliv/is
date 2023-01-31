#include "ibasic.h"

bool Value::operator==(const Value &other) const {
	return i64 == other.i64;
}

BytecodeInfo BytecodeInfoList[] = {
		#define loadEnum(n, effect, opNum, ...) {#n, effect, opNum, {__VA_ARGS__}},
		#include "bytecode.enum"
		#undef loadEnum
};

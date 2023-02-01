#include "ibasic.h"

bool Value::operator==(const Value &other) const {
	return i64 == other.i64;
}

Value::Value(double _f64) : f64(_f64) {}

Value::Value(const string &str) : sp(new string(str)) {}

Value::Value(const refString &str) : fsp(new refString(str)) {}

Value::Value(byte *_ip8) : ip8(_ip8) {}


BytecodeInfo BytecodeInfoList[] = {
		#define loadEnum(n, effect, opNum, ...) {#n, effect, opNum, {__VA_ARGS__}},
		#include "bytecode.enum"
		#undef loadEnum
};

#include "ibasic.h"

bool Value::operator==(const Value &other) const {
	return i64 == other.i64;
}

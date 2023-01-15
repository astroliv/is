//这里定义一些实现用于VM的基础构建
#ifndef IS_BASIC_H
#define IS_BASIC_H

#include "istring.h"

union Value {
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int64_t i64;
	uint8_t ui8;
	uint16_t ui16;
	uint32_t ui32;
	uint64_t ui64;
	float f32;
	double f64;
	string *str;
	refString *fstr;
};

#endif //IS_BASIC_H

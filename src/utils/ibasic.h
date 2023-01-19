//这里定义一些用于实现VM的基础构件
#ifndef IS_IBASIC_H
#define IS_IBASIC_H

#include "istring.h"
#include "iarray.h"
#include "imodifier.h"

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

	bool operator==(const Value &other) const;
};

enum class ValueType : uint8_t {
	unk, num, str, obj
};

enum class VarModifier : uint8_t {
	unk, ptr, cite, mut
};

//primitive - 原生(C/C++)函数指针
//其函数签名: Value fn(Value* args);
using prim = Value(*)(Value *);

union Fn {
	prim prim{};        //可能是原生函数(primitive)
	ByteStream *nat;    //可能是本地函数(native),即指令流
};

class Signature {
public:

};

class FnSignature {
public:

};

class FnInfo {
public:

};

//单个文件被编译后会变成一个模块,每个模块都是个数据单元,该数据类型仅起到存数据的作用
class Module {
public:
	string file;                 //该模块对应的文件
	Array<Fn> fnList;            //模块内的所有函数
	Array<Value> constList;      //模块内的所有的字面量常量
	Array<Value> gvarList;       //模块内全局变量列表
};


#endif //IS_IBASIC_H

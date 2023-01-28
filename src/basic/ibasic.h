//这里定义一些用于实现VM的基础构件
#ifndef IS_IBASIC_H
#define IS_IBASIC_H

#include "../string/istring.h"
#include "../container/iarray.h"
#include "../container/imodifier.h"

//语言的值都将以Value的形式存于内存中,该类型占8Byte
union Value {
	int64_t i64;    //64bit int
	uint64_t ui64;  //64bit unsigned int
	double f64;     //64bit float
	string *sp;     //str ptr
	refString *fsp; //refstr ptr
	Value *vp;      //value ptr
	byte *ip8;      //8bit int ptr


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
using prim = Value (*)(Value *);

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

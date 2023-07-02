//这里定义一些用于实现VM的基础构件
#ifndef IS_IBASIC_H
#define IS_IBASIC_H

#include "../string/istring.h"
#include "../container/iarray.h"
#include "../container/imodifier.h"

enum class ValueType : uint8_t {
	unk, num, str, obj
};

enum class VarModifier : uint8_t {
	unk, ptr, cite, mut
};

//语言的值都将以Value的形式存于内存中,该类型占8Byte
union Value {
	int64_t i64{};  //64bit int
	uint64_t ui64;  //64bit unsigned int
	double f64;     //64bit float
	string *sp;     //str ptr
	refString *fsp; //refstr ptr
	Value *vp;      //value ptr
	byte *ip8;      //8bit int ptr

	Value() = default;
	explicit Value(double _f64);
	explicit Value(const string &str);
	explicit Value(const refString &str);
	explicit Value(byte *_ip8);


	bool operator==(const Value &other) const;
};

//primitive - 原生(C/C++)函数指针
//其函数签名: Value fn(Value* args);
using Prim = Value (*)(Value *);

union Fn {
	Prim prim{};        //可能是原生函数(primitive)
	byte *nat;          //可能是本地函数(native),即指令流
};

//下面是字节码相关的定义

enum class Instruction : byte {
	#define loadEnum(n, effect, opNum, ...) n,
	#include "instruction.enum"
	#undef loadEnum
};

struct InstructionInfo {
	const char *strName{nullptr};       //该字节码名称的字符串形式
	int8_t effect{0};                   //对栈空间的影响量
	int8_t opNum{0};                    //参数个数
	int8_t fopLen[2]{0};                //各个参数的字节数
	//由于我设计的字节码的参数最多才只有两个,于是就给fopLen数组宽度设为2
};

extern InstructionInfo InstructionInfoList[];

inline InstructionInfo instructionInfo(Instruction by) {
	return InstructionInfoList[(isize) by];
}


#endif //IS_IBASIC_H

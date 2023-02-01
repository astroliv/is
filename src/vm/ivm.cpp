#include "ivm.h"
#include <cmath>

//寄存器的定义
//instruction base,ptr;stack base,ptr,upper;
#define ib (regist[Regist::ib].ip8) //指令寄存器:储存指令内存空间起始的指针
#define ip (regist[Regist::ip].ip8) //指令寄存器:储存下一条指令头的指针
#define sb (regist[Regist::sb].vp)  //栈底寄存器:指向栈第一个可使用slot的指针
#define sp (regist[Regist::sp].vp)  //栈顶寄存器:指向栈第一个未使用slot的指针
#define su (regist[Regist::su].vp)  //栈头寄存器:指向栈第一个不可用slot的指针
#define ax (regist[Regist::ax])     //通用寄存器
#define bx (regist[Regist::bx])     //通用寄存器
#define cx (regist[Regist::cx])     //通用寄存器
#define dx (regist[Regist::dx])     //通用寄存器
#define ex (regist[Regist::ex])     //通用寄存器
#define fx (regist[Regist::fx])     //通用寄存器
#define gx (regist[Regist::gx])     //通用寄存器

void VM::execute() {
	Value rigValue, lefValue;
	while (true) {
		switch (read()) {
			case Bytecode::neg:
				sp[-1].f64 = -(sp[-1].f64);
				break;
			case Bytecode::add:
				sp[-2].f64 = sp[-2].f64 + sp[-1].f64, --sp;
//				push(Value(pop().f64 + pop().f64));
				break;
			case Bytecode::sub: {
				sp[-2].f64 = sp[-2].f64 - sp[-1].f64, --sp;
//				Value rValue = pop();
//				push(Value(-pop().f64 + rValue.f64));
				break;
			}
			case Bytecode::mul:
				sp[-2].f64 = sp[-2].f64 * sp[-1].f64, --sp;
//				push(Value(pop().f64 * pop().f64));
				break;
			case Bytecode::div: {
				sp[-2].f64 = sp[-2].f64 / sp[-1].f64, --sp;
//				Value rValue = pop();
//				push(Value(pop().f64 / rValue.f64));
				break;
			}
			case Bytecode::pow: {
				sp[-2].f64 = pow(sp[-2].f64, sp[-1].f64), --sp;
//				Value rValue = pop();
//				push(Value(pow(pop().f64, rValue.f64)));
				break;
			}
			case Bytecode::ldc:
				push(constList[read(bytecodeInfo(Bytecode::ldc).fopLen[0])]);
				break;
			case Bytecode::end:
				return;
			default:
				unreachableBranch();
		}

	}
}


inline Bytecode VM::read() {
	return (Bytecode) *ip++;
}

inline int64_t VM::read(int8_t len) {
	byte *oldip = ip;
	switch (len) {
		case -1:
			return (int64_t) readVarg();
		case 1:
			return ip += len, *(int8_t *) oldip;
		case 2:
			return ip += len, *(int16_t *) oldip;
		case 4:
			return ip += len, *(int32_t *) oldip;
		case 8:
			return ip += len, *(int64_t *) oldip;
		default:
			unreachableBranch();
	}
	return 0;
}

inline uint64_t VM::readVarg() {
	uint64_t result = 0;
	uint8_t i = 0;
	do {
		result |= (i < 8 ? *ip & 0x7F : *ip) << (i * 7);
	} while ((*ip++ >> 7) && (i++ < 8));
	return result;
}

string VM::dumpin() {
	byte *dip = ip;
	uint64_t args[2] = {0};
	char buffer[64] = {0};
	Bytecode by = read();
	const char *fmt = nullptr;
	switch (bytecodeInfo(by).opNum) {
		case 0://无参字节码
			fmt = "%-6llu: %s";
			break;
		case 1://单参字节码
			fmt = "%-6llu: %s %llu";
			args[0] = read(bytecodeInfo(by).fopLen[0]);
			break;
		case 2://双参字节码
			fmt = "%-6llu: %s %llu,%llu";
			args[0] = read(bytecodeInfo(by).fopLen[0]);
			args[1] = read(bytecodeInfo(by).fopLen[1]);
			break;
		default:
			unreachableBranch();
			break;
	}
	sprintf(buffer, fmt, dip - ib, bytecodeInfo(by).strName, args[0], args[1]);
	return string(buffer);
}

void VM::resize(isize newCap) {
	if (newCap == 0) {//清空的情况
		delete[] sb;
		sb = sp = su = nullptr;
	} else if (sb == nullptr) {//初始化的情况
		sp = sb = new Value[newCap];
		su = sb + newCap;
	}//重设容量
	auto *newBp = new Value[newCap];
	isize valid = MIN(usedSize(), newCap);//重设容量后的有效成员数
	for (isize i = 0; i < valid; ++i) { newBp[i] = sb[i]; }//复制内容
	delete[] sb;
	sb = newBp;
	sp = sb + valid;
	su = sb + newCap;
}

inline void VM::ensure(isize remain) {
	if (su - sp >= remain) { return; }
	resize(ceilToPowerOf2(usedSize() + remain));
}

void VM::push(Value value) {
	if (sp == su) {
		resize(ceilToPowerOf2(capacity() + 1));
		reportMsg(RepId::stackResized, nullptr, capacity());
	}
	*sp++ = value;
//	printf("psh:sp[-1].f64=%f\n", sp[-1].f64);
}

Value VM::pop() {
	assert(sp != sb, "The stack has no member!");
//	printf("pop:sp[-1].f64=%f\n", sp[-1].f64);
	return *--sp;
}

inline isize VM::capacity() {
	return su - sb;
}

inline isize VM::usedSize() {
	return sp - sb;
}





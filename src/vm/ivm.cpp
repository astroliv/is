#include "ivm.h"

//寄存器的定义
#define ip (regist[Regist::ip].ip8) //指令寄存器(储存下一条指令头的指针)
#define bp (reg[Regist::bp].vp)  //栈底寄存器(是个指向栈第一个normalSlot的指针)
#define sp (reg[Regist::sp].vp)  //栈顶寄存器(是个指向栈第一个emptySlot的指针)
#define up (reg[Regist::up].vp)  //栈头寄存器(是个指向栈第一个outSlot的指针,该位置不能使用)
#define ax (reg[Regist::ax])     //通用寄存器
#define bx (reg[Regist::bx])     //通用寄存器
#define cx (reg[Regist::cx])     //通用寄存器
#define dx (reg[Regist::dx])     //通用寄存器

void VM::execute() {

}


void VM::write(Bytecode type) {
	instream.append((byte) type);
}

void VM::write(int64_t operand, int8_t len) {
	switch (len) {//直接穷举,没必要整循环
		case -1:
			writeVarg(operand);
			return;
		case 1:
			instream.append(operand);
			return;
		case 2:
			instream.append(operand);
			instream.append(operand >> 8);
			return;
		case 4:
			instream.append(operand);
			instream.append(operand >> 8);
			instream.append(operand >> 16);
			instream.append(operand >> 24);
			return;
		case 8:
			instream.append(operand);
			instream.append(operand >> 8);
			instream.append(operand >> 16);
			instream.append(operand >> 24);
			instream.append(operand >> 32);
			instream.append(operand >> 40);
			instream.append(operand >> 48);
			instream.append(operand >> 56);
			return;
		default:
			unreachableBranch();
	}
}

void VM::write(Bytecode type, int64_t operand, int8_t len) {
	write(type);
	write(operand, len);
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

inline void VM::writeVarg(uint64_t arg) {
	uint8_t i = 0;
	do {
		instream.append((arg >> 7) && (i < 8) ? arg & 0x7F | 0x80 : arg);
	} while ((arg >>= 7) && (i++ < 8));
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
	uint64_t args[2] = {0};           //字节码的参数
	char buffer[64] = {0};            //创建缓冲区
	Bytecode by = read();             //当前字节码
	const char *fmt = nullptr;        //格式化文本
	switch (bytecodeInfo(by).opNum) { //根据参数个数分情况设定
		case 0://无参字节码
			fmt = "%-10llu: %s";
			break;
		case 1://单参字节码
			fmt = "%-10llu: %s %llu";
			args[0] = read(bytecodeInfo(by).fopLen[0]);
			break;
		case 2://双参字节码
			fmt = "%-10llu: %s %llu,%llu";
			args[0] = read(bytecodeInfo(by).fopLen[0]);//读第一个参数
			args[1] = read(bytecodeInfo(by).fopLen[1]);//读第二个参数
			break;
		default:
			unreachableBranch();
			break;
	}
	sprintf(buffer, fmt, ip, bytecodeInfo(by).strName, args[0], args[1]);
	return string(buffer);
}





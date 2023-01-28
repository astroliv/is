#include "ivm.h"

//寄存器的定义
#define ip (reg[0].ip8) //指令寄存器(储存下一条指令头的指针)
#define bp (reg[1].vp)  //栈底寄存器(是个指向栈第一个normalSlot的指针)
#define sp (reg[2].vp)  //栈顶寄存器(是个指向栈第一个emptySlot的指针)
#define up (reg[3].vp)  //栈头寄存器(是个指向栈第一个outSlot的指针,该位置不能使用)
#define ax (reg[4])     //通用寄存器
#define bx (reg[5])     //通用寄存器
#define cx (reg[6])     //通用寄存器
#define dx (reg[7])     //通用寄存器

VM::VM(ByteStream *bst) {
	in.byteStream = bst;
}

void VM::execute() {

}

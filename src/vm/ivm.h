//VM即虚拟机的实现,在这里虚拟机表示的是字节码的执行环境,其内置了执行字节码的方法
#ifndef IS_IVM_H
#define IS_IVM_H

#include "../container/istack.h"
#include "../basic/instream.h"
#include "../basic/ibasic.h"

//寄存器的定义
//#define ip (reg[0].ip8) //指令寄存器(储存下一条指令头的指针)
//#define bp (reg[1].vp)  //栈底寄存器(是个指向栈第一个normalSlot的指针)
//#define sp (reg[2].vp)  //栈顶寄存器(是个指向栈第一个emptySlot的指针)
//#define up (reg[3].vp)  //栈头寄存器(是个指向栈第一个outSlot的指针,该位置不能使用)
//#define ax (reg[4])     //通用寄存器
//#define bx (reg[5])     //通用寄存器
//#define cx (reg[6])     //通用寄存器
//#define dx (reg[7])     //通用寄存器

//寄存器的索引及所指
namespace Regist {
	enum Regist : uint8_t {
		ip, bp, sp, up,
		ax, bx, cx, dx,
		r1, r2, r3, r4,
		r5, r6, r7, r8
	};
}//enum class定义的不能当数使,有些不方便

class VM {
public:
	Value reg[8]{};                 //寄存器
	ByteStream instream;            //指令流
	Array<Value> constList;         //常量表
	Array<Value> gvarList;          //全局变量表

	VM() = default;                 //默认构造(祖传/笑)

	void execute();                 //执行指令流

};

#endif //IS_IVM_H

//VM即虚拟机的实现,在这里虚拟机表示的是字节码的执行环境,其内置了执行字节码的方法
#ifndef IS_IVM_H
#define IS_IVM_H

#include "../container/istack.h"
#include "../basic/instream.h"
#include "../basic/ibasic.h"


class VM {
public:
	Value reg[8]{};                 //寄存器
	Stack<Value> stack{};           //主栈
	Instream in{};                  //指令流

	VM() = default;                 //默认构造(祖传/笑)
	explicit VM(ByteStream *bst);   //指定要执行的指令流

	void execute();                 //执行指令流

};

#endif //IS_IVM_H

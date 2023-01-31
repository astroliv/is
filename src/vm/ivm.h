//VM即虚拟机的实现,在这里虚拟机表示的是字节码的执行环境,其内置了执行字节码的方法
#ifndef IS_IVM_H
#define IS_IVM_H

#include "../container/istack.h"
#include "../basic/instream.h"
#include "../basic/ibasic.h"



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
	Value regist[8]{};                 //寄存器
	ByteStream instream;            //指令流
	Array<Value> constList;         //常量表
	Array<Value> gvarList;          //全局变量表

	VM() = default;                 //默认构造(祖传/笑)

	void execute();                 //执行指令流

	//下面是一些工具函数,用于读写指令

	void write(Bytecode type);                               //写字节码
	void write(int64_t operand, int8_t len);                 //写指定字节操作数
	void write(Bytecode type, int64_t operand, int8_t len);  //写指定字节操作数指令

	void push(Value value); //在bp,sp,up管理的栈中进行压入操作
	Value pop();            //在bp,sp,up管理的栈中进行弹出操作

	string dumpin();        //输出并步过ip对应指令

private:
	//这下面是一些内联函数,由于外部用不到所以设为private
	Bytecode read();                //读一个字节码并步过
	int64_t read(int8_t len);       //读指定长度的参数并步过

	void writeVarg(uint64_t arg);   //变长字节码参数的写入
	uint64_t readVarg();            //变长字节码参数的读取

};

#endif //IS_IVM_H

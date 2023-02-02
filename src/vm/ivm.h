//VM即虚拟机的实现,在这里虚拟机表示的是字节码的执行环境,其内置了执行字节码的方法
#ifndef IS_IVM_H
#define IS_IVM_H

#include "../container/istack.h"
#include "../basic/ibasic.h"

//寄存器的索引及所指
namespace Regist {
	enum Regist : uint8_t {
		ib, ip, sb, sp,
		su, ax, bx, cx,
		dx, ex, fx, gx,
		r1, r2, r3, r4
	};
}//enum class定义的不能当数使,有些不方便

class VM {
public:
	Value regist[16]{};             //寄存器
	Array<Value> constList;         //常量表
	Array<Value> gvarList;          //全局变量表

	VM() = default;                 //默认构造(祖传/笑)

	void initRegist(byte *_ib);     //初始化寄存器

//	void call(isize idx);

	void execute();                 //执行指令流

	void resize(isize newCap);  //在bp,sp,up管理的栈中进行重置大小
	void ensure(isize remain);  //在bp,sp,up管理的栈中确保剩余空间

	void push(Value value);     //在bp,sp,up管理的栈中进行压入操作
	Value pop();                //在bp,sp,up管理的栈中进行弹出操作

	isize capacity();       //计算栈总容量
	isize usedSize();       //计算栈已用容量

	string dumpin();            //输出并步过ip对应指令

private:

	//下面是一些工具函数,用于读取指令(与之相对的是Compiler里的写指令)
	//由于是些内联函数且外部用不到所以设为private
	Bytecode read();                //读一个字节码并步过
	int64_t read(int8_t len);       //读指定长度的参数并步过

	uint64_t readVarg();            //变长字节码参数的读取

};


#define VMRegist(vm, reg) (vm).regist[Regist::reg]
#define VMPRegist(vmp, reg) (vmp)->regist[Regist::reg]

#endif //IS_IVM_H
